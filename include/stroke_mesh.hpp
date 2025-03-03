#ifndef __STROKE_MESH_HPP__
#define __STROKE_MESH_HPP__

#include "mesh.hpp"
#include "stroke.hpp"

#include <type_traits>
#include <iterator>

using std::enable_if_t, std::is_same_v;
using std::iterator_traits;

class StrokeMesh
{
private:
    double _brush_diameter;

public:
    StrokeMesh(double brush_diameter = 1.) :
        _brush_diameter(brush_diameter)
    { }

    template<typename Range>
    auto operator()(Range const & r)
    {
        return (*this)(r.begin(), r.end());
    }

    // returns the next triangle fan in the stroke ranging from begin to end
    template<typename Iter>
    pair<TriangleStrip, Iter /* next or end */> 
    operator()(Iter const & begin, 
               Iter const & end)
    {
        if(distance(begin, end) < 1)
            return { TriangleStrip(), end };

        double r = _brush_diameter / 2.;
        // edges that connect an even vertex to an odd one move in the radial direction
        // odd to even cut back across the quad
        vector<Point> vertices;
        vector<Point> uv;

        double s = 0;

        Event last;
        Vector dp;

        auto build_header = [&vertices, &uv, r](Point const & c, Vector ds)
        {
            ds = ds.normalized();
            Vector rad(-ds.y, ds.x);

            // build the quad that leads this strip and provides space for the brush strokes
            vertices.insert(vertices.end(), {
                c - r * ds - r * rad,
                c - r * ds + r * rad,
                c          - r * rad,
                c          + r * rad
            });

            uv.insert(uv.end(), {
                Point(-r, -r), 
                Point(-r,  r),
                Point( 0, -r),
                Point( 0,  r)
            });
        };

        auto build_footer = [&vertices, &uv, &r, &s](Point const & c, Vector ds)
        {
            ds = ds.normalized();
            Vector rad(-ds.y, ds.x);

            // build the quad that leads this strip and provides space for the brush strokes
            vertices.insert(vertices.end(), {
                c + r * ds - r * rad,
                c + r * ds + r * rad
            });

            uv.insert(uv.end(), {
                Point(s + r, -r), 
                Point(s + r,  r)
            });
        };

        auto i = begin;
        // move through the events in the stroke and create a triangle fan with UV coordinates
        for(; i != end; ++i)
        {
            // is this the first event?
            if(i == begin)
            {
                build_header(*i, i->dp);
                last = *i;
                dp = i->dp.normalized();
                continue;
            }

            // if this is a move_to event we should close out our triangle strip
            if(i->is_move_to())
                break;

            Vector qp = *i - last;
            double qs = qp.norm();

            s += qs;
            qp /= qs;

            Vector rad(-qp.y, qp.x);

            if(i->dp.norm() != 0)
            {
                auto np = i->dp.normalized();
                rad = Vector(-np.y, np.x);
            }

            // TODO: calculate if either of these overlap the previous strip!
            vertices.insert(vertices.end(), {
                (Point)*i - r * rad,
                (Point)*i + r * rad
            });
            uv.insert(uv.end(), {
                Point(s, -r),
                Point(s,  r)
            });

            last = *i;
            dp = i->dp;
        }

        // build the quad that ends the strip and provides space for the brush stroke
        build_footer((Point)last, dp);

        // and return 
        return { TriangleStrip(move(vertices), move(uv)), i };

    }
};


#endif