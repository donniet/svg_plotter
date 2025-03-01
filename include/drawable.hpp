#ifndef __DRAWABLE_HPP__
#define __DRAWABLE_HPP__

#include "point.hpp"

#include <algorithm>
#include <execution>
#include <vector>
#include <utility>
#include <functional>
#include <limits>


using std::for_each, std::inclusive_scan, std::transform, std::upper_bound, std::lower_bound;
using std::vector;
using std::swap, std::tie;
using std::distance;
using std::function;
using std::numeric_limits;

class Drawable 
{
public:
    virtual double length(double t0, double t1) const 
    { 
        return 0.; 
    }

    virtual Event at(double t) const 
    { 
        return Event{}; 
    }

    virtual pair<bool, double> last_move_between(double t0, double t1) const
    {
        if(abs(t1 - t0) <= numeric_limits<double>::epsilon())
            return { false, t1 };

        if(length(t0, t1) >= (at(t1) - at(t0)).norm())
            return { false, t1 };

        // this logic isn't 100% accurate, but it can do if you are being lazy
        // if the length from t0 to t1 is less than the linear distance from 
        // at(t0) to at(t1) then there must have been a move.
        
        for(;;)
        {
            double tp = (t0 + t1) / 2.;

            if(abs(tp - t0) <= numeric_limits<double>::epsilon())
                return { true, tp };
            
            if(length(tp, t1) < (at(t1) - at(tp)).norm())
            {
                t0 = tp;
                continue;
            }

            t1 = tp;
            continue;
        }

        throw std::logic_error("there must be a problem above if it got here!  it should only return from the loop");
    }
};

/* Drawable constructed by sampling a given function.  normalized to constant ds/dt */
class DrawableFromFunction :
    public Drawable
{
protected:
    function<Point(double)> _f; // we shouldn't need to save it...
    size_t _sample_count;
    pair<double, double> _interval;
    vector<Event> _samples;
    vector<double> _lengths;           // _lengths[i] == (_samples[i] - _samples[i-1] ).norm()
    vector<double> _length_index;      // _length_index[i] == length of path from [0, i-1]

public:
    DrawableFromFunction(function<Point(double)> f, 
                         pair<double, double> interval = {0, 1}, 
                         size_t sample_count = 1000) :
        _f(f), _sample_count(sample_count), _interval(interval), _samples(sample_count), 
        _lengths(sample_count), _length_index(sample_count)
    { 
        init();
    }

    DrawableFromFunction(pair<double, double> interval = {0, 1}, 
                         size_t sample_count = 1000) :
        _f(), _sample_count(sample_count), _interval(interval), _samples(0), 
        _lengths(0), _length_index(0)

    {
        // we won't init until we get a function
    }

    void reset(function<Point(double)> f)
    {
        _f = f;
        _samples.resize(_sample_count);
        _lengths.resize(_sample_count);
        _length_index.resize(_sample_count);
        init();
    }

    virtual Event at(double t) const override
    {
        if(_samples.size() < 2)
            throw std::logic_error("uninitialized drawable function");

        size_t gid;
        double u;
        tie(gid, u) = segment_by_parameter(t);

        Point p0 = _samples[gid];
        Point p1 = p0;
        if(gid < _samples.size() - 1)
            p1 = _samples[gid + 1];
        
        Point p = p0 + u * (p1 - p0);
        double dt = (_interval.second - _interval.first) / 
                    ((double)_sample_count - 1);

        Vector v = (p1 - p0) / dt;
        if(gid == _samples.size() - 1)
            v = p0 - _samples[gid - 1];

        if(t == 0)
            return Event{p, v, 0.};

        return Event{p, v, 1.};
    }

    virtual double length(double t0, double t1) const override
    {
        if(_samples.size() < 2)
            throw std::logic_error("uninitialized drawable function");
            
        if(t0 == t1)
            return 0;
        
        return length_to(t1) - length_to(t0);
    }   
protected:
    // assumes _segments.size() > 0
    pair<size_t, double> segment_by_parameter(double t) const 
    {
        t = max(_interval.first, min(t, _interval.second));
        t /= (_interval.second - _interval.first);

        // if t is less than 0 return the first element with no remainder
        if(t <= 0)
            return { 0, 0 };
        
        // if t is greater or equal to 1 return the last segment with parameter 1 (endpoint)
        if(t >= 1)
            return { _length_index.size() - 1, 1 };

        double len = _length_index.back();
        double s = len * t;

        size_t low_index = upper_bound(&_length_index[0], &_length_index[0] + _length_index.size(), s) -
                           &_length_index[0];

        // if this isn't the first one, subtract the length of the previous segments
        // if it is the first one these previous lengths are simply 0
        if(low_index > 0)
            s -= _length_index[low_index-1];

        // return a pointer to the segment and a remaining parameter normalized by the length of this segment
        return { low_index, s / _lengths[low_index] };
    }

    double length_to(double t) const 
    {
        size_t gid;
        double u;

        tie(gid, u) = segment_by_parameter(t);
        Point p0 = _samples[gid];
        Point p1 = p0;
        if(gid < _samples.size() - 1)
            p1 = _samples[gid + 1];

        Point p = p0 + u * (p1 - p0);
        
        return _length_index[gid] + (p - p0).norm();
    }

private:
    void init()
    {
        if(_sample_count < 2)
            throw std::logic_error("sample_count must be greater than 2");

        using std::execution::par_unseq;

        // sample our at function
        for_each(par_unseq,
                 _samples.begin(), _samples.end(),
        [&](Event & e) 
        {
            size_t gid = distance(&_samples[0], &e);
            // the minus one ensures we capture the endpoint of the interval
            double u = (double)gid / ((double)_sample_count - 1.);
            u = _interval.first + u * (_interval.second - _interval.first);

            e = _f(u);
        });

        // build our length index
        transform(par_unseq,
                  _samples.begin(), _samples.end(),
                  _lengths.begin(),
        [&](Event & e) -> double
        {
            size_t gid = distance(&_samples[0], &e);
            if(gid == 0)
                return 0;

            return (_samples[gid] - _samples[gid-1]).norm();
        });

        // scan to get the index
        inclusive_scan(par_unseq,
                       _lengths.begin(), _lengths.end(),
                       _length_index.begin());
    }
};


#endif