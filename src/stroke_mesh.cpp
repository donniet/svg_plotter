#include "stroke_mesh.hpp"

#include <vector>
#include <string>
#include <format>
#include <iostream>
#include <ranges>
#include <functional>
#include <limits>
#include <list>
#include <deque>

namespace ranges = std::ranges;

using std::vector;
using std::string;
using std::format;
using std::to_string;
using std::ostream;
using std::max, std::min;
using std::bind2nd, std::transform, std::tie;
using std::numeric_limits;
using std::list;
using std::ranges::sort;
using std::deque;

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

MeshPlot::MeshPlot(pair<double,double> drawing_size, RGBA clear_color) : 
    _mesh(DrawMode::triangle_strip, 
        MeshPlot::attribute_names[0], 
        MeshPlot::attribute_names[1], 
        MeshPlot::attribute_names[2], 
        MeshPlot::attribute_names[3], 
        MeshPlot::attribute_names[4]),
    _drawing_size(drawing_size),
    _clear_color(clear_color)
{ }



/**
 *   TRANGLE_STRIP:
 *   
 *   a--c--e--g ...
 *   | /| /| /| ...
 *   |/ |/ |/ | ...
 *   b--d--f--g ...
 * 
 ***/


/**
 * riffle returns an array where the even elements come from even and odd from odd
 */
template<typename Iter1, typename Iter2, typename Output>
void riffle(Iter1 ebegin, Iter1 eend, Iter2 obegin, Output out)
{
    for(int i = 0; ebegin != eend; i = (i + 1) % 2)
    {
        if(i == 0)
            *out++ = *ebegin++; // even
        else       
            *out++ = *obegin++; // odd
    }   
}

// updates the intersection range of line l given an intersecting line a
// double update_intersection_range(Line const & l, Line const & a, pair<double,double> & range)
// {
//     auto i = l.intersect(a);

//     if(!i.first)
//         return;

//     range.first = max(range.first, i.second);
//     range.second = min(range.second, i.second); 
// }

/**
 * intersection simplifies the dual valued functions in point.hpp
 */
Point intersection(Line const & l, Line const & m)
{
    auto i = l.intersect(m);

    if(!i.first)
        return 0.5 * (l(1) + m(0));

    return l(i.second);
}

vector<Line> offset_path(vector<Point> const & path, double w)
{
    vector<Vector> norm(path.size()), tang(path.size());
    vector<Line> ret(path.size());

    auto next = [&path](size_t i) -> size_t
    {
        i++;
        if(i >= path.size())
            return 0;

        return i;
    };

    for(size_t i = 0; i < path.size(); i++)
    {
        tang[i] = path[next(i)] - path[i];
        norm[i] = normal(tang[i]);

        double l = norm[i].norm2();
        
        if(l == 0)
        {
            ret[i] = Segment{path[i], path[next(i)]}.line();
            continue;
        }
        
        Vector n = norm[i].normalized();
        ret[i] = Line{path[i] + w * n, tang[i]};
    }

    return ret;
}


double intersection_parameter(Line const & a, Line const & b)
{
    if(&a == &b || a == b)
        return 0;
    
    auto i = a.intersect(b);
    if(!i.first)
        return 0;

    return i.second;
}

void remove_reversing_segments(
    /* in  */ vector<Line> const & offset, 
    /* out */ deque<pair<Segment, size_t>> & segs,  // should be empty
              bool closed = false)
{
    struct T 
    {
        Line const * line;
        size_t index;
        double b, e;
    };

    // setup our list of lines and indices
    list<T> lines;
    for(size_t i = 0; i < offset.size(); i++)
        lines.insert(lines.end(), T{&offset[i], i});

    // helper functions
    auto prev = [closed, &lines](list<T>::iterator i) -> list<T>::iterator
    {
        // return the same segment if we are the first
        if(i == lines.begin())
            return i;
        
        return --i;
    };

    auto next = [closed, &lines](list<T>::iterator i) -> list<T>::iterator
    {
        auto j = i;
        j++;
        if(j == lines.end())
        {
            if(closed)
                return lines.begin();
            
            // return the same segment if we are last
            return i;
        }

        return j;
    };

    auto is_last = [&lines](list<T>::iterator i) -> bool
    {
        return ++i == lines.end();
    };
    
    // loop until we aren't removing any more lines
    bool finished = false;
    while(!finished)
    {
        // assume we are done
        finished = true;

        // TODO: figure out what to do about the first and last ones
        for(auto s = lines.begin(); !is_last(s);)
        {
            // if this is an empty line erase it
            if((*s->line).v.norm2() == 0)
            {
                lines.erase(s);
                finished = false;
                continue;
            }

            auto q = prev(s);
            auto t = next(s);

            s->b = intersection_parameter(*s->line, *q->line);
            s->e = intersection_parameter(*s->line, *t->line);

            // use > instead of >= and handle these rare, equal edge cases
            // by the default calculations
            if(s->b > s->e)
            {
                // this line is reversed so we erase it
                auto u = s++;
                lines.erase(u);
                finished = false;
            }
            else 
                s++;
        }
    }
    
    // return our segments
    for(auto s = lines.begin(); s != lines.end(); s++)
        segs.emplace_back(Segment{ (*s->line)(s->b), (*s->line)(s->e) }, s->index);
}

