#include "utility.hpp"


pair<double,double> intersect_intervals(pair<double,double> a, pair<double,double> b)
{
    return {max(a.first, b.first), min(a.second, b.second)};
}

pair<bool,double> segment_intersects_horizontal_ray(Point p0, Point p1, Point ray_origin, double eps)
{
    /*
        r(t) = o +   (1;0) * t
        s(u) = p + (q - p) * u

        with t >= 0
        u \in [0,1)

        o - p = ( 1  q.x-p.x   ( t
                  0  q.y-p.y )   u )

        o - p = ( 1  dx   ( t
                  0  dy )   u )

        |A| = q.y - p.y = dy
        A^{-1} = ( 1  -dx/dy 
                   0    1/dy ) 

        ( 1  -dx/dy   ( o.x - p.x   = ( t
          0    1/dy )   o.y - p.y )     u )


     */

    double det = p1.y - p0.y;

    if(abs(det) < eps)
    {
        if(abs(p0.y - ray_origin.y) > eps || max(p0.x, p1.x) < ray_origin.x - eps)
            return { false, 0. };
        
        if(p0.x > ray_origin.x - eps && p1.x > ray_origin.x - eps)
            return { true, min(p0.x, p1.x) };
        
        if(p0.x > ray_origin.x - eps)
            return { true, p0.x };

        return { true, p1.x };
    }

    Vector a{ 1, -(p1.x - p0.x) / det };
    Vector b{ 0, 1/det };
    Vector r = ray_origin - p0;

    double t = a.dot(r);
    double u = b.dot(r);

    if(t > -eps && u > -eps && u < 1 + eps)
        return { true, t };

    return { false, 0. };
}
