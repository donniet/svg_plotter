#include "mesh.hpp"

#include <execution>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>

#include <iostream>

using std::vector;
using std::for_each;
using std::distance;


Triangles::Triangles(std::vector<Point> && points, size_t first) :
    _points(move(points)),
    _id(points.size())
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
    _points(move(points)),
    _id(move(id))
{ }

Triangle Triangles::operator[](size_t i) const
{
    return Triangle(
        _points[i / 3 + 0],
        _points[i / 3 + 1],
        _points[i / 3 + 2]
    );
}

Strip::Strip(vector<Point> && points, size_t first) :
    _points(move(points)), 
    _id(points.size())
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

static inline constexpr size_t triangle_strip_offsets[2][3] = {
    { 0, 1, 2 }, // even case
    { 1, 0, 2 }  // odd case
};

Triangles Strip::to_triangles() const
{
    using std::execution::par_unseq;

    if(_points.size() < 3)
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

        p = _points[point_index];
        id[gid] = _id[point_index];
    });

    return Triangles(move(v), move(id));
}

Strip::operator std::vector<Point> const &() const
{
    return _points;
}

Triangle Strip::operator[](size_t i) const
{
    return Triangle(
        _points[i + triangle_strip_offsets[i % 2][0]],
        _points[i + triangle_strip_offsets[i % 2][1]],
        _points[i + triangle_strip_offsets[i % 2][2]]
    );
}

size_t Strip::triangle_count() const
{
    if(_points.size() < 3)
        return 0;

    return _points.size() - 2;
}



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