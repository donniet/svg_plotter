#include "plotter.hpp"
#include "samplers.hpp"

#include <execution>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>

using std::vector;
using std::for_each;
using std::distance;
using std::transform, std::for_each, std::inclusive_scan;

vector<Point> simplify_path(vector<Point> const & path, double eps)
{
    if(path.size() <= 2)
        return path;

    vector<Point> ret;

    ret.push_back(path.front());
    Point p0 = ret.back();

    for(size_t i = 1; i < path.size() - 1; i++)
    {
        Point p1 = path[i+1];
        Point p = path[i];

        double d = Segment{p0, p1}.distance(p);

        if(abs(d) < eps)
            continue;

        ret.push_back(p);
        p0 = ret.back();
    }

    ret.push_back(path.back());

    return ret;
}

vector<Point> simplify_path_par(vector<Point> const & path, double eps)
{
    using std::execution::par_unseq;

    if(path.size() <= 2)
        return path;

    vector<int> keep(path.size());

    transform(//par_unseq,
              path.begin(), path.end(),
              keep.begin(),
    [&](Point const & p) -> int
    {
        size_t gid = distance(&path[0], &p);
        size_t prev, next;

        if(gid == 0 || gid == path.size() - 1)
            return 1;

        prev = gid - 1;
        next = gid + 1;

        if((path[prev] - p).norm() < eps)
            return 0;

        if((path[next] - p).norm() < eps)
            return 1;
        
        double d = Segment{path[prev], path[next]}.distance(p);

        if(abs(d) < eps)
            return 0;

        return 1;
    });

    vector<int> scanned(path.size());

    inclusive_scan(par_unseq,
                   keep.begin(), keep.end(),
                   scanned.begin());

    vector<Point> simplified(scanned.back());

    // pack the rest into simplified
    for_each(//par_unseq,
             path.begin(), path.end(),
    [&](Point const & p)
    {
        size_t gid = distance(&path[0], &p);
        if(keep[gid] == 0)
            return;
        
        simplified[scanned[gid]-1] = p;
    });

    return move(simplified);
}

vector<vector<Point>> simplify_plot(vector<vector<Point>> const & plot, bool is_closed, double eps)
{
    using std::execution::par_unseq;

    vector<vector<Point>> ret(plot.size());

    for(size_t i = 0; i < plot.size(); i++)
        ret[i] = simplify_path(plot[i], eps);

    return ret;
}

Plotter::Plotter() :
    draw_speed(1), sample_count(100), 
    epsilon(1e-2), parameter_interval(0,1)
{ }

std::vector<std::vector<Point>> Plotter::plot(Drawable const & drawing) 
{
    Point p0 = drawing.at(parameter_interval.first);
    Point p1 = drawing.at(parameter_interval.second);

    bool is_closed = (p1 - p0).norm() < epsilon;

    return sample_interval(drawing, sample_count, parameter_interval);

    // return simplify_plot(
    //            sample_interval(drawing, sample_count, parameter_interval),
    //            is_closed, epsilon);
}

std::vector<std::vector<Point>> Plotter::fill(Cover const & cover,                    // a 2D area to be filled
                                              Drawable const & pattern)               // a drawable which we will sample over the interval [0,1] to fill
{
    using std::execution::par_unseq;

    std::vector<std::vector<Point>> sample = plot(pattern);

    std::vector<std::vector<Point>> ret;
    if(sample.size() == 0)
        return ret;

    std::vector<int> is_inside(sample_count);
    std::vector<int> scanned(sample_count);

    for(size_t i = 0; i < sample.size(); i++) 
    {
        vector<Point> const & path = sample[i];

        vector<Point> q;
        q.push_back(path[0]);

        for(size_t j = 1; j < path.size(); j++)
        {
            Point const & p = path[j];

            if(cover.is_inside(p))
            {
                q.push_back(p);
            }
            else 
            {
                if(q.size() >= 2)
                    ret.emplace_back(move(q));
                
                q.clear();
            }
        }

        if(q.size() >= 2)
            ret.emplace_back(move(q));
    }

    return move(ret); // turn this into a stroke and return it
}