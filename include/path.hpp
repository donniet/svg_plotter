#ifndef __PATH_HPP__
#define __PATH_HPP__

#include "drawable.hpp"
#include "point.hpp"

#include <vector>
#include <iterator>
#include <numeric>

using std::vector;
using std::distance;
using std::max;
using std::min;

class Path :
    public Drawable
{
protected:
    vector<Point> _points;          // ordered container for all the points
    vector<double> _length_index;   // increasing list of lengths of each segment
                                    // such that _length_index[i] == | _points[i] - _points[i-1] |

    void init()
    {
        if(_points.size() < 2)
            return;

        using std::execution::par_unseq;

        _length_index.resize(_points.size());

        transform(par_unseq, 
                  _points.begin(), _points.end(),
                  _length_index.begin(),
        [&](Point & p) -> double {
            size_t gid = distance(&_points[0], &p);

            if(gid == 0) 
                return 0;

            return (p - _points[gid-1]).norm();
        });

        inclusive_scan(par_unseq, 
                       _length_index.begin(), _length_index.end(), 
                       _length_index.begin());
    }

public:
    virtual double length(double t0, double t1) const override
    {
        if(_length_index.size() == 0)
            return 0;

        return _length_index.back() * (min(1., t1) - max(0., t0));
    }

    virtual Event at(double t) const override
    {
        if(_points.size() == 0)
            return Point{};

        if(_points.size() == 1)
            return _points[0];

        // if we are outside of the unit inverval return the first or last point
        // treat the first point as a move (dt == 0)
        if(t <= 0)
            return Event{ _points.front(), _points[1] - _points[0], 0 };

        if(t >= 1)
            return Event{ _points.back(), _points.back() - _points[_points.size() - 2], 1. };

        // calculate the arclength for this parameter
        double s = t * _length_index.back();

        // find the segment which crosses this arclength value
        auto i = lower_bound(_length_index.begin(), _length_index.end(), s);

        // what is the id of the second point of the segment that crosses the length s?
        size_t gid = distance(_length_index.begin(), i);

        // get the (dx/dt, dy/dt) values for this segment
        // gid > 0 because s > 0 and _length_index[0] === 0
        Vector dp = _points[gid] - _points[gid-1];

        // if we are not on the first segment, subtract the previous arclength
        // then turn our arclength into a [0,1] parameter for the segment
        // by dividing by the length of that segment
        s = ( s                  - _length_index[gid-1] ) / 
            ( _length_index[gid] - _length_index[gid-1] );

        // now start at the starting point of this segment, and move s toward the second point
        return Event(_points[gid-1] + s * dp, dp);
    }

    Path() : _points(0) { };

    template<typename Iter>
    Path(Iter begin, Iter end) :
        _points(begin, end)
    { 
        init();
    }

    Path(vector<Point> const & p) :
        _points(p)
    { 
        init();
    }

    Path(vector<Point> && p) :
        _points(move(p))
    { 
        init();
    }
};


#endif