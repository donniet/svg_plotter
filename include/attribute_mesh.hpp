#ifndef __ATTRIBUTE_MESH_HPP__
#define __ATTRIBUTE_MESH_HPP__

#include "mesh.hpp"
#include "utility.hpp"

#include <utility>
#include <vector>
#include <array>
#include <numeric>


template<typename T>
class AttributeAdapter;


template<size_t I, typename T>
double attribute_get(T const & a);

double get_element(Point const & p, size_t i);
double get_element(double const & p, size_t i);

template<typename Tuple, size_t I, size_t ... Is>
double row_element_helper(Tuple const & r, size_t attr, size_t el, std::index_sequence<I, Is...>);

template<typename Tuple, size_t I, size_t ... Is>
double row_element_helper(Tuple const & r, size_t attr, size_t el, std::index_sequence<I, Is...>);


class Uniform
{
public:
    std::string name() const;
};

template<typename ... Attrs>
class AttributeMesh
{
public:
    static size_t _stride;
    using value_type = std::tuple<Point, Attrs...>;

    static size_t triangle_strip_offsets[2][3];

    static size_t attribute_count;
    static size_t attribute_sizes[];

    static std::pair<size_t, size_t> attribute_index(size_t i);

    static size_t stride();
private:
    std::vector<value_type> _attributes;
    std::vector<Uniform> _uniforms;
    std::array<std::string, 1 + sizeof...(Attrs)> _names;
    DrawMode _mode;

    AttributeMesh(std::vector<value_type> && attributes,
                  std::vector<Uniform> const & uniforms,
                  std::array<std::string, 1 + sizeof...(Attrs)> const & names,
                  DrawMode mode);
        

    template<size_t I>
    struct AttributeType
    {
        using type = std::tuple_element<I, value_type>::type;
        using adapter = AttributeAdapter<type>;
    };

    template<size_t ... Is>
    void append_attributes(std::tuple<Attrs...> const & tup, std::index_sequence<Is...>);

    double row_element(size_t row, size_t col) const;

    template<size_t ... Is>
    auto row_helper(size_t i, std::index_sequence<Is...>) const;

public:
    friend class AttributeIterator;
    
    class AttributeIterator
    {
        friend class AttributeMesh<Attrs...>;
    private:
        AttributeMesh<Attrs...> const * _m;
        size_t _i;
        AttributeIterator(AttributeMesh<Attrs...> const * m, size_t i = 0);

    public:
        AttributeIterator();
        AttributeIterator(AttributeIterator &&) = default;
        AttributeIterator(AttributeIterator const &) = default;
        AttributeIterator & operator=(AttributeIterator &&) = default;
        AttributeIterator & operator=(AttributeIterator const &) = default;

        std::string const & name() const;
        size_t size() const;
        size_t offset() const;

        AttributeIterator & operator++();
        AttributeIterator   operator++(int);
        AttributeIterator const & operator*() const;
        AttributeIterator const * operator->() const;

        bool operator==(AttributeIterator const & r) const;
        bool operator!=(AttributeIterator const & r) const;

    };  

    struct AttributeRange 
    {
        AttributeIterator _begin;
        AttributeIterator _end;

        AttributeIterator const & begin() const;
        AttributeIterator const & end() const;
    };

    AttributeRange attributes() const ;

    AttributeMesh();

    template<typename ... Names>
    AttributeMesh(DrawMode mode, std::string position_name, Names ... names);

    value_type & operator[](size_t i);
    value_type const & operator[](size_t i) const;
    auto row(size_t i) const;

    friend class buffer_iterator;

    class buffer_iterator
    {
        friend class AttributeMesh<Attrs...>;
    private:
        AttributeMesh<Attrs...> const * _m;
        size_t _i;

        buffer_iterator(AttributeMesh<Attrs...> const * m, size_t i = 0);
    public:
        using value_type = double;
        using reference = double const &;
        using pointer = double const *;

        buffer_iterator();
        buffer_iterator(buffer_iterator &&) = default;
        buffer_iterator(buffer_iterator const &) = default;
        buffer_iterator & operator=(buffer_iterator &&) = default;
        buffer_iterator & operator=(buffer_iterator const &) = default;

        buffer_iterator & operator++();
        buffer_iterator   operator++(int);
        buffer_iterator & operator--();
        buffer_iterator   operator--(int);
        buffer_iterator & operator+=(int x);
        buffer_iterator & operator-=(int x);
        
