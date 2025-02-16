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

    double norm() const { return sqrt(x * x + y * y); }

    Point operator-(Point const & p) const { return { x - p.x, y - p.y }; }
    Point operator+(Point const & p) const { return { x + p.x, y + p.y }; }
    Point & operator+=(Point const & p) 
    { 
        x += p.x;
        y += p.y;
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

Point operator*(double a, Point const & p) { return p * a; }
double abs(Point p) { return p.norm(); }

template<typename Iter>
double area(Iter begin, Iter end) 
{
    using std::execution::par_unseq;

    size_t count = end - begin;

    if(count < 3)
        return 0;

    vector<Point> first(count), second(count);
    vector<double> areas(count);

    copy(par_unseq, begin, end, first.begin());
    copy(par_unseq, begin+1, end, second.begin());
    second[first.size() - 1] = *begin;

    transform(par_unseq, 
              first.begin(), first.end(), 
              second.begin(),  areas.begin(), 
    [&](Point const & a, Point const & b) 
    {
        return a.x * b.y - a.y * b.x;
    });

    double ret = reduce(par_unseq, areas.begin(), areas.end());

    return ret * 0.5;
}

#endif