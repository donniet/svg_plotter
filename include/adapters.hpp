#ifndef __ADAPTERS_HPP__
#define __ADAPTERS_HPP__

#include "point.hpp"
#include "drawable.hpp"

#include <vector>
#include <execution>
#include <algorithm>
#include <iterator>

using std::vector;
using std::execution::par_unseq;
using std::distance;
using std::reduce;
using std::for_each;
using std::reduce;
using std::inclusive_scan;

struct ConstantArcLengthAdapter :
    public Drawable
{
    Drawable const & _d;
    size_t _maximum_points;
    double _length;
    vector<double> _normalized_lengths;

    void init()
    {
        // first sample maximum_points evenly
        vector<Point> sample(_maximum_points);

        for_each(par_unseq, sample.begin(), sample.end(), [&](Point & p) {
            size_t gid = &p - &sample[0];

            double t = (double)gid / (double)(_maximum_points - 1);

            p = _d.at(t);
        });

        // now measure the distance from one point to the next
        vector<double> distance(_maximum_points);

        for_each(par_unseq, distance.begin() + 1, distance.end(), [&](double & l) {
            size_t gid = &l - &distance[0];
            // HACK: assume a closed path
            size_t next = (gid + 1) % _maximum_points;

            // multiply by maximum_points to turn this into a normalized number indepdendent of the number of points
            distance[gid] = (sample[gid] - sample[next]).norm();
        });

        // add it all up to get the total length
        _length = reduce(par_unseq, distance.begin(), distance.end());

        // divide the distances by the total length to normalize it to a [0,1] interval 
        for_each(par_unseq, distance.begin(), distance.end(), [&](double & d) {
            d /= _length;
        });

        _normalized_lengths.resize(_maximum_points);

        // sum it up to make it ascending
        inclusive_scan(par_unseq, distance.begin(), distance.end(), _normalized_lengths.begin());

    }

    ConstantArcLengthAdapter(Drawable const & d, size_t maximum_points = 1e5) : 
        _d(d), _maximum_points(maximum_points)
    { 
        init();
    }

    virtual double length() const override
    {
        return _length;
    }

    virtual Point at(double t) const override
    {
        // find the lower_bound of t in the set
        auto i = lower_bound(_normalized_lengths.begin(), _normalized_lengths.end(), t);
        size_t i0 = distance(&_normalized_lengths[0], &*i);

        if(i0 >= _normalized_lengths.size())
            return Point{};

        double t0 = *i;
        double s = (double)i0 / (double)_maximum_points;

        return _d.at(s + (t - t0));
    }
};




#endif