        buffer_iterator   operator+ (int x) const;
        buffer_iterator   operator- (int x) const;

        int               operator- (buffer_iterator const & r);

        bool operator==(buffer_iterator const & r) const;
        bool operator!=(buffer_iterator const & r) const;

        double operator*() const;
    };

    size_t size() const { return _attributes.size(); }
    size_t buffer_size() const { return _attributes.size() * _stride; }
    buffer_iterator buffer_begin() const { return buffer_iterator(this, 0); }
    buffer_iterator buffer_end() const { return buffer_iterator(this, buffer_size()); }
    value_type & front() { return operator[](0); }
    value_type const & front() const { return operator[](0); }
    value_type & back() { return operator[](size() - 1); }
    value_type const & back() const { return operator[](size() - 1); }

    auto begin() { return _attributes.begin(); }
    auto begin() const { return _attributes.begin(); }
    auto end() { return _attributes.end(); }
    auto end() const { return _attributes.end(); }

    Point & vertex(size_t i)
    {
        return get<0>((*this)[i]);
    }
    Point const & vertex(size_t i) const
    {
        return get<0>((*this)[i]);
    }

    std::vector<Point> vertices() const
    {
        std::vector<Point> ret(_attributes.size());
        std::transform(std::execution::par_unseq,
                      _attributes.begin(), _attributes.end(),
                      ret.begin(),
        [](value_type const & v) -> Point {
            return get<0>(v);
        });

        return ret;
    }

    template<size_t ... Is>
    static bool call_predicate(size_t i, value_type const & tup, 
                        std::function<bool(size_t i, Point const & p, Attrs const & ...)> & pred,
                        std::index_sequence<Is...>)
    {
        return pred(i, get<Is>(tup)...);
    }

    AttributeMesh<Attrs...> submesh(std::function<bool(size_t i, Point const & p, Attrs const & ...)> && pred) const
    {
        if(_attributes.size() == 0)
            return *this;

        std::vector<size_t> result(_attributes.size());
        std::vector<size_t> scan(_attributes.size());

        std::transform(std::execution::par_unseq,
                      _attributes.begin(), _attributes.end(),
                      result.begin(),
        [&](value_type const & tup) -> size_t {
            size_t gid = std::distance(&_attributes[0], &tup);

            if(call_predicate(gid, tup, pred, std::make_index_sequence<1 + sizeof...(Attrs)>{}))
                return 1;

            return 0;
        });

        std::inclusive_scan(std::execution::par_unseq,
                            result.begin(), result.end(),
                            scan.begin());

        size_t found = scan.back();

        std::vector<value_type> sub(found);
        std::for_each(std::execution::par_unseq,
                      scan.begin(), scan.end(),
        [&](size_t & id) {
            size_t gid = std::distance(&scan[0], &id);

            if(result[gid] != 1)
                return;
            
            sub[id-1] = _attributes[gid];
        });

        return AttributeMesh<Attrs...>(std::move(sub), _uniforms, _names, _mode);
    }

    size_t triangle_count() const 
    {
        switch(_mode)
        {
        case DrawMode::triangle_strip:
            return size() < 2 ? 0 : size() - 2;
        case DrawMode::triangles:
        default:
            return size() / 3;
        }
    }
    
    Triangle triangle(size_t i) const 
    {        
        switch(_mode)
        {
        case DrawMode::triangle_strip:
            return Triangle(
                vertex(i + triangle_strip_offsets[i%2][0]),
                vertex(i + triangle_strip_offsets[i%2][1]),
                vertex(i + triangle_strip_offsets[i%2][2])
            );
        case DrawMode::triangles:
        default:
            return Triangle(
                vertex(3 * i + 0),
                vertex(3 * i + 1),
                vertex(3 * i + 2)
            );
        }
    }

    bool contains(Point const & p) const
    {
        for(size_t i = 0; i < triangle_count(); ++i)
            if(triangle(i).contains(p))
                return true;

        return false;
    }

    size_t append(Point const & p, Attrs const & ... attrs)
    {
        _attributes.emplace_back(p, attrs...);
        return _attributes.size() - 1;
    }
    size_t append(value_type const & r) 
    {
        _attributes.emplace_back(r);
        return _attributes.size() - 1;
    }


};


#endif