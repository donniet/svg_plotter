#ifndef __SHAPE_HPP__
#define __SHAPE_HPP__

#include "bounding_box.hpp"
#include "path.hpp"
#include "point.hpp"
#include "cover.hpp"

#include <vector>
#include <utility>

using std::vector;
using std::swap;





/*

//  deprecated in favor of Cover

struct Shape
{
    vector<Event> _outline;
    BoundingBox _bounding_box;

    double area() const
    {
        return ::area(_outline.begin(), _outline.end());
    }

    double perimeter() const
    {
        return ::path_length(_outline.begin(), _outline.end(), true);
    }

    Path outline() const
    {
        using std::execution::par_unseq;

        vector<Point> p(_outline.size() + 1);

        transform(par_unseq, 
                  _outline.begin(), _outline.end(), 
                  p.begin(), 
        [](Event const & e) -> Point {
            return static_cast<Point>(e);
        });
        p[_outline.size()] = p[0];

        return(Path(move(p)));
    }

    using const_iterator = vector<Event>::const_iterator;

    const_iterator begin() const { return _outline.begin(); }
    const_iterator end() const { return _outline.end(); }

    // PairwiseCircularAdapter<const_iterator> 
    // segments() const
    // {
    //     return PairwiseCircularAdapter<const_iterator>(begin(), end());
    // }



    BoundingBox calculate_bounding_box(vector<Event> const & events)
    {
        using std::execution::par_unseq;

        auto iters = minmax_element(par_unseq, 
                                    events.begin(), events.end(), 
        [&](Event const & e0, Event const & e1) -> bool {
            return e0.x < e1.x || e0.y < e1.y;
        });

        return BoundingBox(static_cast<Point>(*iters.first), static_cast<Point>(*iters.second));
    }

    Shape() : _outline(), _bounding_box() { }

    Shape(vector<Event> && outline) :
        _outline(outline), 
        _bounding_box(calculate_bounding_box(_outline))
    { }

    BoundingBox bounding_box() const { return _bounding_box; }

    bool is_inside(Point p) const 
    {
        using std::execution::par_unseq;

        
        Event last = _outline.back();
        int count = 0;
        bool intersect;
        double _;
        for(Event const & e : _outline)
        {
            tie(intersect, _) = segment_intersects_horizontal_ray(last, e, p);

            if(intersect)
                count++;

            last = e;
        }

        return (count % 2) == 1;

        // PairwiseCircularAdapter<const_iterator> edges(begin(), end());
        // vector<unsigned char> intersections(edges.size());

        // transform(par_unseq,
        //          edges.begin(), edges.end(), 
        //          intersections.begin(),
        // [&](pair<Point const &, Point const &> edge) -> unsigned char {
        //     if(segment_intersects_horizontal_ray(edge.first, edge.second, p))
        //         return 1;
            
        //     return 0;
        // });

        // unsigned char odd = reduce(par_unseq, 
        //                     intersections.begin(), intersections.end(), 0, 
        // [](unsigned char c, unsigned char d) {
        //     return (c + d) % 2;
        // });

        // return odd == 1;
    }
};

*/



#endif