#ifndef __SAMPLERS_HPP__
#define __SAMPLERS_HPP__

#include "point.hpp"
#include "drawable.hpp"

#include <vector>
#include <tuple>
#include <cmath>
#include <execution>

using std::vector;
using std::pair;
using std::sqrt;

template<typename Iter>
vector<Point> sample_range(Drawable const & d, Iter begin, Iter end)
{
    using std::execution::par_unseq;

    size_t n = distance(begin, end);
    vector<Point> samples(n);

    transform(par_unseq, begin, end, samples.begin(), 
    [&d](double const & t) -> Point 
    {
        return d.at(t);
    });

    return samples;
}

vector<Point> sample_interval(Drawable const & d, size_t sample_count = 10000, pair<double,double> interval = {0., 1.})
{
    vector<Point> samples(sample_count);

    for_each(/*execution::par_unseq, */
             samples.begin(), samples.end(), 
    [&](Point & p) 
    {
        size_t n = &p - &samples[0];

        double t = (double)n / (double)(sample_count - 1);
        t = interval.first + t * (interval.second - interval.first);

        p = d.at(t);
    });

    return samples;
}

template<typename Iter>
vector<Point> sample_interval_constant_arclength(Drawable const & d, 
                                                 size_t sample_count = 10000, 
                                                 pair<double,double> interval = {0,1})
{
    vector<Point> samples;

    return samples;
}


pair<double,double> distrib(Drawable const & d, 
                            size_t sample_count = 1000, 
                            pair<double,double> interval = {0., 1.})
{
    using std::execution::par_unseq;

    vector<Point> samples = sample_interval(d, sample_count + 1, interval);
    vector<double> lengths(samples.size() - 1);

    double s1 = 0., s2 = 0.;
    double n = lengths.size();

    // calculate the lengths between each of the points
    transform(par_unseq,
              samples.begin() + 1, samples.end(),
              lengths.begin(),
    [&samples](Point const & p1) -> double {
        size_t gid = &p1 - &samples[0];
        Point const & p0 = samples[gid-1];

        return (p1 - p0).norm2();
    });

    // sum the squared lengths
    s2 = reduce(par_unseq,
    lengths.begin(), lengths.end());

    // take the square root
    for_each(par_unseq,
             lengths.begin(), lengths.end(),
    [](double & d2) {
        d2 = sqrt(d2);
    });

    // sum the lengths
    s1 = reduce(par_unseq, 
    lengths.begin(), lengths.end());

    double mu = s1/n;

    // return the mean and standard deviation
    return { mu, sqrt(s2/n - mu * mu) };
}

#endif