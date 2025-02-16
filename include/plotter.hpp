#ifndef __PLOTTER_HPP__
#define __PLOTTER_HPP__

#include <execution>
#include <vector>
#include <algorithm>

#include "parser.hpp"
#include "point.hpp"

using std::vector;
using std::for_each;

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

struct Shape
{
    vector<Event> _outline;

    vector<Event>::const_iterator begin() const { return _outline.begin(); }
    vector<Event>::const_iterator end() const { return _outline.end(); }
};

struct Context 
{
    double draw_speed; // length/time
    size_t maximum_points;

    void validate()
    {
        if(draw_speed <= 0)
            draw_speed = 1;
    }

    Context(Context const & c) : 
        draw_speed(c.draw_speed)
    {
        validate();
    }
    Context() : draw_speed(1), maximum_points(100) { }
};

Shape plot(Drawable const & drawing, Context ctx = Context()) 
{
    using std::execution::par_unseq;

    vector<Event> points(ctx.maximum_points);

    double len = drawing.length();

    // first stab: just plot the points
    for_each(par_unseq, points.begin(), points.end(), [&](Event & e) {
        size_t gid = &e - &points[0];

        // t \in [0,1]
        double t = (double)gid / (double)(ctx.maximum_points - 1);

        Point p = drawing.at(t);

        e = Event(p, t * len / ctx.draw_speed);
    });

    return Shape{._outline = points};
}





#endif