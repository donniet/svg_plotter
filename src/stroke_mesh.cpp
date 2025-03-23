#include "stroke_mesh.hpp"


typedef enum {
    beginning = -1,
    middle = 0,
    end = 1
} point_type;

double offset_from_point_type(point_type t)
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
}

void MeshPlot::stroke(vector<vector<Point>> plot, 
                      BrushStyle brush_style,
                      double brush_size, 
                      RGBA brush_color, 
                      pair<double,double> time_range)
{
    StrokePlot & m = _strokes.emplace_back(StrokePlot{
        .brush_style = brush_style,
        .brush_size = brush_size,
        .brush_color = brush_color,
        .time_range = time_range,
        .draw_mode = DrawMode::triangle_strip,
        .arclength = 0,
        .vertex_range = {0,0},
        .section_range = {0,0}
    });



    size_t section = 0;
    double last_cross_product = 0;
    double r = m.brush_size * 0.5;
    double s = 0;


    auto start_fresh = [&](size_t j, size_t i)
    {
        // copies of the last two inserted vertices
        auto pA = _mesh[_mesh.size()-2];
        auto pB = _mesh[_mesh.size()-1];

        // start a new section
        section++;

        get<3>(pA) = section;
        get<3>(pB) = section;

        // get<5>(pA) = j;
        // get<5>(pB) = j;

        // get<6>(pA) = i;
        // get<6>(pB) = i;

        _mesh.append(pA);
        _mesh.append(pB);  
    };

    auto cross_last_with = [&](Point const & p)
    {
        Point const & pA = _mesh.vertex(_mesh.size() - 2);
        Point const & pB = _mesh.vertex(_mesh.size() - 1);

        return cross(pB - pA, p - pA);
    };

    auto insert_point_pair = [&](point_type typ, 
                                 Point const & brush, 
                                 Vector const & tangent, 
                                 Vector const & normal, 
                                 size_t j, size_t i)
    {
        Point points[] = { 
            brush - r * normal + offset_from_point_type(typ) * 0.5 * m.brush_size * tangent,
            brush + r * normal + offset_from_point_type(typ) * 0.5 * m.brush_size * tangent,
        };
        Point uvs[] = { 
            Point{ -0.5, s / m.brush_size + 0.5 * offset_from_point_type(typ) }, 
            Point{  0.5, s / m.brush_size + 0.5 * offset_from_point_type(typ) },
        };

        if(typ == beginning || typ == end)
        {
            _mesh.append(points[0], uvs[0], brush, section, s);
            _mesh.append(points[1], uvs[1], brush, section, s);
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

        _mesh.append(points[0], uvs[0], brush, section, s);

        float c1 = cross_last_with(points[1]);

        if(last_cross_product * c1 > 0)
        {
            start_fresh(j, i);
            last_cross_product = 0.;
        }
        else
            last_cross_product = cross_last_with(points[1]);

        _mesh.append(points[1], uvs[1], brush, section, s);
    };

    for(size_t j = 0; j < plot.size(); ++j, ++section)
    {
        last_cross_product = 0;

        std::vector<Point> const & path = plot[j];

        std::vector<Vector> tangents;
        std::vector<Vector> normals;

        coordinate_space(path, tangents, normals);

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
}



void coordinate_space(std::vector<Point> const & path, 
                     std::vector<Vector> & tangents, 
                     std::vector<Vector> & normals,
                     bool is_closed)
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