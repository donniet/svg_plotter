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

template<typename ... Ts>
class AttributeAdapter<std::tuple<Ts...>>
{
    static inline constexpr size_t size = sizeof...(Ts);
    using row_type = std::tuple<Ts...>;
    using attribute_type = std::tuple<Ts...>;

    template<size_t I>
    static double get(attribute_type const & t)
    {
        return std::get<I>(t);
    }
    
    static row_type row(attribute_type const & a) { return a; }
};

template<>
class AttributeAdapter<double>
{
public:
    static inline constexpr size_t size = 1;
    using row_type = std::array<double,1>;
    using attribute_type = double;

    template<size_t I>
    static double const & get(attribute_type const & x) { return x; }
    static double const & get(attribute_type const & x, size_t i) 
    { return x; }

    static row_type row(attribute_type const & a) { return row_type{a}; }
};

template<>
class AttributeAdapter<size_t>
{
public:
    static inline constexpr size_t size = 1;
    using row_type = std::array<double,1>;
    using attribute_type = size_t;

    template<size_t I>
    static size_t const & get(attribute_type const & x) { return x; }
    static size_t const & get(attribute_type const & x, size_t i) 
    { return x; }

    static row_type row(attribute_type const & a) { return row_type{(double)a}; }
};

template<>
class AttributeAdapter<Point>
{
public:
    static inline constexpr size_t size = 2;
    using row_type = std::array<double,2>;
    using attribute_type = Point;

    template<size_t I>
    static double const & get(Point const & p)
    {
        return std::get<I>(p);
    }

    static double const & get(Point const & p, size_t i)
    {
        if(i == 0)
            return p.x;

        return p.y;
    }

    static row_type row(attribute_type const & a) { return row_type{a.x, a.y}; }

};



template<size_t I, typename T>
double attribute_get(T const & a)
{
    return AttributeAdapter<T>::template get<I>(a);
}

template<typename T>
double get_element(T const & p, size_t i)
{
    switch(i)
    {
    case  0: return attribute_get< 0>(p);
    case  1: return attribute_get< 1>(p);
    case  2: return attribute_get< 2>(p);
    case  3: return attribute_get< 3>(p);
    case  4: return attribute_get< 4>(p);
    case  5: return attribute_get< 5>(p);
    case  6: return attribute_get< 6>(p);
    case  7: return attribute_get< 7>(p);
    case  8: return attribute_get< 8>(p);
    case  9: return attribute_get< 9>(p);
    case 10: return attribute_get<10>(p);
    case 11: return attribute_get<11>(p);
    case 12: return attribute_get<12>(p);
    case 13: return attribute_get<13>(p);
    case 14: return attribute_get<14>(p);
    case 15: return attribute_get<15>(p);
    default:
        return attribute_get<0>(p);
    }
}

template<typename Tuple, size_t I, size_t ... Is>
double row_element_helper(Tuple const & r, size_t attr, size_t el, std::index_sequence<I, Is...>);

template<typename Tuple>
double row_element_helper(Tuple const & r, size_t attr, size_t el, std::index_sequence<>)
{
    return 0;
}

template<typename Tuple, size_t I, size_t ... Is>
double row_element_helper(Tuple const & r, size_t attr, size_t el, std::index_sequence<I, Is...>)
{
    if(I == attr)
        return get_element(get<I>(r), el);

    return row_element_helper(r, attr, el, std::index_sequence<Is...>{});
}


class Uniform
{
private:
    std::string _name;
public:
    std::string name() const
    {
        return _name;
    }
};

template<typename ... Attrs>
class AttributeMesh
{
public:
    static constexpr size_t _stride = (2 + (AttributeAdapter<Attrs>::size + ...));
    using value_type = std::tuple<Point, Attrs...>;
    using row_type = std::array<double, _stride>;

    static inline constexpr size_t triangle_strip_offsets[2][3] = {
        { 0, 1, 2 }, // even case
        { 1, 0, 2 }  // odd case
    };

    static constexpr size_t attribute_count = 1 + sizeof...(Attrs);
    static inline constexpr size_t attribute_sizes[] = { 2, AttributeAdapter<Attrs>::size... };

