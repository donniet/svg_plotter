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


void Lines::init()
{
    using std::execution::par_unseq;

    _separation_direction = Vector(_direction.y, -_direction.x);
    _separation_direction = _separation * _separation_direction.normalized();

    if(_direction.is_parallel({1,0}))
    {
        // f_i(t) = (_origin + _separation * i * Y) + t * _direction

        double t0 = (_bbox.p0.x - _origin.x ) / _direction.x;
        double t1 = (_bbox.p1.x - _origin.x ) / _direction.x;
        
        double i0 = (_bbox.p0.y - _origin.y ) / _separation;
        double i1 = (_bbox.p1.y - _origin.y ) / _separation;

        _parameter_range.first =  { min(t0, t1), (int)ceil(min(i0, i1)) };
        _parameter_range.second = { max(t0, t1), (int)floor(max(i0, i1)) };

        auto line_count = _parameter_range.second.second - _parameter_range.first.second;
        _line_origins.resize(line_count);
        _line_lengths.resize(line_count);

        for_each(par_unseq,
                    _line_origins.begin(), _line_origins.end(),
        [&](Point & p) 
        {
            size_t gid = distance(&_line_origins[0], &p);

            // TODO: check for negative/positive cases
            p = _origin + _separation * gid * Point(0,1);
            _line_lengths[gid] = _bbox.p1.x - _bbox.p0.x;
        });
    }
    else if(_direction.is_parallel({0,1}))
    {
        double t0 = (_bbox.p0.y - _origin.y ) / _direction.y;
        double t1 = (_bbox.p0.y - _origin.y ) / _direction.y;

        double i0 = (_bbox.p0.x - _origin.x ) / _separation;
        double i1 = (_bbox.p1.x - _origin.x ) / _separation;

        _parameter_range.first =  { min(t0, t1), (int)ceil(min(i0, i1)) };
        _parameter_range.second = { max(t0, t1), (int)floor(max(i0, i1)) };

        for_each(par_unseq,
            _line_origins.begin(), _line_origins.end(),
        [&](Point & p) 
        {
            size_t gid = distance(&_line_origins[0], &p);

            // TODO: check for negative/positive cases
            p = _origin + _separation * gid * Point(1,0);
            _line_lengths[gid] = _bbox.p1.y - _bbox.p0.y;
        });
    }
    else 
    {
        // pick the corner of the bounding box to start in.  This will be the
        // smallest i value of all the corners
        double i0 = numeric_limits<double>::max();
        double t0 = numeric_limits<double>::max();

        double i1 = numeric_limits<double>::min();
        double t1 = numeric_limits<double>::min();

        for(Point p : _bbox.corners())
        {
            double iy = ( p.y - _origin.y ) / _separation_direction.y;
            double ix = ( p.x - _origin.x ) / _separation_direction.x;

            i0 = min({i0, ix, iy});
            i1 = max({i1, ix, iy});

            double tx0 = ( p.x - _origin.x - _separation_direction.x * i0 ) / _direction.x;
            double tx1 = ( p.x - _origin.x - _separation_direction.x * i1 ) / _direction.x;
            double ty0 = ( p.y - _origin.y - _separation_direction.y * i0 ) / _direction.y;
            double ty1 = ( p.y - _origin.y - _separation_direction.y * i1 ) / _direction.y;

            t0 = min({t0, tx0, ty0, tx1, ty1 });
            t1 = max({t1, tx0, ty0, tx1, ty1 });
        }

        _parameter_range.first =  { t0, (int)ceil(i0)  };
        _parameter_range.second = { t1, (int)floor(i1) };

        bool intersects;
        double intersection_parameter;
        // now calculate the origins of each line
        for(int sep = _parameter_range.first.second; sep <= _parameter_range.second.second; sep++)
        {
            Point o = _origin + _separation_direction * sep;

            // where does this line intersect the bounding box?
            tie(intersects, intersection_parameter) = 
                _bbox.intersect_ray(o, _direction);

            // HACK: hopefully they all intersect, but just to be sure lets throw an error if it doesn't
            if(!intersects)
                throw logic_error("All lines within the parameter range should intersec the bounding box.  Check the logic above");

            o = o + intersection_parameter * _direction;
            
            tie(intersects, intersection_parameter) = 
                _bbox.intersect_ray(o, _direction);

            if(!intersects)
                continue; // just grazed the corner perhaps?

            _line_origins.push_back(o);
            _line_lengths.push_back((intersection_parameter * _direction).norm());
        }
    }

    _length_index.resize(_line_lengths.size());
    inclusive_scan(par_unseq, _line_lengths.begin(), _line_lengths.end(), _length_index.begin());
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

    double s = t * length(0, 1);

    auto i = lower_bound(_length_index.begin(), _length_index.end(), s);
    if(i == _length_index.end())
        return Event{};

    size_t gid = distance(_length_index.begin(), i);
    double sp = *i - _line_lengths[gid];

    double ds = s - sp;

    return Event{_line_origins[gid] + ds * _direction, _direction };
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