#ifndef __POINT_HPP__
#define __POINT_HPP__

#include <tuple>
#include <math.h>

using std::tuple;
using std::get;

struct Point : public tuple<double,double> 
{
    Point(double x0, double y0) : tuple<double,double>(x0, y0) { }
    Point() : tuple<double,double>() { }

    double   x() const { return get<0>(*this); }
    double   y() const { return get<1>(*this); }
    double & x()       { return get<0>(*this); }
    double & y()       { return get<1>(*this); }

    double norm() const { return sqrt(x() * x() + y() * y()); }

    Point operator-(Point const & p) const { return { x() - p.x(), y() - p.y() }; }
    Point operator+(Point const & p) const { return { x() + p.x(), y() + p.y() }; }
    Point & operator+=(Point const & p) 
    { 
        x() += p.x();
        y() += p.y();
        return *this;
    }
    Point operator*(double a) const { return {a * x(), a * y() }; }
    Point operator/(double a) const { return { x() / a, y() / a }; }

    double dot(Point p) const { return x() * p.x() + y() * p.y(); }
    Point reflect(Point o = {0,0})
    {
        Point v = o - *this;
        return o + v;
    }
    double angle(Point v) const {
        double s = 1;
        if(x() * v.y() - y() * v.x() < 0)
            s = -1;

        return s * acos(dot(v) / norm() / v.norm());
    }
};

Point operator*(double a, Point const & p) { return p * a; }
double abs(Point p) { return p.norm(); }

#endif