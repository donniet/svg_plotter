#ifndef __ADAPTERS_HPP__
#define __ADAPTERS_HPP__

#include "point.hpp"
#include "drawable.hpp"

#include <vector>
#include <execution>
#include <algorithm>
#include <iterator>
#include <utility>

using std::vector;
using std::execution::par_unseq;
using std::distance;
using std::reduce;
using std::for_each;
using std::reduce;
using std::inclusive_scan;
using std::pair;

struct ConstantArcLengthAdapter :
    public Drawable
{
    Drawable const & _d;
    size_t _maximum_points;
    pair<double,double> _interval;
    vector<double> _length_index;

    void init()
    {
        // first sample maximum_points evenly
        vector<Point> sample(_maximum_points);

        for_each(par_unseq, 
                 sample.begin(), sample.end(), 
        [&](Point & p) {
            size_t gid = distance(&sample[0], &p);

            double t = (double)gid / ((double)_maximum_points - 1);
            t = _interval.first + t * (_interval.second - _interval.first);

            p = _d.at(t);
        });

        // now measure the distance from one point to the next
        _length_index.resize(_maximum_points);

        for_each(par_unseq, 
                 _length_index.begin(), _length_index.end(), 
        [&](double & l) {
            size_t gid = distance(&_length_index[0], &l);
            
            if(gid == 0) 
            {
                l = 0;
                return;
            }

            // multiply by maximum_points to turn this into a normalized number indepdendent of the number of points
            l = (sample[gid] - sample[gid-1]).norm();
        });

        // sum it up to make it ascending
        inclusive_scan(par_unseq, 
                       _length_index.begin(), _length_index.end(), 
                       _length_index.begin());

    }

    ConstantArcLengthAdapter(Drawable const & d, size_t maximum_points = 1e5, pair<double,double> interval = {0,1}) : 
        _d(d), _maximum_points(maximum_points), _interval(interval)
    { 
        init();
    }

    virtual double length(double t0, double t1) const override
    {
        if(t0 < t1)
        {
            t0 = max(_interval.first, t0);
            t1 = min(_interval.second, t1);
        }
        else
        {
            t1 = max(_interval.first, t1);
            t0 = min(_interval.second, t0);
        }
        // ds/dt is a constant
        double u = (t1 - t0) / (_interval.second - _interval.first);

        return u * _length_index.back();
    }

    virtual Event at(double t) const override
    {
        if(t <= _interval.first)
            return _d.at(_interval.first);
        
        if(t >= _interval.second)
            return _d.at(_interval.second);

        // first convert t such that it ranges from (0 to 1) exclusive.
        double s = (t - _interval.first) / (_interval.second - _interval.first);

        // now determine how much of the length should have rolled out by now
        s *= _length_index.back();

        // find the lower_bound of t in the length index
        auto i = lower_bound(_length_index.begin(), _length_index.end(), s);
        size_t gid = distance(_length_index.begin(), i);

        // gid must be > 0 since _length_index[0] === 0
        // calculate the remaining arclength after sample[gid]
        s -= _length_index[gid-1];

        // get the parameter u of the measured segment *i
        double u0 = (double)gid / ((double)_maximum_points - 1);
        u0 = _interval.first + u0 * (_interval.second - _interval.first);

        // get the event corresponding to gid
        Event e = _d.at(u0);

        // use the derivative here to estimate the proper parameter
        double ds_dt = e.dp_dt().norm();

        return _d.at(u0 + s / ds_dt);
    }
};




#endif