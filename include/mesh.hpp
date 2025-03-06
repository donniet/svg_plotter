#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "point.hpp"

#include <vector>
#include <ranges>
#include <utility>
#include <tuple>
#include <algorithm>

using std::vector;
using std::ranges::views::zip;
using std::ranges::view;
using std::ranges::zip_view;
using std::pair, std::tuple;

class Fan;
class Strip;
class Triangles;

class Triangles
{
private:
    std::vector<Point> _points;
    std::vector<size_t> _id;
public:
    Triangles(std::vector<Point> && points, size_t first = 0);
    Triangles(std::vector<Point> && points, std::vector<size_t> && id);
    Triangles() = default;
    Triangles(Triangles &&) = default;
    Triangles(Triangles const &) = default;

    Triangle operator[](size_t) const;
};

class Strip
{
private:
    std::vector<Point> _points;
    std::vector<size_t> _id;
public:
    Strip(std::vector<Point> && points, size_t first = 0);
    Strip() = default;
    Strip(Strip &&) = default;
    Strip(Strip const &) = default;

    Triangles to_triangles() const;

    size_t triangle_count() const;

    Triangle operator[](size_t) const;

    operator std::vector<Point> const &() const;



};

class TriangleStrip 
{
private:
    vector<Point> _vertices;
    vector<Point> _uv;
    vector<double> _arclength;
    vector<Point> _brush;

    void adjust_vertices();
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

    bool contains(Point const & p) const;

    value_type operator[](size_t i) const;
};


#endif