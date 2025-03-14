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

vector<vector<Point>> sample_interval(Drawable const & d, size_t sample_count = 10000, pair<double,double> interval = {0., 1.})
{
    vector<Point> samples(sample_count);
    vector<bool> is_move(sample_count);
    

    for_each(/*execution::par_unseq, */
             samples.begin(), samples.end(),
    [&](Point & p) {
        size_t gid = std::distance(&samples[0], &p);

        double t0 = (double)gid / (double)(sample_count - 1);
        t0 = interval.first + t0 * (interval.second - interval.first);

        double t1 = (double)(gid + 1) / (double)(sample_count - 1);
        t1 = interval.first + t1 * (interval.second - interval.first);

        auto m = d.last_move_between(t0, t1);

        samples[gid] = d.at(t0);
        is_move[gid] = (m.first && m.second < t1);
    });

    is_move[0] = false;
    vector<vector<Point>> ret(1);

    for(size_t i = 0; i < sample_count; i++)
    {
        ret.back().push_back(samples[i]);

        if(is_move[i])
            ret.emplace_back(0);
    }

    return move(ret);
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

    vector<vector<Point>> samples = sample_interval(d, sample_count + 1, interval);
    vector<vector<double>> lengths(samples.size());

    double s1 = 0., s2 = 0.;
    size_t n = 0;

    for_each(samples.begin(), samples.end(),
    [&](vector<Point> & path) {
        size_t gid = std::distance(&samples[0], &path);
        
        for_each(path.begin() + 1, path.end(),
        [&](Point & p1) {
            size_t gid2 = std::distance(&path[0], &p1);
            Point p0 = path[gid2 - 1];
        
            double d2 = (p1 - p0).norm2();
            s2 += d2;
            s1 += std::sqrt(d2);
            ++n;
        });
    });

    double mu = s1/(double)n;

    // return the mean and standard deviation
    return { mu, sqrt(s2/(double)n - mu * mu) };
}

#endif