    static constexpr std::pair<size_t, size_t> attribute_index(size_t i)
    {
        i = i % _stride;
        size_t j = 0;

        for(; i >= attribute_sizes[j]; i -= attribute_sizes[j++])
        { }

        return {j, i};
    }

    static constexpr size_t stride() 
    {
        return _stride;
    }
private:
    std::vector<value_type> _attributes;
    std::vector<Uniform> _uniforms;
    std::array<std::string, 1 + sizeof...(Attrs)> _names;
    DrawMode _mode;

    AttributeMesh(std::vector<value_type> && attributes,
                  std::vector<Uniform> const & uniforms,
                  std::array<std::string, 1 + sizeof...(Attrs)> const & names,
                  DrawMode mode) :
        _attributes(std::move(attributes)), _uniforms(uniforms),
        _names(names), _mode(mode)
    { }
        

    template<size_t I>
    struct AttributeType
    {
        using type = std::tuple_element<I, value_type>::type;
        using adapter = AttributeAdapter<type>;
    };

    template<size_t ... Is>
    void append_attributes(std::tuple<Attrs...> const & tup, std::index_sequence<Is...>)
    {
        (get<Is>(_attributes).append(get<Is>(tup)), ...);
    }

    

    double row_element(size_t row, size_t col) const
    {   
        auto n = attribute_index(col);

        return row_element_helper(_attributes[row], n.first, n.second, std::make_index_sequence<attribute_count>{});
    }

    template<size_t ... Is>
    row_type row_helper(size_t i, std::index_sequence<Is...>) const
    {
        return array_cat(
            AttributeType<Is>::adapter::row(get<Is>(_attributes[i])) ...
        );
    }

public:
    friend class AttributeIterator;
    
    class AttributeIterator
    {
        friend class AttributeMesh<Attrs...>;
    private:
        AttributeMesh<Attrs...> const * _m;
        size_t _i;
        AttributeIterator(AttributeMesh<Attrs...> const * m, size_t i = 0) :
            _m(m), _i(i)
        { }

    public:
        AttributeIterator() : _m(nullptr), _i(0) { }
        AttributeIterator(AttributeIterator &&) = default;
        AttributeIterator(AttributeIterator const &) = default;
        AttributeIterator & operator=(AttributeIterator &&) = default;
        AttributeIterator & operator=(AttributeIterator const &) = default;

        std::string const & name() const
        {
            return _m->_names[_i];
        }
        size_t size() const
        {
            return AttributeMesh<Attrs...>::attribute_sizes[_i];
        }
        size_t offset() const
        {
            return std::reduce(&AttributeMesh<Attrs...>::attribute_sizes[0], &AttributeMesh<Attrs...>::attribute_sizes[0] + _i, 0);
        }

        AttributeIterator & operator++() { ++_i; return *this; }
        AttributeIterator & operator++(int) { AttributeIterator r = *this; ++(*this); return r; }
        AttributeIterator const & operator*() const { return *this; }
        AttributeIterator const * operator->() const { return this; }

        bool operator==(AttributeIterator const & r) const
        {
            return _m == r._m && _i == r._i;
        }
        bool operator!=(AttributeIterator const & r) const
        {
            return _m != r._m || _i != r._i;
        }

    };  

    struct AttributeRange 
    {
        AttributeIterator _begin;
        AttributeIterator _end;

        AttributeIterator const & begin() const { return _begin; }
        AttributeIterator const & end() const { return _end; }
    };

    AttributeRange attributes() const 
    {
        return AttributeRange{
            ._begin = AttributeIterator(this, 0),
            ._end = AttributeIterator(this, attribute_count)
        };
    }

    std::vector<Uniform> const & uniforms() const { return _uniforms; }
    std::vector<Uniform> & uniforms() { return _uniforms; }

    AttributeMesh() : _mode(DrawMode::triangles), _names({"position"}) { }

    template<typename ... Names>
    AttributeMesh(DrawMode mode, std::string position_name, Names ... names) :
        _mode(mode), _names{position_name, names ...}
    { }