void MeshPlot::stroke_path(vector<Point> const & path, double brush_size, vector<Triangle> & vertices, vector<Triangle> & uv, vector<tuple<size_t, size_t, size_t>> & path_indices, bool closed)
{
    // TODO: probably a faster way of creating triangles than this mess, but I think this will work
    double w = 0.5 * brush_size;

    if(path.size() < 2)
        return;

    auto outside = offset_path(path, w);
    deque<pair<Segment, size_t>> outside_segments;
    remove_reversing_segments(outside, outside_segments, closed);

    auto inside = offset_path(path, -w);
    deque<pair<Segment, size_t>> inside_segments;
    remove_reversing_segments(inside, inside_segments, closed);
    
    // create triangles
    size_t outside_index = outside_segments.front().second;
    size_t inside_index = inside_segments.front().second;
    size_t first_index = min(outside_index, inside_index);
    Point o = outside_segments.front().first.p0;
    Point i = inside_segments.front().first.p0;

    Point path_last;
    
    if(first_index > 0) // degenerate case
        path_last = path[first_index-1];
    else if(closed)
        path_last = path[max(outside_segments.back().second, inside_segments.back().second)];
    else
        path_last = path[first_index];


    auto next_outside_or = [&](size_t i) -> size_t
    {
        auto j = outside_segments.begin();
        if(j != outside_segments.end())
            j++;

        if(j == outside_segments.end())
            return i;

        return j->second;
    };

    auto next_inside_or = [&](size_t i) -> size_t
    {
        auto j = inside_segments.begin();
        if(j != inside_segments.end())
            j++;

        if(j == inside_segments.end())
            return i;

        return j->second;
    };

    double brush_distance = 0.;

    // loop while we still have segments
    while(!outside_segments.empty() || !inside_segments.empty())
    {
        // as long as we are following in parallel, create quads
        while(!outside_segments.empty() && !inside_segments.empty()
            && outside_index == inside_index)
        {
            Point o1 = outside_segments.front().first.p1;
            Point i1 = inside_segments.front().first.p1;

            outside_index = outside_segments.front().second;
            inside_index = inside_segments.front().second;

            auto next_outside_index = next_outside_or(outside_index);
            auto next_inside_index = next_inside_or(inside_index);

            double len_outside = (path[next_outside_index] - path_last).norm() / brush_size;
            double len_inside = (path[next_inside_index] - path_last).norm() / brush_size;

            vertices.emplace_back(o, i, o1);
            vertices.emplace_back(i, o1, i1);

            uv.emplace_back(Point{ 0.5, brush_distance}, Point{-0.5, brush_distance              }, Point{ 0.5, brush_distance + len_outside});
            uv.emplace_back(Point{-0.5, brush_distance}, Point{ 0.5, brush_distance + len_outside}, Point{-0.5, brush_distance + len_inside});

            path_indices.emplace_back(outside_index, inside_index,                   next_outside_or(outside_index));
            path_indices.emplace_back(inside_index,  next_outside_or(outside_index), next_inside_or(inside_index ));

            o = o1;
            i = i1;
            path_last = path[outside_index];
            brush_distance += min(len_outside, len_inside);

            outside_segments.pop_front();
            inside_segments.pop_front();
        }

        // I don't think we need this anymore as we set outside_index and inside_index inside the loop above
        // if(!outside_segments.empty())
        //     outside_index = outside_segments.front().second;

        // if(!inside_segments.empty())
        //     inside_index = inside_segments.front().second;

        // crawl along the outside until the outside index is greater or equal to the inside
        while(!outside_segments.empty() && 
              outside_index < inside_index)
        {
            // our next outside point
            Point o1 = outside_segments.front().first.p1;

            // the brush length of this segment
            double len = (path[outside_segments.front().second] - path_last).norm() / brush_size;

            // add a triangle using the last inside point and the outside segment
            vertices.emplace_back(o, i, o1);
            uv.emplace_back(Point{ 0.5, brush_distance}, Point{-0.5, brush_distance      }, Point{ 0.5, brush_distance + len});
            path_indices.emplace_back(outside_index, inside_index,                   next_outside_or(outside_index));

            o = o1;
            path_last = path[outside_index];
            brush_distance += len;
            outside_index = outside_segments.front().second;

            outside_segments.pop_front();
        }

        // then crawl along the inside until that index is greater or equal to the outside
        while(!inside_segments.empty() &&
              inside_index < outside_index)
        {
            Point i1 = inside_segments.front().first.p1;

            double len = (path[inside_segments.front().second] - path_last).norm() / brush_size;

            vertices.emplace_back(i, o, i1);
            uv.emplace_back(Point{-0.5, brush_distance}, Point{ 0.5, brush_distance + len}, Point{-0.5, brush_distance + len});
            path_indices.emplace_back(inside_index,  next_outside_or(outside_index), next_inside_or(inside_index ));

            i = i1;
            path_last = path[inside_index];
            brush_distance += len;
            inside_index = inside_segments.front().second;

            inside_segments.pop_front();
        }


    }
}

