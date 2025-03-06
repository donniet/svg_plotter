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

vector<Point> simplify_plot(vector<Point> const & plot, bool is_closed, double eps)
{
    using std::execution::par_unseq;

    if(plot.size() <= 2)
        return plot;

    vector<int> keep(plot.size());

    transform(//par_unseq,
              plot.begin(), plot.end(),
              keep.begin(),
    [&](Point const & p) -> int
    {
        size_t gid = distance(&plot[0], &p);
        size_t prev, next;

        if(gid == 0 || gid == plot.size() - 1)
            return 1;

        prev = gid - 1;
        next = gid + 1;

        if((plot[prev] - p).norm() < eps)
            return 0;

        if((plot[next] - p).norm() < eps)
            return 1;
        
        double d = Segment{plot[prev], plot[next]}.distance(p);

        if(abs(d) < eps)
            return 0;

        return 1;
    });

    vector<int> scanned(plot.size());

    inclusive_scan(par_unseq,
                   keep.begin(), keep.end(),
                   scanned.begin());

    vector<Point> simplified(scanned.back());

    // pack the rest into simplified
    for_each(//par_unseq,
             plot.begin(), plot.end(),
    [&](Point const & p)
    {
        size_t gid = distance(&plot[0], &p);
        if(keep[gid] == 0)
            return;
        
        simplified[scanned[gid]-1] = p;
    });

    return move(simplified);
}

Plotter::Plotter() :
    draw_speed(1), sample_count(100), 
    epsilon(1e-2), parameter_interval(0,1)
{ }

std::vector<Point> Plotter::plot(Drawable const & drawing) 
{
    Point p0 = drawing.at(parameter_interval.first);
    Point p1 = drawing.at(parameter_interval.second);

    bool is_closed = (p1 - p0).norm() < epsilon;

    return simplify_plot(
               sample_interval(drawing, sample_count, parameter_interval),
               is_closed, epsilon);
}

std::vector<Point> Plotter::fill(Cover const & cover,                    // a 2D area to be filled
                            Drawable const & pattern)               // a drawable which we will sample over the interval [0,1] to fill
{
    using std::execution::par_unseq;

    std::vector<Point> sample = plot(pattern);
    std::vector<int> is_inside(sample_count);
    std::vector<int> scanned(sample_count);
    
    for_each(par_unseq,
             sample.begin(), sample.end(),
    [&](Point & p) 
    {
        size_t gid = distance(&sample[0], &p);

        is_inside[gid] = cover.is_inside(p) ? 1 : 0;
    });

    inclusive_scan(par_unseq, 
                is_inside.begin(), is_inside.end(), 
                scanned.begin());
                    
    int last = 0;
    bool b = true;
    size_t count = 0;
    size_t total = scanned.back();

    std::vector<Point> inside(total);

    // pack the sample
    for_each(/* par_unseq, */
            sample.begin(), sample.end(),
    [&](Point & e) 
    {
        size_t gid = distance(&sample[0], &e);

        // is this event inside our cover?
        if(!is_inside[gid])
            return; // nope

        // use our scanned vector to calculate the index 
        // of this inside event in the final vector
        Point * pe = &inside[scanned[gid]-1];
        *pe = e;
        
        // // are we the first event
        // if(gid == 0 || !is_inside[gid-1])
        //     make_move_to(*pe); // turn this into a move_to

    });
    
    return move(inside); // turn this into a stroke and return it
}