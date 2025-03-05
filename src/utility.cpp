#include "utility.hpp"


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