vector<Triangle> stroke_path4(vector<Point> const & path, double brush_size, bool closed)
{
    double w = 0.5 * brush_size;
    vector<Vector> tangents, normals;
    coordinate_space(path, tangents, normals);

    vector<Triangle> ret;

    if(path.size() < 2)
        return ret;

    vector<Segment> outside, inside;

    for(size_t j = 1; j < path.size(); j++)
    {
        Vector t = (path[j] - path[j-1]).normalized();
        Vector n{-t.y, t.x};

        outside.push_back(Segment{path[j-1] + w * n, path[j] + w * n});
        inside.push_back(Segment{path[j-1] - w * n, path[j] - w * n});
    }

    if(closed)
    {
        Vector t = (path.front() - path.back()).normalized();
        Vector n{-t.y, t.x};

        outside.push_back(Segment{path.back() + w * n, path.front() + w * n});
        inside.push_back(Segment{path.back() - w * n, path.front() - w * n});
    }

    // find the intersection points between the outside and inside lines
    for(size_t j = 1; j < outside.size(); j++)
    {
        auto a = outside[j-1].line().intersect(outside[j].line());
        auto b = inside[j-1].line().intersect(inside[j].line());

        Point o1 = outside[j-1].line()(a.second);
        Point i1 = inside[j-1].line()(b.second);

        outside[j-1].p1 = outside[j].p0 = o1;
        inside[j-1].p1 = inside[j].p0 = i1;
    }

    // make triangles
    for(size_t j = 1; j < outside.size(); j++)
    {
        ret.emplace_back(outside[j-1].p0, inside[j-1].p0, outside[j].p0);
        ret.emplace_back(inside[j-1].p0, outside[j].p0, inside[j].p0);
    }

    // // then add the last two 
    // ret.emplace_back(o, i, outside[0](1.));
    // ret.emplace_back(i, outside[0](1.), inside[0](1.));


    return ret;
}

/**
 * stroke_path method turns a path into a triangular strip mesh of width brush_size
 * enclosing the path
 */
