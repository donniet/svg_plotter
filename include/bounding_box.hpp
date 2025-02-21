#ifndef __BOUNDING_BOX_HPP__
#define __BOUNDING_BOX_HPP__

#include "point.hpp"

#include <vector>

using std::vector;

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


#endif