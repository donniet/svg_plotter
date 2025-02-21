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
    bool closed;

    void init()
    {
        // first sample maximum_points evenly
        vector<Point> sample(_maximum_points + 1);

        for_each(par_unseq, sample.begin(), sample.end(), [&](Point & p) {
            size_t gid = &p - &sample[0];

            double t = (double)gid / (double)_maximum_points;
            t = _interval.first + t * (_interval.second - _interval.first);

            p = _d.at(t);
        });

        // now measure the distance from one point to the next
        _length_index.resize(_maximum_points);

        for_each(par_unseq, _length_index.begin(), _length_index.end(), [&](double & l) {
            size_t gid = &l - &_length_index[0];
            size_t next = gid + 1;

            // multiply by maximum_points to turn this into a normalized number indepdendent of the number of points
            _length_index[gid] = (sample[gid] - sample[next]).norm();
        });

        if(closed)
            _length_index.push_back((sample.front() - sample.back()).norm());

        // sum it up to make it ascending
        inclusive_scan(par_unseq, _length_index.begin(), _length_index.end(), _length_index.begin());

    }

    ConstantArcLengthAdapter(Drawable const & d, size_t maximum_points = 1e5, pair<double,double> interval = {0,1}) : 
        _d(d), _maximum_points(maximum_points), _interval(interval), closed(false)
    { 
        init();
    }

    virtual double length() const override
    {
        return _length_index.back();
    }

    virtual Point at(double t) const override
    {
        // first convert t such that it ranges from 0 to 1.
        double s = (t - _interval.first) / (_interval.second - _interval.first);

        // now determine how much of the length should have rolled out by now
        s *= length();

        // find the lower_bound of t in the length index
        auto i = upper_bound(_length_index.begin(), _length_index.end(), s);

        // get the index to the lower_bound and calculate the interpolation factor ds
        size_t i0 = _maximum_points;
        double ds = s;

        if(i != _length_index.end())
        {
            i0 = distance(&_length_index[0], &*i);
            ds -= *i;
        }
        else
            ds -= length();

        // the + 1 here is due to the length_index[i] measuring the length from i to i+1
        double u = (double)(i0 + 1) / (double)_maximum_points;
        u = _interval.first + u * (_interval.second - _interval.first);

        return _d.at(u + ds);
    }
};




#endif