vector<Triangle> stroke_path3(vector<Point> const & path, double brush_size)
{
    double w = 0.5 * brush_size;
    vector<Vector> tangents, normals;
    coordinate_space(path, tangents, normals);

    vector<Segment> segments;

    vector<Line> outers;
    vector<pair<double,double>> op;
    size_t o0 = 0;
    vector<Line> inners;
    vector<pair<double,double>> ip;
    size_t i0 = 0;

    pair<double,double> initial_range{numeric_limits<double>::min(), numeric_limits<double>::max()};

    vector<Triangle> ret;

    // displace the segments
    for(size_t i = 1; i < path.size(); i++)
    {
        segments.emplace_back(path[i-1], path[i]);
        Vector t = (path[i] - path[i-1]);
        double l = t.norm();
        t /= l;
        Vector n{t.y, -t.x};

        outers.emplace_back(path[i-1] + w * n, path[i] + w * n);
        op.push_back(initial_range);
        inners.emplace_back(path[i-1] - w * n, path[i] - w * n);
        ip.push_back(initial_range);
    }

    // find the intersection points using the line parameters
    op.front().first = 0.;
    ip.front().first = 0.;

    for(size_t i = 1; i < outers.size(); i++)
    {
        auto a = outers[i-1].intersect(outers[i]);

        // if there is no intersection this line is parallel.  Use the standard interval
        if(!a.first)
            op[i] = {0,1};
        else
        {
            // otherwise record the intersection parameters
            op[i-1].second = a.second;
            op[i].first = outers[i].intersect(outers[i-1]).second;
        }

        a = inners[i-1].intersect(inners[i]);

        // if there is no intersection this line is parallel.  Use the standard interval
        if(!a.first)
            ip[i] = {0,1};
        else
        {
            // otherwise record the intersection parameters
            ip[i-1].second = a.second;
            ip[i].first = inners[i].intersect(inners[i-1]).second;
        }
    }

    op.back().second = 1.;
    ip.back().second = 1.;


    Point in = inners.front()(0),
          out = outers.front()(0);

    size_t j = 0, k = 0;

    // go through our parameters, and if any line segment goes backwards, skip it
    for(;;)
    {
        auto o = outers[j];
        auto i = inners[k];

        // do both segments go backwrds?
        if(op[j].first > op[j].second && ip[k].first > ip[k].second)
        {

        }
        // just the outer one goes backwards
        else if(op[j].first > op[j].second)
        {
            // we create one triangle using the inner segment for one side
            // and intersect the 
        }
        // just the inner one goes backwards
        else if(ip[k].first > ip[k].second)
        {

        }
        // neither go backwards
        else
        {
            ret.emplace_back(out, in, o(op[j].second));
            ret.emplace_back(in, o(op[j].second), i(ip[k].second));

            out = o(op[j].second);
            in = i(ip[k].second);

            j++;
            k++;
        }
    }

    return ret;
}

void MeshPlot::stroke(string name,     
                      vector<vector<Point>> plot, 
                      BrushStyle brush_style,
                      double brush_size, 
                      RGBA brush_color, 
                      pair<double,double> time_range)
{
    StrokePlot m{
        .name = name,
        .brush_style = brush_style,
        .brush_size = brush_size,
        .brush_color = brush_color,
        .time_range = time_range,
        .draw_mode = DrawMode::triangles,
        .arclength = 0,
        .strokelength = 0,
        .vertex_range = {0,0},
        .section_range = {0,0}
    };

    // prepare variables to track vertices and sections
    size_t start_vertex = 0;
    size_t start_section = 0;
    if(_strokes.size() > 0)
    {
        start_section = _strokes.back().section_range.second;
        start_vertex = _strokes.back().vertex_range.second;
    }
    size_t section = start_section;

    
    for(auto const & path : plot)
    {
        vector<Triangle> vertices, uvs;
        vector<tuple<size_t, size_t, size_t>> indices;

        stroke_path(path, brush_size, vertices, uvs, indices);

        for(size_t i = 0; i < vertices.size(); i++)
        {
            auto const & tr = vertices[i];
            auto const & uv = uvs[i];
            auto const & is = indices[i];

            _mesh.append(tr.p0, uv.p0, path[get<0>(is)], section, uv.p0.y * brush_size);
            _mesh.append(tr.p1, uv.p1, path[get<1>(is)], section, uv.p1.y * brush_size);
            _mesh.append(tr.p2, uv.p2, path[get<2>(is)], section, uv.p2.y * brush_size);
        }

        section++;
    }

}

void MeshPlot::stroke2(string name,
                      vector<vector<Point>> plot, 
                      BrushStyle brush_style,
                      double brush_size, 
                      RGBA brush_color, 
                      pair<double,double> time_range)
{
    StrokePlot m{
        .name = name,
        .brush_style = brush_style,
        .brush_size = brush_size,
        .brush_color = brush_color,
        .time_range = time_range,
        .draw_mode = DrawMode::triangle_strip,
        .arclength = 0,
        .strokelength = 0,
        .vertex_range = {0,0},
        .section_range = {0,0}
    };

    double last_cross_product = 0;
    double r = m.brush_size * 0.5;
    double s = 0;

    size_t start_vertex = 0;
    size_t start_section = 0;
    if(_strokes.size() > 0)
    {
        start_section = _strokes.back().section_range.second;
        start_vertex = _strokes.back().vertex_range.second;
    }
    size_t section = start_section;

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

    m.vertex_range = { start_vertex, _mesh.size() };
    m.section_range = { start_section, section };
    m.arclength = s;
    m.strokelength = s / m.brush_size;

    _strokes.emplace_back(m);
}


string ts(size_t level)
{
    return string(level * 3, ' ');
}

