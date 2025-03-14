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

    using mesh_type =   AttributeMesh<Point, Point, double>;
    
    mesh_type create_mesh(std::vector<std::vector<Point>> const & plot, bool is_closed = false) const
    {

        mesh_type strip(DrawMode::triangle_strip, 
            "a_position", "a_uv", "a_brush", "a_section");
        
        size_t section = 0;
        double last_cross_product = 0;
        double r = _brush_diameter * 0.5;
        double s = 0;
        size_t current_size = 0;

        auto start_fresh = [&strip, &section](Point & nuv, bool even = true)
        {
            // copies of the last two inserted vertices
            auto pA = strip[strip.size()-2];
            auto pB = strip[strip.size()-1];

            // start a new section
            section++;
            get<3>(pA) = section;
            get<3>(pB) = section;
            
            if(even) 
            {
                strip.append(pA);
                strip.append(pB);
            }
            else
            {
                strip.append(pA);
                strip.append(pB);
            }
            
        };


        auto insert_point = [&last_cross_product, &strip, &start_fresh, &section, &current_size]
                            (Point const & p, Point uv, Point const & brush)
        {
            if(current_size >= 2)
            {
                Point const & pA = strip.vertex(strip.size() - 2);
                Point const & pB = strip.vertex(strip.size() - 1);

                double c = cross(pB - pA, p - pA);

                // cross_product sign should switch each time
                if(last_cross_product * c > 0 || strip.contains(p)) 
                    start_fresh(uv, c > 0);
                
                last_cross_product = c;

            }

            strip.append(p, uv, brush, section);
            current_size++;
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
                Point p1 = path[i] + r * normals[i],
                    p2 = path[i] - r * normals[i];

                if(i == 0) // create a landing zone for the brush stroke
                {
                    double v = s / _brush_diameter;

                    Point pA = p1 - r * tangents[i],
                        pB = p2 - r * tangents[i];

                    insert_point(pA, { 0.5, v - 0.5}, path[i]);
                    insert_point(pB, {-0.5, v - 0.5}, path[i]);
                }

                if(i > 0) 
                    s += (path[i] - path[i-1]).norm();


                if (i > 0 && i < path.size() - 1) { // Handle line joins
                    Vector prevNormal  = normals[i - 1],
                        nextNormal  = normals[i],
                        prevTangent = tangents[i-1],
                        nextTangent = tangents[i];

                    double cosAngle = dot(prevTangent, nextTangent);

                    if (cosAngle < 0.9999999)   // Avoid issues with near-straight lines
                    { 
                        Point prevP1 = path[i] + r * prevNormal,
                            prevP2 = path[i] - r * prevNormal; 

                        Point nextP1 = path[i] + r * nextNormal,
                            nextP2 = path[i] - r * nextNormal;

                        auto intersectionP1 = Segment{prevP1, tangents[i-1]}
                                                .intersect(Segment{nextP1, tangents[i]});
                        auto intersectionP2 = Segment{prevP2, tangents[i-1]}
                                                .intersect(Segment{nextP2, tangents[i]});

                        if (_join == LineJoin::Miter) 
                        {
                            if (intersectionP1.first && intersectionP2.first) 
                            {
                                p1 = prevP1 + intersectionP1.second * tangents[i-1];
                                p2 = prevP2 + intersectionP2.second * tangents[i-1];
                            }
                        } 
                        else if (_join == LineJoin::Chamfer) 
                        {
                            p1 = nextP1;
                            p2 = nextP2;
                        } 
                        else if (_join == LineJoin::Bevel) 
                        {
                            double v = s / _brush_diameter;

                            insert_point(prevP1, { 0.5, v}, path[i]);
                            insert_point(prevP2, {-0.5, v}, path[i]);
                            insert_point(nextP1, { 0.5, v}, path[i]);
                            insert_point(nextP2, {-0.5, v}, path[i]);
                            continue;
                        }
                    }
                }

                
                double v = s / _brush_diameter;

                insert_point(p1, { 0.5, v}, path[i]);
                insert_point(p2, {-0.5, v}, path[i]);

                if(i == path.size() - 1)
                {
                    Point pA = p1 + r * tangents[i],
                        pB = p2 + r * tangents[i];

                    insert_point(pA, { 0.5, v + 0.5}, path[i]);
                    insert_point(pB, {-0.5, v + 0.5}, path[i]);
                }
            }
        }

        return strip;
    }
};


#endif