    value_type & operator[](size_t i)
    {
        return _attributes[i];
    }
    value_type const & operator[](size_t i) const
    {
        return _attributes[i];
    }
    row_type row(size_t i) const
    {
        return row_helper(i, std::make_index_sequence<sizeof...(Attrs)>{});
    }

    friend class buffer_iterator;

    class buffer_iterator
    {
        friend class AttributeMesh<Attrs...>;
    private:
        AttributeMesh<Attrs...> const * _m;
        size_t _i;

        buffer_iterator(AttributeMesh<Attrs...> const * m, size_t i = 0) :
            _m(m), _i(i)
        { }
    public:
        using value_type = double;
        using reference = double const &;
        using pointer = double const *;

        buffer_iterator() : _m(nullptr), _i(0) { }
        buffer_iterator(buffer_iterator &&) = default;
        buffer_iterator(buffer_iterator const &) = default;
        buffer_iterator & operator=(buffer_iterator &&) = default;
        buffer_iterator & operator=(buffer_iterator const &) = default;

        buffer_iterator & operator++()      { ++_i; return *this; }
        buffer_iterator   operator++(int)   { buffer_iterator r = *this; ++_i; return r; }
        buffer_iterator & operator--()      { --_i; return *this; }
        buffer_iterator   operator--(int)   { buffer_iterator r = *this; --_i; return r; }
        buffer_iterator & operator+=(int x) { _i += x; return *this; }
        buffer_iterator & operator-=(int x) { _i -= x; return *this; }
        
        buffer_iterator   operator+ (int x) const { buffer_iterator r = *this; return r += x; }
        buffer_iterator   operator- (int x) const { buffer_iterator r = *this; return r -= x; }

        int               operator- (buffer_iterator const & r) { return _i - r._i; }

        bool operator==(buffer_iterator const & r) const
        {
            return _m == r._m && _i == r._i;
        }
        bool operator!=(buffer_iterator const & r) const
        {
            return !(*this == r);
        }

        double operator*() const {
            size_t row = _i / _stride;
            size_t col = _i % _stride;

            return _m->row_element(row, col);
        }
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

    class triangle_const_iterator
    {
    private:
        size_t _i;
        AttributeMesh<Attrs...> const * _m;
        Triangle _c;

    public:
        using value_type = Triangle;
        using reference = Triangle const &;
        using pointer = Triangle const *;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = size_t;

        Triangle operator*() const 
        { 
            return _m->triangle(_i); 
        }
        Triangle const * operator->() const
        {
            _c = _m->triangle(_i);
            return &_c;
        }
        triangle_const_iterator& operator++()
        {
            ++_i;
            return *this;
        }
        triangle_const_iterator operator++(int)
        {
            triangle_const_iterator r = *this;
            ++*this;
            return r;
        }
        triangle_const_iterator operator+=(size_t d) 
        {
            _i += d;
            return *this;
        }

        bool operator==(triangle_const_iterator const & a) const
        {
            // general equality
            if(_m == a._m && _i == a._i) 
                return true;

            // both end iterators
            if(_m == nullptr && a._m == nullptr)
                return true;
            
            if(_m == nullptr && a._i == a._m->triangle_count())
                return true;

            if(a._m == nullptr && _i == _m->triangle_count())
                return true;

            return false;
        }

        bool operator!=(triangle_const_iterator const & a) const
        {
            return !(*this == a);
        }

        triangle_const_iterator() : _i(0), _m(nullptr) { }
        triangle_const_iterator(AttributeMesh<Attrs...> const & m, size_t i = 0) : _i(i), _m(&m) { }
        triangle_const_iterator(triangle_const_iterator const &) = default;
        triangle_const_iterator(triangle_const_iterator &&) = default;
        triangle_const_iterator& operator=(triangle_const_iterator const &) = default;
        triangle_const_iterator& operator=(triangle_const_iterator &&) = default;

    };

    triangle_const_iterator triangles_begin() const 
    {
        return triangle_const_iterator(*this, 0);
    }
    triangle_const_iterator triangles_end() const 
    {
        return triangle_const_iterator();
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