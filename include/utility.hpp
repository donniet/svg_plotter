#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

#include "point.hpp"

#include <type_traits>
#include <iterator>
#include <utility>
#include <numeric>
#include <execution>
#include <vector>
#include <tuple>
#include <stdexcept>

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);

template<size_t Start, typename OutputArray, typename InputArray, size_t ... Is>
void static_fill(OutputArray & out, InputArray const & in, std::index_sequence<Is...>)
{
    ((out[Start + Is] = in[Is]), ...);
}

template<size_t Start, typename T, size_t Sum, size_t ... Ns>
void array_cat_helper(std::array<T, Sum> & arr, std::array<T, Ns> const & ... As);

template<size_t Start, typename T, size_t Sum>
void array_cat_helper(std::array<T, Sum> & arr)
{ }

template<size_t Start, typename T, size_t Sum, size_t N, size_t ... Ns>
void array_cat_helper(std::array<T, Sum> & arr, std::array<T, N> const & A, std::array<T, Ns> const & ... As)
{
    static_fill<Start, Sum>(arr, A, std::make_index_sequence<N>{});

    array_cat_helper<Start + N>(arr, As...);
}

template<typename T, size_t ... Ns>
std::array<T, (Ns + ...)> array_cat(std::array<T, Ns> const & ... As)
{
    std::array<T, (Ns + ...)> ret;

    array_cat_helper<0>(ret, As...);

}

template<typename Tuple, size_t ... Is>
void const * runtime_tuple_element_helper(Tuple const & t, size_t i, std::index_sequence<Is...>)
{
    void const * all[] = { &get<Is>(t) ... };

    return all[i];
}