void MeshPlot::to_c(ostream & os) const
{
    auto stroke_to_c = [&](StrokePlot const & p) -> string { 
        return format(R"(   {{
        .name = "{}",
        .brush_style = {},
        .brush_size = {},
        .brush_color = {{ {}, {}, {}, {} }},
        .time_range = {{ {}, {} }},
        .draw_mode = {},
        .arclength = {},
        .strokelength = {},
        .vertex_range = {{ {}, {} }},
        .section_range = {{ {}, {} }},
    }})", 
            p.name,
            to_string(p.brush_style), p.brush_size, 
            p.brush_color.r, p.brush_color.g, p.brush_color.b, p.brush_color.a, 
            p.time_range.first, p.time_range.second, 
            to_string(p.draw_mode), p.arclength, p.strokelength, 
            p.vertex_range.first, p.vertex_range.second, 
            p.section_range.first, p.section_range.second);

    };
    auto strokes_to_c = [&]() -> string {
        string out = "{\n";
        for(StrokePlot const & p : _strokes)
        {
            out += stroke_to_c(p) + ",\n";
        }
        out += "\n}";
        return out;
    };
    auto attributes_to_c = [&]() -> string { 
        string out = "{\n";
        for(auto const & a : _mesh.attributes())
        {
            out += format(R"(   {{
        .name = "{}",
        .size = {},
        .offset = {},
    }},
            )", a.name(), a.size(), a.offset());
        }
        out += "\n}";
        return out;
    };
    auto uniforms_to_c = [&]() -> string { 
        // TODO: add more uniform details to the attribute_mesh
        string out = "{\n";
        for(auto const & u : _mesh.uniforms())
        {
            out += format(R"(   {{
        .name = "{}",
    }})", u.name());
        }
        out += "\n}";
        return out;
    };
    auto buffer_to_c = [&]() -> string { 
        string out = "{\n";
        auto i = _mesh.buffer_begin();
        for(size_t c = 0; i != _mesh.buffer_end(); ++i, ++c)
        {
            if(c % _mesh.stride() == 0)
                out += "\n";
            
            out += format("{},", *i);
        }
        out += "\n}";
        return out;
    };
    auto section_end_vertex_to_c = [&]() -> string {
        string out = "{";

        size_t section = 0;
        size_t vertex_index = 0;
        for(auto const & p : _mesh)
        {
            // get the section of this vertex
            size_t n = get<3>(p);

            if(n != section) 
            {
                out += format("{},", vertex_index);
            
                // TEMPORARY: to verify the logic above works
                if(n != section + 1)
                    throw std::logic_error(format("sections are not sequential at {} from {} to {}", vertex_index, section, n));
            }
            
            section = n;
            vertex_index++;
        }

        // output the total vertices as the last section
        out += format("{},", vertex_index);
        return out += "}";
    };
    auto section_arclength_to_c = [&]() -> string {
        string out = "{";

        size_t section = 0;
        double s = 0.;
        for(auto const & p : _mesh)
        {
            // append the arclength (brush.y)
            s = get<1>(p).y;

            // get the section of this vertex
            size_t n = get<3>(p);

            if(n != section)
                out += format("{},", s);
            
            section = n;
        }

        // output the total arclength as the last section
        out += format("{},", s);
        return out += "}";
    };

    double max_draw_time = 0.;
    for(StrokePlot const & p : _strokes)
        max_draw_time = max(max_draw_time, p.time_range.second);

    os << format(R"(
#include "mesh.h"

float _drawing_width = {};
float _drawing_height = {};
float _draw_time = {};
float _clear_color[4] = {{ {}, {}, {}, {} }};

uint _section_end_vertex[] = {};
float _section_cumulative_arclength[] = {};

Stroke const _strokes[] = {};
uint _stroke_count = {};

Attribute const _attributes[] = {};
uint _attribute_count = {};

Uniform const _uniforms[] = {};
uint _uniform_count = {};

uint _stride = {};

float const _buffer[] = {};
uint _buffer_size = {};
    )", 
        _drawing_size.first, _drawing_size.second, max_draw_time, 
        _clear_color.r, _clear_color.g, _clear_color.b, _clear_color.a,
        section_end_vertex_to_c(), section_arclength_to_c(),
        strokes_to_c(), _strokes.size(), 
        attributes_to_c(), _mesh.attribute_count,  
        uniforms_to_c(), _mesh.uniforms().size(),
        _mesh.stride(), 
        buffer_to_c(), _mesh.buffer_size());
}

void coordinate_space(vector<Point> const & path, 
                      vector<Vector> & tangents, 
                      vector<Vector> & normals,
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