#include "mesh.hpp"
#include "json_writer.hpp"

#include <execution>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <utility>
#include <iostream>

using std::vector;
using std::for_each;
using std::distance;
using std::string;
using std::make_index_sequence, std::tuple_size, std::index_sequence;



static inline constexpr size_t triangle_strip_offsets[2][3] = {
    { 0, 1, 2 }, // even case
    { 1, 0, 2 }  // odd case
};

DrawMode::DrawMode() : _mode(triangles) { }
DrawMode::DrawMode(mode_type m) : _mode(m) { }
string DrawMode::to_string() const
{
    return string(mode_names[_mode]);
}
DrawMode::operator string () const
{
    return string(mode_names[_mode]);
}
DrawMode::operator int() const
{
    return _mode;
}
namespace std
{
    string to_string(DrawMode const & m)
    {
        return m.to_string();
    }
}

Mesh::Mesh(DrawMode mode) : 
    _draw_mode(mode), _next_id(0) 
{ }

Mesh::Mesh(DrawMode mode, std::vector<Point> && points, size_t first) :
    _draw_mode(mode), _vertices(move(points)), 
    _id(points.size()), _next_id(first)
{
    if(_vertices.size() == 0)
        return;

    using std::execution::par_unseq;

    for_each(par_unseq,
             _id.begin(), _id.end(),
    [&](size_t & id)
    {
        id = distance(&_id[0], &id) + first;
    });

    _next_id = _id.back() + 1;
}
Mesh::Mesh(DrawMode mode, std::vector<Point> && points, std::vector<size_t> ids) :
    _draw_mode(mode), _vertices(move(points)), _id(move(ids)), _next_id(0)
{
    if(_id.size() == 0)
        return;

    using std::execution::par_unseq;

    _next_id = *std::max_element(par_unseq, _id.begin(), _id.end()) + 1;
}

DrawMode Mesh::draw_mode() const 
{ 
    return _draw_mode; 
}

size_t Mesh::vertex_count() const 
{ 
    return _vertices.size(); 
}
size_t Mesh::triangle_count() const
{
    size_t v = vertex_count();
    switch(_draw_mode)
    {
    case DrawMode::triangles:
        return v / 3;
    case DrawMode::triangle_strip:
        return v - 2;
    }

    return v;
}

Point & Mesh::vertex(size_t i) 
{ 
    return _vertices[i]; 
}

Point const & Mesh::vertex(size_t i) const 
{ 
    return _vertices[i]; 
}

size_t & Mesh::id(size_t i) 
{ 
    return _id[i]; 
}

size_t const & Mesh::id(size_t i) const 
{ 
    return _id[i]; 
}

size_t Mesh::append(Point const & p, size_t id)
{
    if(id == std::numeric_limits<size_t>::max())
        id = _next_id++;

    _vertices.push_back(p);
    _id.push_back(id);
    return _vertices.size() - 1;
}

decltype(std::views::zip(Mesh::_vertices, Mesh::_id)) 
Mesh::data() 
{
    return std::views::zip(_vertices, _id);
}

vector<Point> const & Mesh::vertices() const
{
    return _vertices;
}
vector<size_t> const & Mesh::ids() const
{
    return _id;
}

UVMesh::UVMesh(DrawMode mode) :
    Mesh(mode)
{ }

UVMesh::UVMesh(DrawMode mode, std::vector<Point> && points, std::vector<Point> && uv) :
    Mesh(mode, move(points)), _uv(move(uv))
{ }

UVMesh::UVMesh(DrawMode mode,
               std::vector<Point> && points, 
               std::vector<Point> && uv, 
               std::vector<size_t> ids) :
    Mesh(mode, move(points), move(ids)), _uv(move(uv))
{ }

Point & UVMesh::uv(size_t i)
{
    return _uv[i];
}
Point const & UVMesh::uv(size_t i) const
{
    return _uv[i];
}

size_t UVMesh::append(Point const & vertex, Point const & uv, size_t id)
{
    _uv.push_back(uv);
    return Mesh::append(vertex, id);
}

decltype(std::views::zip(UVMesh::_vertices, UVMesh::_uv, UVMesh::_id)) 
UVMesh::data()
{
    return std::views::zip(_vertices, _uv, _id);
}

vector<Point> const & UVMesh::uvs() const
{
    return _uv;
}

