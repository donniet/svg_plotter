#ifndef __POINT_HPP__
#define __POINT_HPP__

#include <tuple>
#include <vector>
#include <math.h>
#include <execution>

using std::tuple;
using std::get;
using std::vector;

struct Point
{
    double x, y;

    Point(double x0, double y0) : 
        x(x0), y(y0) 
    { }

    Point() : 
        x(0), y(0) 
    { }

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

    double dot(Point p) const { return x * p.x + y * p.y; }
    
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
};

struct Vector : public Point { };

struct Event :
    public Point 
{
    double t;
    

    Event(double x0, double y0, double t0) :
        Point(x0, y0), t(t0)
    { }
    Event(Point p0, double t0) :
        Point(p0), t(t0)
    { }
    Event() : 
        Point(), t(0)
    { }

    Event operator-(Event const & e) const { 
        return { (Point)*this - (Point)e, t - e.t };
    }
    Event operator+(Event const & e) const {
        return { (Point)*this + (Point)e, t + e.t };
    }
    Event & operator+=(Event const & e) { 
        (Point)*this += (Point)e;
        t += e.t;
        return *this;
    }
    Event & operator-=(Event const & e) {
        (Point)*this -= (Point)e;
        t -= e.t;
        return *this;
    }

    Event operator*(double a) const {
        return { (Point)*this * a, t * a };
    }
    Event operator/(double a) const {
        return { (Point)*this / a, t / a };
    }

    double dot(Event e) const {
        // Minikowski
        return ((Point)*this).dot((Point)e) - t * e.t;
    }

};

struct BoundingBox 
{
    Point p0, p1;

    double area() const
    {
        Point a = (p1 - p0);
        return a.x * a.y;
    }
    double perimeter() const {
        Point a = (p1 - p0);
        return 2 * a.x + 2 * a.y;
    }

    Vector diagonal() const {
        return Vector(p1 - p0);
    }

    vector<Point> corners() const
    {
        return {
            Point{p0.x, p0.y},
            Point{p0.x, p1.y},
            Point{p1.x, p1.y},
            Point{p1.x, p0.y}
        };
    }

    BoundingBox(Point mi, Point ma) :
        p0(mi), p1(ma)
    { }
    BoundingBox() { }
};

struct Velocity :
    public Event
{
    Velocity(Event from, Event to) : 
        Event((static_cast<Point>(to) - static_cast<Point>(from))/(to.t - from.t), from.t)
    { }
    Velocity() : Event() { }
};

struct Acceleration :
    public Event
{
    Acceleration(Velocity from, Velocity to) :
        Event((static_cast<Point>(to) - static_cast<Point>(from))/(to.t - from.t), from.t)
    { }

    Acceleration() : Event() { }
};

Point operator*(double a, Point const & p) { return p * a; }
double abs(Point p) { return p.norm(); }

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

template<typename Iter>
double path_length(Iter begin, Iter end, bool closed = true)
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

    if(!closed)
        return len - lengths[count-1];

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
double area(Iter begin, Iter end) 
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

#endif