template<typename Tuple>
void const * runtime_tuple_element(Tuple const & t, size_t i)
{
    return runtime_tuple_element_helper(t, i, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
}


template<typename Iter>
struct PairwiseCircularAdapter
{
    using value_type = std::pair<typename Iter::value_type const &, typename Iter::value_type const &>;

    Iter _begin, _end;
    
    struct const_iterator 
    {
        Iter _i;
        PairwiseCircularAdapter const * _adapter;
        using iterator_category = typename Iter::iterator_category;
        using value_type = PairwiseCircularAdapter<Iter>::value_type;
        using difference_type = typename Iter::difference_type;
        using pointer = value_type*;
        using reference = value_type&;

        const_iterator() : _adapter(nullptr) { }
        const_iterator(Iter i, PairwiseCircularAdapter const & adapter) : 
            _i(i), _adapter(&adapter) 
        { }

        const_iterator(const_iterator const & i) :
            _i(i._i), _adapter(i._adapter)
        { }
        const_iterator(const_iterator && i) :
            _i(i._i), _adapter(i._adapter)
        { }

        bool operator==(const_iterator const & r) 
        {
            // _adapter being null signifies end()

            if(_adapter == nullptr && r._adapter == nullptr)
                return true;

            if(_adapter == nullptr && r._i == r._adapter->_end)
                return true;

            if(_i == _adapter->_end && r._adapter == nullptr)
                return true;

            if(_adapter == nullptr || r._adapter == nullptr)
                return false;

            return _i == r._i;
        }
        bool operator!=(const_iterator const & r) 
        {
            return !(_i == r._i);
        }

        const_iterator & operator=(const_iterator const & i)
        {
            _i = i._i;
            return *this;
        }
        const_iterator & operator=(const_iterator && i)
        {
            _i = i._i;
            return *this;
        }
        const_iterator & operator++()
        {
            ++_i;
            return *this;
        }
        const_iterator operator++(int)
        {
            const_iterator r = *this;
            operator++();
            return r;
        }
        typename Iter::difference_type operator-(const_iterator const & r)
        {
            return _i - r._i;
        }
        
        value_type operator*()
        {
            auto j = _i;
            j++;

            if(j == _adapter->_end)
                j = _adapter->_begin;
            
            return { *_i, *j };
        }
    };


    PairwiseCircularAdapter(Iter begin, Iter end) :
        _begin(begin), _end(end)
    { }

    const_iterator begin() const 
    {
        return const_iterator(_begin, *this);
    }
    const_iterator end() const 
    {
        return const_iterator(_end, *this);
    }

    size_t size() const 
    {
        return distance(_begin, _end);
    }

};

template<typename Iter>
struct NextPointRange 
{
    Iter _original_begin;
    Iter _original_end;
    bool closed;

    NextPointRange(Iter begin, Iter end, bool closed = true) :
        _original_begin(begin), _original_end(end)
    { }

    struct iterator 
    {
        Iter _original_begin;
        Iter _original_end;
        Iter _prev;

        using value_type = typename Iter::value_type;
        using difference_type = typename Iter::difference_type;

        iterator() : 
            _original_begin(), _original_end(_original_begin), _prev(_original_end) 
        { }

        iterator(iterator const & i) :
            _original_begin(i._original_begin), _original_end(i._original_end), _prev(i._prev)
        { }

        iterator(Iter original_begin, Iter original_end, Iter prev) :
            _original_begin(original_begin), _original_end(original_end), _prev(prev)
        { }

        bool operator==(iterator const & i) const
        {
            if(_prev == i._prev)
                return true;

            if(_prev == _original_end && i._prev == i._original_end)
                return true;

            return false;
        }
        bool operator!=(iterator const & i) const 
        {
            return !(*this == i);
        }
        iterator & operator=(iterator const & i)
        {
            _original_begin = i._original_begin;
            _original_end = i._original_end;
            _prev = i._prev;
            return *this;
        }
        iterator & operator++()
        {  
            if(_prev != _original_end)
                ++_prev;

            return *this; 
        }
        iterator operator++(int)
        {
            iterator r = *this;
            operator++();
            return r;
        }
        value_type operator*()
        {
            if(_prev == _original_end)
                return *_prev;

            Iter c = _prev;
            ++c;

            if(c == _original_end)
                return *_original_begin;

            return *c;
        }
        
    };

    iterator begin() const 
    {
        return iterator(_original_begin, _original_end, _original_begin);
    }
    iterator end() const
    {
        return iterator(_original_begin, _original_end, _original_end);
    }
};


template<typename RandomAccessIterator, 
         std::enable_if_t< 
            std::is_same_v< 
                Event, 
                typename std::iterator_traits<RandomAccessIterator>::value_type 
            >, bool > = true>
double path_length(RandomAccessIterator begin, RandomAccessIterator end)
{
    using std::execution::par_unseq;

    size_t count = distance(begin, end);

    if(count < 2)
        return 0;

    std::vector<double> lengths(count - 1);

    NextPointRange<RandomAccessIterator> next(begin, end);
    RandomAccessIterator fin = end;
    --fin;

    transform(par_unseq, 
              begin, fin, // skip the last one
              next.begin(),
              lengths.begin(), 
    [&](Event & p0, Event & p1) -> double {
        if(p1.is_move_to())
            return 0;

        return (p1 - p0).norm();
    });

    double len = std::reduce(par_unseq, lengths.begin(), lengths.end());

    return len;
}


template<typename Iter, 
         std::enable_if_t< 
            std::is_same_v< 
                Point, 
                typename std::iterator_traits<Iter>::value_type 
            >, bool > = true>
double path_length(Iter begin, Iter end)
{
    using std::execution::par_unseq;

    size_t count = distance(begin, end);

    if(count < 2)
        return 0;

    std::vector<double> lengths(count);
    NextPointRange next(begin, end);

    transform(par_unseq, 
              begin, end, 
              next.begin(), 
              lengths.begin(), 
    [&](Point const & p0, Point const & p1) {
        return (p0 - p1).norm();
    });

    double len = std::reduce(par_unseq, lengths.begin(), lengths.end());

    return len;
}

/* 
calculates the area of a shape made from a series of points using stoke's 
theorem 

Description: take points in pairs from the list. Calculate the signed area of 
the paralellagram made from the origin and our two points using the cross 
product.  The sum of all these signed areas will be twice the signed area of 
the closed shape enclosed by the points by stoke's theorem.  Divide by 2
before returning.

*/
template<typename Iter>
double enclosed_area(Iter begin, Iter end) 
{
    using std::execution::par_unseq;

    size_t count = distance(begin, end);

    if(count < 3)
        return 0;

    NextPointRange next(begin, end);

    // // allocate temporary space for working in parallel
    // vector<Point> first(count), second(count);
    std::vector<double> areas(count);

    // calculate the length of the cross product of first and second which 
    // is equal to the signed area of the parallelgram spanned by 
    // x and y
    transform(par_unseq, 
              begin, end,
              next.begin(), 
              areas.begin(),
    [&](Point const & a, Point const & b) -> double
    {
        return a.x * b.y - a.y * b.x;
    });

    // add up all the signed areas
    double ret = std::reduce(par_unseq, areas.begin(), areas.end());

    // divide by two because we want the area of the triangle formed by O-X-Y
    // and not the parallelgram formed by O-X-{X+Y}-Y
    return ret * 0.5;
}

std::pair<double,double> intersect_intervals(std::pair<double,double> a, std::pair<double,double> b);
std::pair<bool,double> segment_intersects_horizontal_ray(Point p0, Point p1, Point ray_origin, double epsilon = 1e-5);
std::pair<bool,double> segment_intersects_vertical_ray(Point p0, Point p1, Point ray_origin, double eps);
Point from_poly_index(std::vector<std::vector<Point>> const & poly, size_t index);
std::pair<size_t, size_t> parse_poly_index(std::vector<std::vector<Point>> const & poly, size_t index);


#endif