#ifndef __STROKE_MESH_HPP__
#define __STROKE_MESH_HPP__

#include "mesh.hpp"
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
    StrokeMesh(double brush_diameter = 1., LineJoin join = LineJoin::Bevel) :
        _brush_diameter(brush_diameter), _join(join)
    { }


    auto create(vector<Point> const & path, bool is_closed = false)
    {
        std::vector<Point> vertices;
        std::vector<Vector> tangents;
        std::vector<Vector> normals;

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
    

        double s = 0;
        double r = _brush_diameter * 0.5;

        // Generate offset points and triangle strip vertices
        for (size_t i = 0; i < path.size(); ++i) 
        {
            Point p1 = path[i] + r * normals[i];
            Point p2 = path[i] - r * normals[i];

            if (i > 0 && i < path.size() - 1) { // Handle line joins
                Vector prevNormal = normals[i - 1];
                Vector nextNormal = normals[i];
                Vector prevTangent = tangents[i-1];
                Vector nextTangent = tangents[i];

                double cosAngle = dot(prevTangent, nextTangent);

                if (cosAngle < 0.9999999)   // Avoid issues with near-straight lines
                { 
                    Point prevP1 = path[i] + r * prevNormal; 
                    Point prevP2 = path[i] - r * prevNormal; 

                    Point nextP1 = path[i] + r * nextNormal; 
                    Point nextP2 = path[i] - r * nextNormal;

                    auto intersectionP1 = Line{prevP1, tangents[i-1]}.intersect(Line{nextP1, tangents[i]});
                    auto intersectionP2 = Line{prevP2, tangents[i-1]}.intersect(Line{nextP2, tangents[i]});

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

                        vertices.push_back(prevP1); 
                        vertices.push_back(prevP2); 
                        vertices.push_back(nextP1); 
                        vertices.push_back(nextP2); 
                        continue;
                    }
                }
            }

            
            double v = s / _brush_diameter;

            vertices.push_back(p1); 
            vertices.push_back(p2); 
        }

        return move(vertices);
    }
};


#endif