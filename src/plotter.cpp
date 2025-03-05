#include "plotter.hpp"

#include <execution>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>

using std::vector;
using std::for_each;
using std::distance;

Plotter::Plotter() :
    draw_speed(1), sample_count(100), 
    epsilon(1e-5), parameter_interval(0,1)
{ }

Stroke Plotter::plot(Drawable const & drawing) 
{
    using std::execution::par_unseq;

    vector<Event> points(sample_count);

    double len = drawing.length(0, 1);

    // first stab: just plot the points
    for_each(par_unseq, 
                points.begin(), points.end(), 
    [&](Event & e) 
    {
        size_t gid = &e - &points[0];

        // t \in [0,1]
        double t = (double)gid / (double)(sample_count - 1);
        t = parameter_interval.first + t * (parameter_interval.second - parameter_interval.first);

        Point p = drawing.at(t);
        Vector ds;

        // |dp/dt| should equal draw_speed
        if(gid > 0) 
        {
            Point p0 = drawing.at(t - epsilon);
            ds = Vector(p.x - p0.x, p.y - p0.y);
        }
        else
        {
            Point p1 = drawing.at(t + epsilon);
            ds = Vector(p1.x - p.x, p1.y - p.y);
        }
        ds = ds.normalized();

        e = Event(p, ds * draw_speed);
    });

    return Stroke(move(points));
}

Stroke Plotter::fill(Cover const & cover,                    // a 2D area to be filled
            Drawable const & pattern)               // a drawable which we will sample over the interval [0,1] to fill
{
    using std::execution::par_unseq;

    vector<Event> events(sample_count);
    vector<int> is_inside(sample_count);
    vector<int> scanned(sample_count);
    
    for_each(/* par_unseq, */
                events.begin(), events.end(),
    [&](Event & p) 
    {
        size_t gid = &p - &events[0];
        double t = (double)gid / (double)(sample_count - 1);

        p = pattern.at(t);
        if(gid > 0 && pattern.last_move_between((double)(gid - 1) / (double)(sample_count - 1), t).first)
        {
            p.dt = 0;
        }

        is_inside[gid] = cover.is_inside(p) ? 1 : 0;
    });

    inclusive_scan(par_unseq, 
                is_inside.begin(), is_inside.end(), 
                scanned.begin());
                    
    int last = 0;
    bool b = true;
    size_t count = 0;
    size_t total = scanned.back();

    vector<Event> inside(total);

    // pack the events
    for_each(/* par_unseq, */
            events.begin(), events.end(),
    [&](Event & e) 
    {
        size_t gid = distance(&events[0], &e);

        // is this event inside our cover?
        if(!is_inside[gid])
            return; // nope

        // use our scanned vector to calculate the index 
        // of this inside event in the final vector
        Event * pe = &inside[scanned[gid]-1];
        *pe = e;
        
        // are we the first event
        if(gid == 0 || !is_inside[gid-1])
            make_move_to(*pe); // turn this into a move_to

    });
    
    return Stroke(move(inside)); // turn this into a stroke and return it
}