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
    StrokeMesh(double brush_diameter = 1., LineJoin join = LineJoin::Miter) :
        _brush_diameter(brush_diameter), _join(join)
    { }

    void coordinate_space(vector<Point> const & path, 
                          vector<Vector> & tangents, 
                          vector<Vector> & normals,
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

    std::vector<Strip>
    create_no_overlap(std::vector<Point> const & path, bool is_closed = false) const
    {
        std::vector<Vector> tangents;
        std::vector<Vector> normals;

        coordinate_space(path, tangents, normals, is_closed);

        std::vector<Point> vertices;
        std::vector<size_t> ids;

        double s = 0;
        double r = _brush_diameter * 0.5;

        for(size_t i = 0; i < path.size(); i++)
        {
            Point p1 = path[i] + r * normals[i];
            Point p2 = path[i] - r * normals[i];

            if(i == 0) // make a half-square to hold the back-side of the brush
            {
                // TODO: check which tangent we should use for closed paths
                Point pA = p1 - tangents[i]; 
                Point pB = p2 - tangents[i];

                vertices.insert(vertices.end(), {
                    pA, pB, p1, p2
                });
                ids.insert(ids.end(), {
                    i, i, i, i
                });
                continue;
            }

            if(i == path.size() - 1)
            {
                Point pA = p1 + tangents[i];
                Point pB = p2 + tangents[i];

                vertices.insert(vertices.end(), {
                    p1, p2, pA, pB
                });
                ids.insert(ids.end(), {
                    i, i, i, i
                });
                continue;
            }

            Vector prevNormal = normals[i - 1];
            Vector nextNormal = normals[i];
            Vector prevTangent = tangents[i-1];
            Vector nextTangent = tangents[i];

            if(dot(prevTangent, nextTangent) < 0.9999999)
            {
                Point prevP1 = path[i] + r * prevNormal; 
                Point prevP2 = path[i] - r * prevNormal; 

                Point nextP1 = path[i] + r * nextNormal; 
                Point nextP2 = path[i] - r * nextNormal;

                auto intersectionP1 = Segment{prevP1, tangents[i-1]}.intersect(Segment{nextP1, tangents[i]});
                auto intersectionP2 = Segment{prevP2, tangents[i-1]}.intersect(Segment{nextP2, tangents[i]});

                if(_join == LineJoin::Bevel)
                {
                    vertices.insert(vertices.end(), {
                        prevP1, prevP2, nextP1, nextP2
                    });
                    ids.insert(ids.end(), {
                        i, i, i, i
                    });
                    
                    continue;
                }

                if(_join == LineJoin::Miter && intersectionP1.first && intersectionP2.first)
                {
                    p1 = prevP1 + intersectionP1.second * tangents[i-1];
                    p2 = prevP2 + intersectionP2.second * tangents[i-1];
                }

                if(_join == LineJoin::Chamfer)
                {
                    p1 = nextP1;
                    p2 = nextP2;
                }
            }

            vertices.insert(vertices.end(), {
                p1, p2
            });
            ids.insert(ids.end(), {
                i, i
            });
        }


    }

    bool strip_contains(vector<Point> const & strip, Point const & p) const
    {
        for(size_t i = 2; i < strip.size(); ++i)
            if(Triangle{strip[i-2], strip[i-1], strip[i]}.contains(p))
                return true;

        return false;
    }

    std::vector<Strip> 
    create(vector<Point> const & path, bool is_closed = false) const
    {
        std::vector<Vector> tangents;
        std::vector<Vector> normals;

        coordinate_space(path, tangents, normals, is_closed);
        
        std::vector<Strip> strips;
        std::vector<Point> vertices;

        auto start_fresh = [&vertices, &strips](bool even = true)
        {
            Point pA = vertices[vertices.size() - 2];
            Point pB = vertices[vertices.size() - 1];

            strips.emplace_back(move(vertices));
            vertices.clear();

            // if(!even) std::swap(pA, pB);

            vertices.push_back(pA);
            vertices.push_back(pB);
            
        };

        auto insert_point = [&](Point const & p)
        {
            if(strip_contains(vertices, p))
                start_fresh(true);

            vertices.push_back(p); 
        };

        double s = 0;
        double r = _brush_diameter * 0.5;

        // Generate offset points and triangle strip vertices
        for (size_t i = 0; i < path.size(); ++i) 
        {
            Point p1 = path[i] + r * normals[i];
            Point p2 = path[i] - r * normals[i];

            if(i == 0) // create a landing zone for the brush stroke
            {
                Point pA = p1 - r * tangents[i];
                Point pB = p2 - r * tangents[i];

                insert_point(pA);
                insert_point(pB);
            }


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

                    auto intersectionP1 = Segment{prevP1, tangents[i-1]}.intersect(Segment{nextP1, tangents[i]});
                    auto intersectionP2 = Segment{prevP2, tangents[i-1]}.intersect(Segment{nextP2, tangents[i]});

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

                        // does this overlap?
                        if(strip_contains(vertices, prevP1) || strip_contains(vertices, prevP2) ||
                           strip_contains(vertices, nextP1) || strip_contains(vertices, nextP2)) 
                        {
                            strips.emplace_back(move(vertices));
                            vertices.clear();
                        }

                        insert_point(prevP1);
                        insert_point(prevP2);
                        insert_point(nextP1);
                        insert_point(nextP2);
                        continue;
                    }
                }
            }

            
            double v = s / _brush_diameter;

            insert_point(p1);
            insert_point(p2);

            if(i == path.size() - 1)
            {
                Point pA = p1 + r * tangents[i];
                Point pB = p2 + r * tangents[i];

                insert_point(pA);
                insert_point(pB);
            }
        }


        if(vertices.size() > 0)
            strips.emplace_back(move(vertices));

        return strips;
    }
};


#endif