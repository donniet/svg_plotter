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
};

struct Shape
{
    vector<Point> _outline;

    vector<Point>::const_iterator begin() const { return _outline.begin(); }
    vector<Point>::const_iterator end() const { return _outline.end(); }
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

    vector<Point> points(ctx.maximum_points);

    // first stab: just plot the points
    for_each(par_unseq, points.begin(), points.end(), [&](Point & p) {
        size_t gid = &p - &points[0];

        double t = (double)gid / (double)(ctx.maximum_points - 1);

        p = drawing.at(t);
    });

    return Shape{._outline = points};
}





#endif