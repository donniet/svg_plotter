#include "fills.hpp"

#include <algorithm>
#include <execution>
#include <numeric>
#include <cmath>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <utility>


using std::lower_bound;
using std::min, std::max;
using std::ceil, std::floor;
using std::distance;
using std::numeric_limits;
using std::logic_error;
using std::pair, std::tie;


int Lines::find_minimum_index(BoundingBox const & bbox, 
                              Point const & origin, Vector const & separation)
{
    // f_i(t) = origin + i * separation + t * direction
    //       where direction is orthogonal to separation
    // we want to find the min i such that for some t', f_i(t') = bbox.corner[j]

    // f_i(t) = origin + (separation; direction) * (i, t) 
    // f_i(t) - origin = A * (i, t) 
    // (i, t) = A^{-1} (f_i(t) - origin)
    /* 
        sep.x =  dir.y;
        sep.y = -dir.x;

        A       = ( sep.x  dir.x 
                    sep.y  dir.y )

        |A|     = sep.x * dir.y - dir.x * sep.y
                = sep.x * sep.x + sep.y * sep.y
                = |sep|

        A^{-1}  = ( dir.y/|A|  -dir.x/|A|
                    -sep.y/|A|  sep.x/|A| )
                
                = ( sep.x/|A|   sep.y/|A| 
                   -sep.y/|A|   sep.x/|A| )

        i = sep / |A| . (bb - origin)
          = - sep / |sep| . (bb - origin)
          = - sep.proj(bb - origin)
    */

    Vector n = separation / separation.norm2();
    auto c = bbox.corners();

    double i0 = n.dot(c[0] - origin);
    double i1 = n.dot(c[1] - origin);
    double i2 = n.dot(c[2] - origin);
    double i3 = n.dot(c[3] - origin);

    double i = min({i0, i1, i2, i3});

    int in = ceil(i);
    if(in == (int)i)
        ++in;
    
    return in;
}


std::pair<double,double> Lines::intersect(Line const & l, BoundingBox const & b)
{
    auto segs = b.sides();

    double t0 = numeric_limits<double>::max(), 
           t1 = numeric_limits<double>::min();

    for(int i = 0; i < 4; i++)
    {
        auto a = l.intersect(segs[i]);
        if(!a.first)
            continue;

        t0 = min(t0, a.second);
        t1 = max(t1, a.second);
    }
    
    return {t0, t1};
}

void Lines::add_segment(Segment && seg) 
{
    double s = 0.;
    if(_length_index.size() > 0)
        s = _length_index.back();

    double l = seg.length();
    
    _segments.push_back(seg);
    _line_lengths.push_back(l);
    _length_index.push_back(s + l);
}

void Lines::init()
{

    _separation_direction = Vector(_direction.y, -_direction.x);
    _separation_direction = _separation * _separation_direction.normalized();

    int i = find_minimum_index(_bbox, _origin, _separation_direction);

    double t0, t1;

    for(;; i++)
    {
        auto line = Line{ _origin + (double)i * _separation_direction, _direction };

        tie(t0, t1) = intersect(line, _bbox);
        
        if(t0 >= t1)
            break;

        add_segment(Segment{line(t0), line(t1)});
    }


}

double Lines::length(double t0, double t1) const
{
    if(_length_index.size() == 0)
        return 0;

    // this is length constant
    return _length_index.back() * (t1 - t0);
}

Event Lines::at(double t) const
{
    if(_length_index.size() == 0)
        return Event{_origin, _direction, 0.};

    double s = t * length(0, 1);  // the total length we shold go along
                                  // all the lines

    auto i = lower_bound(_length_index.begin(), _length_index.end(), s);
    if(i == _length_index.end())
        return Event{};

    size_t gid = distance(_length_index.begin(), i);

    double rem;                   // the remaining length
    if(i == _length_index.begin())
        rem = s;
    else
        rem = s - *(i-1);
        
    auto seg = _segments[gid];
    auto v = (seg.p1 - seg.p0).normalized();

    return Event{seg.p0 + v * rem, _direction };
}

pair<bool, double> Lines::last_move_between(double t0, double t1) const
{
    if(_length_index.size() == 0)
        return { false, t1 };

    double s0 = t0 * length(0, 1);
    double s1 = t1 * length(0, 1);

    auto i0 = upper_bound(_length_index.begin(), _length_index.end(), s0);
    auto i1 = upper_bound(_length_index.begin(), _length_index.end(), s1);

    if(i0 == i1)
        return { false, t1 };

    --i1;
    return { true, *i1 / length(0, 1) };
}

Lines::Lines(BoundingBox bbox, 
        double separation,
        Vector direction,
        Point origin) :
    _origin(origin), _direction(direction.normalized()),
    _bbox(bbox), _separation(separation)
{ 
    init();
}



double Spiral::find_max_parameter()
{
    // estimate the length of the spiral from concentric circles
    // which corner of the bounding box is farthest from the center?
    
    auto corners = _region.corners();
    double max = 0.;

    for_each(corners.begin(), corners.end(), [&](Point const & c) {
        double d = (_center - c).norm2();
        if(d > max)
            max = d;
    });

    return sqrt(max) / _dradius;
}

Event Spiral::at(double t) const
{
    double theta = t * _max_parameter;
    double r = _dradius * theta;

    Point p = Point(r * cos(theta), r * sin(theta)) + _center;

    // dx/dt = δx/δθ * dθ/dt + δx/δr * dr/dt
    // δx/δθ = -r sin(θ)
    // dθ/dt = _max_parameter
    // δx/δr = cos(θ)
    // dr/dt = _dradius
    // dx/dt = -r sin(θ) _max_parameter + cos(θ) _dradius

    Vector dp{ - sin(theta) * r * _max_parameter,
                 cos(theta) * _dradius               };

    return Event(p, dp, _max_parameter);
}

Spiral::Spiral(BoundingBox region, Point center, double dradius) :
    _center(center), _region(region), _dradius(dradius), 
    _max_parameter(find_max_parameter())
{ }