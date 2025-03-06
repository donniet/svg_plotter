#include "mesh.hpp"

#include <execution>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>

#include <iostream>

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