Triangles::Triangles(std::vector<Triangle> const & tris) :
    Mesh(DrawMode::triangles)
{
    size_t id = 0;
    for(Triangle const & t : tris)
    {
        append(t.p0, id + 0);
        append(t.p1, id + 1);
        append(t.p2, id + 2);
        id += 3;
    }
}

Triangles::Triangles(std::vector<Point> && points, size_t first) :
    Mesh(DrawMode::triangles)
{ 
    using std::execution::par_unseq;

    // parallel iota
    for_each(par_unseq,
             _id.begin(), _id.end(),
    [&](size_t & i)
    {
        i = distance(&_id[0], &i) + first;
    });
}

Triangles::Triangles(std::vector<Point> && points, std::vector<size_t> && id) :
    Mesh(DrawMode::triangles, move(points), move(id))
{ }


Triangle Triangles::operator[](size_t i) const
{
    return Triangle(
        vertex(i * 3 + 0),
        vertex(i * 3 + 1),
        vertex(i * 3 + 2)
    );
}

Triangles Triangles::from_cover(MeshCover const & c)
{
    return Triangles{c.triangles()};
}


Strip::Strip(vector<Point> && points, size_t first) :
    Mesh(DrawMode::triangle_strip, move(points))
{ 
    using std::execution::par_unseq;

    // parallel iota
    for_each(par_unseq,
             _id.begin(), _id.end(),
    [&](size_t & i)
    {
        i = distance(&_id[0], &i) + first;
    });
}


Strip::Strip(vector<Point> && points, vector<size_t> && id) :
    Mesh(DrawMode::triangle_strip, move(points), move(id))
{ }


Triangles Strip::to_triangles() const
{
    using std::execution::par_unseq;

    if(_vertices.size() < 3)
        return Triangles{};

    vector<Point>   v(triangle_count() * 3);
    vector<size_t> id(triangle_count() * 3);

    for_each(par_unseq,
             v.begin(), v.end(),
    [&](Point & p)
    {
        size_t gid = distance(&v[0], &p);
        size_t tri = gid / 3;
        size_t j = gid % 3;

        size_t point_index = tri + triangle_strip_offsets[tri % 2][j];

        p = _vertices[point_index];
        id[gid] = _id[point_index];
    });

    return Triangles(move(v), move(id));
}

Triangle Strip::operator[](size_t i) const
{
    return Triangle(
        _vertices[i + triangle_strip_offsets[i % 2][0]],
        _vertices[i + triangle_strip_offsets[i % 2][1]],
        _vertices[i + triangle_strip_offsets[i % 2][2]]
    );
}

// size_t MeshRender::calculate_stride() 
// {
//     size_t ret = 4; // position.xy and uv.xy

//     for(auto const & a : _attributes)
//         ret += _attributes.size();

//     return ret;
// }

// // TODO: add escape sequences
// string quote(string s)
// {
//     return "\"" + s + "\"";
// }

// void MeshRender::write_attributes(std::ostream & os, std::string const & tab) 
// {
//     size_t stride = calculate_stride();
//     size_t offset = 0;

//     os << tab << quote("a_position") << ": [ 2, " << stride << ", " << offset << " ],";
//     offset += 2;

//     os << tab << quote("a_uv") << ": [ 2, " << stride << ", " << offset << " ],";
//     offset += 2;

//     for(auto const & a : _attributes)
//     {
//         os << tab << quote(a.name()) << ": [ " << a.size() << ", " << stride << ", " << offset << " ],";
//         offset += a.size(); 
//     }
// } 

// void MeshRender::write_uniforms(std::ostream & os, std::string const & tab) 
// {
//     for(auto const & u : _uniforms)
//     {
//         os << tab << quote(u.name()) << ": ";
//         write_uniform_value(os, u);
//         os << ",";
//     }
// }

// template<typename Tuple, size_t ... Is>
// void write_data_tuple(ostream & os, Tuple const & tup, string const & sep, integer_sequence<Is...>)
// {
//     ((os << get<Is>(tup) << sep), ...);
// }

// void MeshRender::write_buffer_data(std::ostream & os, std::string const & tab) 
// {
//     auto dat = _mesh->data(); 

//     for(auto i = dat.begin(); i != dat.end(); ++i)
//     {
//         os << tab;

