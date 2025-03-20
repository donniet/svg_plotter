#ifndef __STROKE_MESH_HPP__
#define __STROKE_MESH_HPP__


#include "attribute_mesh.hpp"
#include "stroke.hpp"

#include <type_traits>
#include <iterator>

using std::enable_if_t, std::is_same_v;
using std::iterator_traits;


#include <algorithm>
#include <execution>
#include <iterator>
#include <utility>

using std::for_each, std::transform, std::inclusive_scan;
using std::execution::par_unseq;
using std::distance;
using std::pair;

enum class LineJoin {
    Bevel,
    Miter,
    Chamfer
};

class StrokeMesh
{
private:
    double _brush_diameter;
    LineJoin _join;

public:
    StrokeMesh(double brush_diameter = 1., LineJoin join = LineJoin::Chamfer) :
        _brush_diameter(brush_diameter), _join(join)
    { }

    void coordinate_space(std::vector<Point> const & path, 
                          std::vector<Vector> & tangents, 
                          std::vector<Vector> & normals,
                          bool is_closed = false) const
    {
        // Calculate tangents
        for (size_t i = 0; i < path.size(); ++i) 
        {
            size_t prev, next;

            if(is_closed)
            {
                prev = (i + path.size() - 1) % path.size();
                next = (i + 1) % path.size();
            }
            else 
            {
                if(i == 0) 
                    prev = 0;
                else
                    prev = i - 1;

                if(i == path.size() - 1)
                    next = i;
                else 
                    next = i + 1;
            }

            Vector v = path[next] - path[prev];
            double l = v.norm();

            tangents.push_back(v / l);
            normals.push_back(normal(tangents.back()));
        }
    }


    bool strip_contains(std::vector<Point> const & strip, Point const & p) const
    {
        for(size_t i = 2; i < strip.size(); ++i)
            if(Triangle{strip[i-2], strip[i-1], strip[i]}.contains(p))
                return true;

        return false;
    }

    using mesh_type =   AttributeMesh<Point, Point, size_t, double>;
    
    mesh_type create_mesh(std::vector<std::vector<Point>> const & plot, bool is_closed = false) const
    {

        mesh_type strip(DrawMode::triangle_strip, 
            "a_position", "a_uv", "a_brush", "a_section", "a_arclength");
        
        size_t section = 0;
        double last_cross_product = 0;
        double r = _brush_diameter * 0.5;
        double s = 0;
        size_t current_size = 0;

        typedef enum {
            beginning = -1,
            middle = 0,
            end = 1
        } point_type;

        auto start_fresh = [&strip, &section](bool even = true)
        {
            // copies of the last two inserted vertices
            auto pA = strip[strip.size()-2];
            auto pB = strip[strip.size()-1];

            // start a new section
            section++;
            get<3>(pA) = section;
            get<3>(pB) = section;

            strip.append(pA);
            strip.append(pB);            
        };


        auto insert_point_pair = [&](point_type typ, Point const & brush, Vector const & tangent, Vector const & normal)
        {
            Point points[] = { 
                brush - r * normal + (double)typ * 0.5 * tangent,
                brush + r * normal + (double)typ * 0.5 * tangent
            };
            Point uvs[] = { 
                Point{ -0.5, s / _brush_diameter + 0.5 * (double)typ }, 
                Point{  0.5, s / _brush_diameter + 0.5 * (double)typ } 
            };

            if(current_size >= 2)
            {
                Point const & pA = strip.vertex(strip.size() - 2);
                Point const & pB = strip.vertex(strip.size() - 1);

                bool reset = false;
                for(Point p : points)
                {
                    double c = cross(pB - pA, p - pA);

                    // cross_product sign should switch each time
                    if(last_cross_product * c < 0) 
                        continue;

                    reset = true;
                    last_cross_product = c;
                    break;
                }
                
                if(reset)
                    start_fresh(true);

            }

            strip.append(points[0], uvs[0], brush, section, s);
            strip.append(points[1], uvs[1], brush, section, s);
            current_size += 2;
        };

        for(size_t j = 0; j < plot.size(); ++j, ++section)
        {
            current_size = 0;
            last_cross_product = 0;

            std::vector<Point> const & path = plot[j];

            std::vector<Vector> tangents;
            std::vector<Vector> normals;

            coordinate_space(path, tangents, normals, is_closed);

            for(size_t i = 0; i < path.size(); ++i) 
            {
                // the first point in every path is a move
                // and doesn't increase the arclength
                if(i > 0) 
                    s += (path[i] - path[i-1]).norm();

                if(i == 0) // create a landing zone for the brush stroke
                    insert_point_pair(beginning, path[i], tangents[i], normals[i]);
                else if(i == path.size() - 1)
                    insert_point_pair(end, path[i], tangents[i], normals[i]);
                else
                    insert_point_pair(middle, path[i], tangents[i], normals[i]);

            }
        }

        return strip;
    }
};


#endif