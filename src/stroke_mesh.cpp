#include "stroke_mesh.hpp"


#include <format>

typedef enum {
    beginning = -1,
    middle = 0,
    end = 1
} point_type;

namespace std
{
    string to_string(::BrushStyle const & s)
    {
        switch(s)
        {
        case ::BrushStyle::BrushRound: 
        default:
            return "BrushRound";
        }
    }
}

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

MeshPlot::MeshPlot() : _mesh(DrawMode::triangle_strip, 
    MeshPlot::attribute_names[0], 
    MeshPlot::attribute_names[1], 
    MeshPlot::attribute_names[2], 
    MeshPlot::attribute_names[3], 
    MeshPlot::attribute_names[4]) 
{ }

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


std::string ts(size_t level)
{
    return std::string(level * 3, ' ');
}

void MeshPlot::to_c(std::ostream & os) const
{
    auto stroke_to_c = [&](StrokePlot const & p) -> std::string { 
        return std::format(R"({{
            .brush_style = {},
            .brush_size = {},
            .brush_color = {},
            .time_range = {{ {}, {} }},
            .draw_mode = {},
            .arclength = {},
            .vertex_range = {{ {}, {} }},
            .section_range = {{ {}, {} }},
        }})", 
            std::to_string(p.brush_style), p.brush_size, std::to_string(p.brush_color), p.time_range.first, p.time_range.second, 
            std::to_string(p.draw_mode), p.arclength, p.vertex_range.first, 
            p.vertex_range.second, p.section_range.first, p.section_range.second);

    };
    auto strokes_to_c = [&]() -> std::string {
        std::string out = "[\n";
        for(StrokePlot const & p : _strokes)
        {
            out += stroke_to_c(p) + ",\n";
        }
        out += "\n]";
        return out;
    };
    auto attributes_to_c = [&]() -> std::string { 
        std::string out = "[\n";
        for(auto const & a : _mesh.attributes())
        {
            out += std::format(R"({{
                .name = {},
                .size = {},
                .offset = {},
            }},
            )", a.name(), a.size(), a.offset());
        }
        out += "\n]";
        return out;
    };
    auto uniforms_to_c = [&]() -> std::string { 
        // TODO: add more uniform details to the attribute_mesh
        std::string out = "[\n";
        for(auto const & u : _mesh.uniforms())
        {
            out += std::format(R"({{
                .name = {},
            }})", u.name());
        }
        out += "\n]";
        return out;
    };
    auto buffer_to_c = [&]() -> std::string { 
        std::string out = "[";
        auto i = _mesh.buffer_begin();
        for(size_t c = 0; i != _mesh.buffer_end(); ++i, ++c)
        {
            if(c % _mesh.stride() == 0)
                out += "\n";
            
            out += std::format("{},", *i);
        }
        out += "\n]";
        return out;
    };

    os << std::format(R"({{
        .strokes = {},
        .attributes = {},
        .uniforms = {},
        .buffer = {},
    }})", strokes_to_c(), attributes_to_c(), uniforms_to_c(), buffer_to_c());
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