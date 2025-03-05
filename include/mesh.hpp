#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "point.hpp"

#include <vector>
#include <ranges>
#include <utility>
#include <tuple>

using std::vector;
using std::ranges::views::zip;
using std::ranges::view;
using std::ranges::zip_view;
using std::pair, std::tuple;

class TriangleStrip 
{
private:
    vector<Point> _vertices;
    vector<Point> _uv;
    vector<double> _arclength;
    vector<Point> _brush;
public:
    TriangleStrip();

    TriangleStrip(vector<Point> && vertices, 
                  vector<Point> && uv,
                  vector<double> && arclength,
                  vector<Point> && brush);

    using value_type = tuple<Point,Point,double,Point>;
    using collection_type = decltype(zip(_vertices, _uv, _arclength, _brush));
    using iterator = decltype(zip(_vertices, _uv, _arclength, _brush).begin());
    
    size_t size() const;
    iterator begin();
    iterator end();

    vector<Point> const & vertices() const;
    vector<Point> const & uv() const;

    value_type operator[](size_t i) const;
};


#endif