#include "utility.hpp"

#include <algorithm>
#include <functional>
#include <utility>

using std::pair;
using std::string;
using std::find_if;
using std::ptr_fun;
using std::not1;
using std::isspace;
using std::max, std::min;

// trim from start (in place)
void ltrim(string &s) 
{
    s.erase(s.begin(), find_if(s.begin(), s.end(),
        not1(ptr_fun<int, int>(isspace))));
}

// trim from end (in place)
void rtrim(string &s) 
{
    s.erase(find_if(s.rbegin(), s.rend(),
        not1(ptr_fun<int, int>(isspace))).base(), s.end());
}

void trim(string &s)
{
    rtrim(s);
    ltrim(s);
}

pair<double,double> intersect_intervals(pair<double,double> a, pair<double,double> b)
{
    return {max(a.first, b.first), min(a.second, b.second)};
}

pair<bool,double> segment_intersects_vertical_ray(Point p0, Point p1, Point ray_origin, double eps)
{
    /*
        r(t) = o +   (0;1) * t
        s(u) = p + (q - p) * u

        with t >= 0
        u \in [0,1)

        o - p = ( 0  q.x-p.x   ( t
                  1  q.y-p.y )   u )

        o - p = ( 0  dx   ( t
                  1  dy )   u )

        |A| = -dx 
        A^{-1} = ( -dy/dx    1 
                     1/dx    0 ) 

        ( -dy/dx  1   ( o.x - p.x   = ( t
            1/dx  0 )   o.y - p.y )     u )
     */

    double det = p0.x - p1.x;

    if(abs(det) < eps)
    {
        if(abs(p0.x - ray_origin.x) > eps || max(p0.y, p1.y) <= ray_origin.y - eps)
            return { false, 0. };
        
        if(p0.y > ray_origin.y- eps && p1.y > ray_origin.y - eps)
            return { true, min(p0.x, p1.x) };
        
        if(p0.y > ray_origin.y - eps)
            return { true, p0.y };

        return { true, p1.y };
    }

    Vector a{ (p1.y - p0.y) / det, 1 };
    Vector b{ -1./det, 0 };
    Vector r = ray_origin - p0;

    double t = a.dot(r);
    double u = b.dot(r);

    if(t > -eps && u > -eps && u < 1 + eps)
        return { true, t };

    return { false, 0. };
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
