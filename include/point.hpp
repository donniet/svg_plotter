#ifndef __POINT_HPP__
#define __POINT_HPP__

#include <tuple>
#include <vector>
#include <math.h>
#include <execution>
#include <tuple>
#include <numeric>
#include <utility>
#include <type_traits>
#include <iterator>


using std::tuple;
using std::get;
using std::vector;
using std::pair;
using std::max, std::min;
using std::numeric_limits;
using std::swap;
using std::enable_if_t, std::is_same_v;
using std::iterator_traits;

struct Point
{
    double x, y;

    Point(double x0, double y0) : 
        x(x0), y(y0) 
    { }

    Point() : 
        x(0), y(0) 
    { }

    Point(Point const & r) = default;
    Point(Point && r) = default;
    
    Point & operator=(Point const & r)
    {
        x = r.x;
        y = r.y;
        return *this;
    }
    Point & operator=(Point && r)
    {
        x = r.x;
        y = r.y;
        return *this;
    }

    double norm2() const { return dot(*this); }
    double norm() const { return sqrt(norm2()); }

    Point operator-(Point const & p) const { return { x - p.x, y - p.y }; }
    Point operator+(Point const & p) const { return { x + p.x, y + p.y }; }
    Point & operator+=(Point const & p) 
    { 
        x += p.x;
        y += p.y;
        return *this;
    }
    Point & operator-=(Point const & p) 
    { 
        x -= p.x;
        y -= p.y;
        return *this;
    }
    Point operator*(double a) const { return {a * x, a * y }; }
    Point operator/(double a) const { return { x / a, y / a }; }
    Point & operator/=(double a)
    {
        x /= a;
        y /= a;
        return *this;
    }

    double dot(Point p) const { return x * p.x + y * p.y; }


    Point normalized() const
    {
        return *this / norm();
    }
    bool is_parallel(Point v, double epsilon = 1e-5) const
    {
        v = v.normalized();
        Point u = normalized();

        if(1.0 - abs(v.dot(u)) < epsilon)
            return true;
        
        return false;
    }
    
    Point reflect(Point o = {0,0})
    {
        Point v = o - *this;
        return o + v;
    }
    double angle(Point v) const {
        double s = 1;
        if(x * v.y - y * v.x < 0)
            s = -1;

        return s * acos(dot(v) / norm() / v.norm());
    }

    bool operator==(Point const & p) const 
    {
        return x == p.x && y == p.y;
    }
    bool operator!=(Point const & p) const 
    {
        return !operator==(p);
    }
};


using Vector = Point;


struct Event :
    public Point 
{
    Vector dp;
    double dt;

    Event(double x0, double y0, double dx0 = 0., double dy0 = 0., double dt0 = 1.) :
        Point(x0, y0), dp(dx0, dy0), dt(dt0)
    { }
    Event(Point p0, double dx0 = 0., double dy0 = 0., double dt0 = 1.) :
        Point(p0), dp(dx0, dy0), dt(dt0)
    { }
    Event(Point p0, Vector v0, double dt0 = 1.) :
        Point(p0), dp(v0), dt(dt0)
    { }
    Event() : 
        Point(), dp(0,0), dt(0)
    { }

    Vector dp_dt() const
    {
        if(is_move_to())
            return Vector(0,0);

        return dp / dt;
    }

    bool is_move_to() const 
    { 
        return dt <= 0.; // a time derivative of zero or less isn't realistic and represents a move
    }
};

void make_move_to(Event & e)
{
    e.dt = 0;
}

Point operator*(double a, Point const & p) { return p * a; }
double abs(Point p) { return p.norm(); }


namespace std {
    string to_string(Point const & p)
    {
        return format("({},{})", p.x, p.y);
    }
    string to_string(Event const & p)
    {
        if(p.is_move_to())
            return format("move({},{})", p.x, p.y);

        return format("draw({},{},{})", p.x, p.y, p.dt);
    }
}

template<typename Iter>
struct NextPointRange 
{
    Iter _original_begin;
    Iter _original_end;

    NextPointRange(Iter begin, Iter end) :
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
         enable_if_t< 
            is_same_v< 
                Event, 
                typename iterator_traits<RandomAccessIterator>::value_type 
            >, bool > = true>
double path_length(RandomAccessIterator begin, RandomAccessIterator end)
{
    using std::execution::par_unseq;

    size_t count = distance(begin, end);

    if(count < 2)
        return 0;

    vector<double> lengths(count - 1);

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

    double len = reduce(par_unseq, lengths.begin(), lengths.end());

    return len;
}


template<typename Iter, 
         enable_if_t< 
            is_same_v< 
                Point, 
                typename iterator_traits<Iter>::value_type 
            >, bool > = true>
double path_length(Iter begin, Iter end)
{
    using std::execution::par_unseq;

    size_t count = distance(begin, end);

    if(count < 2)
        return 0;

    vector<double> lengths(count);
    NextPointRange next(begin, end);

    transform(par_unseq, 
              begin, end, 
              next.begin(), 
              lengths.begin(), 
    [&](Point const & p0, Point const & p1) {
        return (p0 - p1).norm();
    });

    double len = reduce(par_unseq, lengths.begin(), lengths.end());

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
    vector<double> areas(count);

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
    double ret = reduce(par_unseq, areas.begin(), areas.end());

    // divide by two because we want the area of the triangle formed by O-X-Y
    // and not the parallelgram formed by O-X-{X+Y}-Y
    return ret * 0.5;
}

pair<double,double> intersect_intervals(pair<double,double> a, pair<double,double> b)
{
    return {max(a.first, b.first), min(a.second, b.second)};
}

pair<bool,double> segment_intersects_horizontal_ray(Point p0, Point p1, Point ray_origin)
{
    p0 -= ray_origin;
    p1 -= ray_origin;

    // ensure p0 has a lower y value than p1
    if(p0.y > p1.y) 
        swap(p0, p1);

    pair<bool,double> false_value{false,0.};

    // is our ray in the right y-range?
    if(p0.y > 0 || p1.y < 0)
        return false_value;
    
    // is the segment horizontal?
    if(p0.y == p1.y) {
        if(p0.x >= 0 && p1.x >= 0)
            return {true, min(p0.x, p1.x)};
        
        if(p0.x >= 0)
            return {true, p0.x};

        if(p1.x >= 0)
            return {true, p1.x};
            
        return false_value;
    }

    // calculate x-intercept
    double t = -p0.y / (p1.y - p0.y);

    if(t < 0. || t > 1.)
        return false_value;

    // is this to the right of our point?
    double x = p0.x + t * (p1.x - p0.x);

    if(x < 0)
        return false_value;

    return { true, x };
}

#endif