//         write_data_tuple(os, *i, ", ", make_index_sequence<tuple_size<attribute_tuple_type>>{});
//     }
// }

// void MeshRender::save(std::ostream & os) 
// {
//     string tab = "\n";
//     auto increase_tab = [&tab]() { tab += "\t"; };
//     auto decrease_tab = [&tab]() { tab = tab.substr(0, tab.size() - 1); };

//     os << "{" << tab;
//     increase_tab();
//     os << tab << quote("draw_mode") << ": " << quote(_mesh->draw_mode()) << ",";
//     os << tab << quote("vertex_count") << ": " << _mesh->vertex_count() << ",";
//     os << tab << quote("attributes") << ": {";
//     increase_tab();
     
//     write_attributes(os, tab); 

//     decrease_tab();
//     os << tab << "},";
//     os << tab << quote("uniforms") << ": {";
//     increase_tab();

//     write_uniforms(os, tab);

//     decrease_tab();
//     os << tab << "}.";
//     os << tab << quote("buffer_data") << ": [";
//     increase_tab();

//     write_buffer_data(os, tab);

//     decrease_tab();
//     os << tab << "]";
//     decrease_tab();

//     os << tab << "}\n";
// }


TriangleStrip::TriangleStrip() { }

TriangleStrip::TriangleStrip(vector<Point> && vertices, 
              vector<Point> && uv,
              vector<double> && arclength,
              vector<Point> && brush) :
    _vertices(vertices), _uv(uv), _arclength(arclength), _brush(brush)
{ 
    adjust_vertices();
}

void TriangleStrip::adjust_vertices()
{
    if (_vertices.size() < 3) {
        return;
    }

    std::vector<Triangle> triangles;
    for (size_t i = 0; i < _vertices.size() - 2; ++i) {
        if (i % 2 == 0) {
            triangles.push_back({_vertices[i], _vertices[i + 1], _vertices[i + 2]});
        } else {
            triangles.push_back({_vertices[i + 1], _vertices[i], _vertices[i + 2]});
        }
    }

    int maxIterations = 1000; // Set a maximum number of iterations
    int iterationCount = 0;
    bool modified = true;

    while (modified && iterationCount < maxIterations) {
        modified = false;
        iterationCount++;

        for (size_t vertexIndex = 0; vertexIndex < _vertices.size(); ++vertexIndex) {
            Point& vertex = _vertices[vertexIndex];

            for (const Triangle& triangle : triangles) {
                if (triangle.contains(vertex)) {
                    // Move the vertex slightly outside the triangle
                    Point center = {(triangle.p0.x + triangle.p1.x + triangle.p2.x) / 3.0,
                                    (triangle.p0.y + triangle.p1.y + triangle.p2.y) / 3.0};
                    Vector direction = {vertex.x - center.x, vertex.y - center.y};
                    double length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

                    if (length > 0) {
                        direction.x /= length;
                        direction.y /= length;
                    } else {
                        direction = {1.0, 0.0};
                    }

                    vertex.x += direction.x * 0.1;
                    vertex.y += direction.y * 0.1;

                    modified = true;
                    break; // Move to the next vertex after modification
                }
            }
            if(modified) break; // move to the next iteration.
        }
    }

    if (iterationCount >= maxIterations) {
        std::cout << "Warning: Maximum iterations reached. Movement might not be fully resolved." << std::endl;
    }
}

size_t TriangleStrip::size() const
{
    return _vertices.size();
}

TriangleStrip::iterator TriangleStrip::begin()
{
    return zip(_vertices, _uv, _arclength, _brush).begin();
}
TriangleStrip::iterator TriangleStrip::end()
{
    return zip(_vertices, _uv, _arclength, _brush).end();
}

vector<Point> const & TriangleStrip::vertices() const 
{
    return _vertices;
}
vector<Point> const & TriangleStrip::uv() const
{
    return _uv;
}

TriangleStrip::value_type TriangleStrip::operator[](size_t i) const
{
    return { _vertices[i], _uv[i], _arclength[i], _brush[i] };
}

bool TriangleStrip::contains(Point const & p) const
{
    for(size_t i = 2; i < _vertices.size(); ++i)
    {
        if(Triangle(_vertices[i-2], 
                    _vertices[i-1], 
                    _vertices[i  ]).contains(p))
            return true;
    }

    return false;
}