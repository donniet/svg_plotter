#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "point.hpp"

#include <vector>
#include <ranges>
#include <utility>

using std::vector;
using std::ranges::views::zip;
using std::ranges::view;
using std::ranges::zip_view;
using std::pair;

class TriangleStrip 
{
private:
    vector<Point> _vertices;
    vector<Point> _uv;
public:
    TriangleStrip() { }

    TriangleStrip(vector<Point> && vertices, vector<Point> && uv) :
        _vertices(vertices), _uv(uv) 
    { }

    using value_type = pair<Point,Point>;
    using collection_type = decltype(zip(_vertices, _uv));
    using iterator = decltype(zip(_vertices, _uv).begin());
    
    size_t size() const
    {
        return _vertices.size();
    }

    iterator begin()
    {
        return zip(_vertices, _uv).begin();
    }
    iterator end()
    {
        return zip(_vertices, _uv).end();
    }

    vector<Point> const & vertices() const 
    {
        return _vertices;
    }
    vector<Point> const & uv() const
    {
        return _uv;
    }

    pair<Point,Point> operator[](size_t i) const
    {
        return { _vertices[i], _uv[i] };
    }
};


#endif