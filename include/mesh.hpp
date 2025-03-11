#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "point.hpp"

#include <vector>
#include <ranges>
#include <utility>
#include <tuple>
#include <algorithm>
#include <string>
#include <memory>
#include <limits>
#include <array>

using std::vector;
using std::ranges::views::zip;
using std::ranges::view;
using std::ranges::zip_view;
using std::pair, std::tuple;
using std::string;

class Fan;
class Strip;
class Triangles;

class DrawMode
{
public:
    typedef enum {
        triangles = 0,
        triangle_strip = 1,
    } mode_type;

private:
    static inline constexpr const char * mode_names[] = {
        "TRIANGLES",
        "TRIANGLE_STRIP",
    };

    mode_type _mode;

public:
    std::string to_string() const;
    operator std::string () const;

    DrawMode();
    DrawMode(DrawMode &&) = default;
    DrawMode(DrawMode const &) = default;
    DrawMode(mode_type);

    operator int() const;

    DrawMode & operator=(DrawMode const &) = default;
};

class Mesh
{
protected:
    DrawMode _draw_mode;

    std::vector<Point> _vertices;
    std::vector<size_t> _id;
    size_t _next_id;
public:
    Mesh(DrawMode = DrawMode{});
    Mesh(Mesh &&) = default;
    Mesh(Mesh const &) = default;

    Mesh(DrawMode, std::vector<Point> && points, size_t first = 0);
    Mesh(DrawMode, std::vector<Point> && points, std::vector<size_t> ids);

    Mesh & operator=(Mesh &&) = default;
    Mesh & operator=(Mesh const &) = default;

    DrawMode draw_mode() const;
    size_t vertex_count() const;
    size_t triangle_count() const;

    Point & vertex(size_t);
    size_t & id(size_t);

    Point const & vertex(size_t) const;
    size_t const & id(size_t) const;

    size_t append(Point const &, size_t = std::numeric_limits<size_t>::max());

    // tuple<Point &, size_t &> operator[](size_t);
    // tuple<Point const &, size_t const &> operator[](size_t) const;

    decltype(std::views::zip(_vertices, _id)) data();

    std::vector<Point> const & vertices() const;
    std::vector<size_t> const & ids() const;
};

class UVMesh :
    public Mesh
{
protected:
    using Mesh::append;

    std::vector<Point> _uv;
public:
    UVMesh(DrawMode = DrawMode{});
    UVMesh(UVMesh &&) = default;
    UVMesh(UVMesh const &) = default;

    UVMesh(DrawMode, std::vector<Point> && points, std::vector<Point> && uv);
    UVMesh(DrawMode, std::vector<Point> && points, std::vector<Point> && uv, std::vector<size_t> ids);

    Mesh & operator=(Mesh&&) = delete;
    Mesh & operator=(Mesh const &) = delete;

    UVMesh & operator=(UVMesh &&) = default;
    UVMesh & operator=(UVMesh const &) = default;

    Point & uv(size_t);
    Point const & uv(size_t) const;

    size_t append(Point const &, Point const &, size_t = std::numeric_limits<size_t>::max());

    decltype(std::views::zip(_vertices, _uv, _id)) data();

    std::vector<Point> const & uvs() const;
};

class Triangles :
    public Mesh
{
public:
    Triangles(std::vector<Point> && points, size_t first = 0);
    Triangles(std::vector<Point> && points, std::vector<size_t> && id);
    Triangles() = default;
    Triangles(Triangles &&) = default;
    Triangles(Triangles const &) = default;

    Triangle operator[](size_t) const;
};

class Strip : 
    public Mesh
{
public:
    Strip(std::vector<Point> && points, size_t first = 0);
    Strip(std::vector<Point> && points, std::vector<size_t> && id);
    Strip() = default;
    Strip(Strip &&) = default;
    Strip(Strip const &) = default;

    Triangles to_triangles() const;

    Triangle operator[](size_t) const;
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