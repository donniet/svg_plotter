#ifndef __SAMPLERS_HPP__
#define __SAMPLERS_HPP__

#include "point.hpp"
#include "drawable.hpp"

#include <vector>
#include <tuple>

using std::vector;
using std::pair;

template<typename Iter>
vector<Point> sample_range(Drawable const & d, Iter begin, Iter end)
{
    size_t n = distance(begin, end);
    vector<Point> samples(n);

    transform(execution::par_unseq, begin, end, samples.begin(), 
    [&d](double const & t) -> Point 
    {
        return d.at(t);
    });

    return samples;
}

template<typename Iter>
vector<Point> sample_interval(Drawable const & d, size_t sample_count = 10000)
{
    vector<Point> samples(sample_count);

    for_each(execution::par_unseq, 
             samples.begin(), samples.end(), 
    [&](Point & p) 
    {
        size_t n = &p - &samples[0];

        double t = (double)n / (double)(sample_count - 1);

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

#endif