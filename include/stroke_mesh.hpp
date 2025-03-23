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

void coordinate_space(std::vector<Point> const & path, 
                      std::vector<Vector> & tangents, 
                      std::vector<Vector> & normals,
                      bool is_closed = false);

enum class BrushStyle {
    // PencilNo2,
    // Graphite2B,
    // SharpiePoint,
    BrushRound,
};

class MeshPlot
{
public:
    //          Vertex    UV     Brush, Section,  Arclength
    typedef AttributeMesh<Point, Point,  size_t,     double> mesh_type;

    static inline constexpr const char * attribute_names[] = {
        "a_position", "a_uv", "a_brush", "a_section", "a_arclength"
    };

    static inline const char * brush_color_uniform_name = "u_brush_color";
    static inline const char * brush_size_uniform_name = "u_brush_size";
    // static const char * brush_color_uniform_name = "u_arclength";

private:
    struct StrokePlot
    {
        BrushStyle brush_style;
        double brush_size;
        RGBA brush_color;
        std::pair<double, double> time_range;
        DrawMode draw_mode;
        double arclength;
        std::pair<size_t, size_t> vertex_range;
        std::pair<size_t, size_t> section_range;
    };

    mesh_type _mesh;
    std::vector<StrokePlot> _strokes;
public:
    /**
     * stroke method appends a triangle mesh onto _mesh by creating a path of triangles along the plot parameter
     * @param {Plot} plot
     * @param {double} brush_size
     * @param {RGBA} brush_color
     */
    void stroke(std::vector<std::vector<Point>> plot, 
                BrushStyle brush_style = BrushStyle::BrushRound,
                double brush_size = 1., 
                RGBA brush_color = RGBA{0,0,0,1}, 
                std::pair<double,double> time_range = {0.,1.});

    void to_c(std::ostream & os);
    void to_json(std::ostream & os);
    void to_stl(std::ostream & os);
    void to_obj(std::ostream & os);
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

    


    bool strip_contains(std::vector<Point> const & strip, Point const & p) const
    {
        for(size_t i = 2; i < strip.size(); ++i)
            if(Triangle{strip[i-2], strip[i-1], strip[i]}.contains(p))
                return true;

        return false;
    }

    using mesh_type =   AttributeMesh<Point, Point, size_t, double, size_t, size_t>;
    
    mesh_type create_mesh(std::vector<std::vector<Point>> const & plot, bool is_closed = false) const
    {

        mesh_type strip(DrawMode::triangle_strip, 
            "a_position", "a_uv", "a_brush", "a_section", "a_arclength");
        
        size_t section = 0;
        double last_cross_product = 0;
        double r = _brush_diameter * 0.5;
        double s = 0;

        typedef enum {
            beginning = -1,
            middle = 0,
            end = 1
        } point_type;

        auto offset_from_point_type = [&](point_type t) -> double
        {
            switch(t)
            {
            case beginning:
                return -1;
            case end:
                return 1;
            case middle:
            default:
                return 0;
            }
        };

        auto start_fresh = [&strip, &section, &s, &last_cross_product](size_t j, size_t i)
        {
            // copies of the last two inserted vertices
            auto pA = strip[strip.size()-2];
            auto pB = strip[strip.size()-1];

            // start a new section
            section++;

            get<3>(pA) = section;
            get<3>(pB) = section;

            get<5>(pA) = j;
            get<5>(pB) = j;

            get<6>(pA) = i;
            get<6>(pB) = i;

            strip.append(pA);
            strip.append(pB);  
        };

        auto cross_last_with = [&](Point const & p)
        {
            Point const & pA = strip.vertex(strip.size() - 2);
            Point const & pB = strip.vertex(strip.size() - 1);

            return cross(pB - pA, p - pA);
        };

        auto insert_point_pair = [&](point_type typ, Point const & brush, Vector const & tangent, Vector const & normal, size_t j, size_t i)
        {
            Point points[] = { 
                brush - r * normal + offset_from_point_type(typ) * 0.5 * _brush_diameter * tangent,
                brush + r * normal + offset_from_point_type(typ) * 0.5 * _brush_diameter * tangent,
            };
            Point uvs[] = { 
                Point{ -0.5, s / _brush_diameter + 0.5 * offset_from_point_type(typ) }, 
                Point{  0.5, s / _brush_diameter + 0.5 * offset_from_point_type(typ) },
            };

            if(typ == beginning || typ == end)
            {
                strip.append(points[0], uvs[0], brush, section, s, j, i);
                strip.append(points[1], uvs[1], brush, section, s, j, i);
                last_cross_product = 0;
                return;
            }

            // ASSERT(typ == middle)
            
            float c0 = cross_last_with(points[0]);

            if(last_cross_product * c0 > 0)
            {
                start_fresh(j, i);
                last_cross_product = 0.;
            }
            else
                last_cross_product = cross_last_with(points[0]);

            strip.append(points[0], uvs[0], brush, section, s, j, i);

            float c1 = cross_last_with(points[1]);

            if(last_cross_product * c1 > 0)
            {
                start_fresh(j, i);
                last_cross_product = 0.;
            }
            else
                last_cross_product = cross_last_with(points[1]);

            strip.append(points[1], uvs[1], brush, section, s, j, i);
        };

        for(size_t j = 0; j < plot.size(); ++j, ++section)
        {
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
                {
                    insert_point_pair(beginning, path[i], tangents[i], normals[i], j, i);
                    insert_point_pair(middle, path[i], tangents[i], normals[i], j, i);
                }
                else if(i == path.size() - 1)
                {
                    insert_point_pair(middle, path[i], tangents[i], normals[i], j, i);
                    insert_point_pair(end, path[i], tangents[i], normals[i], j, i);
                }
                else
                {
                    insert_point_pair(middle, path[i], tangents[i], normals[i], j, i);
                }

            }
        }

        return strip;
    }
};


#endif