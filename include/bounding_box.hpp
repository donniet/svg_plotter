#ifndef __BOUNDING_BOX_HPP__
#define __BOUNDING_BOX_HPP__

#include "point.hpp"
#include "cover.hpp"

#include <vector>

using std::vector;

struct BoundingBox :
    public Cover
{
    Point p0, p1;

    virtual double area() const override
    {
        Point a = (p1 - p0);
        return a.x * a.y;
    }
    virtual double perimeter() const override
    {
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

    virtual Path outline() const override 
    { 
        return Path(corners()); 
    }

    virtual bool is_inside(Point const & p) const override
    {
        return p.x >= p0.x + margin() && p.x <= p1.x - margin() &&
               p.y >= p0.y + margin() && p.y <= p1.y - margin(); 
    }
    virtual pair<bool,double> intersect_ray(Point const & origin, Vector const & ray) const override
    {
        // line formula:
        // f(t) = origin + t * ray


        // is the ray horizontal?
        if(ray.is_parallel(Vector{1,0})) 
        {
            if(origin.y < p0.y || origin.y > p1.y)
                return {false, 0};

            // only worry about x
            double t0 = (p0.x - origin.x) / ray.x;
            double t1 = (p1.x - origin.x) / ray.x;

            if(t0 > t1)
                swap(t0, t1);

            if(t1 <= 0)
                return {false, 0};
            
            if(t0 <= 0)
                return {true, t1};

            return {true, t0};
        }

        double t0 = (p0.y - origin.y) / ray.y;
        double t1 = (p1.y - origin.y) / ray.y;

        if(t0 > t1)
            swap(t0, t1);

        // is the ray vertical?
        if(ray.is_parallel(Vector{0,1})) 
        {
            if(origin.x < p0.x || origin.x > p1.x)
                return {false, 0};

            if(t1 <= 0)
                return {false, 0};

            if(t0 <= 0)
                return {true, t1};

            return {true, t0};
        }

        double u0 = (p0.x - origin.x) / ray.x;
        double u1 = (p1.x - origin.x) / ray.x;

        if(u0 > u1)
            swap(u0, u1);

        // do these ranges overlap?
        pair<double,double> intersection = intersect_intervals({t0, t1}, {u0, u1});

        if(intersection.first > intersection.second)
            return {false, 0};

        if(intersection.second <= 0)
            return {false, 0};

        if(intersection.first <= 0)
            return {true, intersection.second};

        return {true, intersection.first};
    }

    BoundingBox(Point q0, Point q1) :
        p0(min(q0.x, q1.x), min(q0.y, q1.y)), 
        p1(max(q0.x, q1.x), max(q0.y, q1.y))
    { }
    BoundingBox(double x0, double y0, double x1, double y1) :
        p0(min(x0, x1), min(y0, y1)),
        p1(max(x0, x1), max(y0, y1))
    { }
    BoundingBox() :
        p0(0,0), p1(0,0)
    { }
};


#endif