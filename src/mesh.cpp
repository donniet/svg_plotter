#include "mesh.hpp"


TriangleStrip::TriangleStrip() { }

TriangleStrip::TriangleStrip(vector<Point> && vertices, 
              vector<Point> && uv,
              vector<double> && arclength,
              vector<Point> && brush) :
    _vertices(vertices), _uv(uv), _arclength(arclength), _brush(brush)
{ }


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