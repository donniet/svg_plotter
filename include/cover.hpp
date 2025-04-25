// cover.hpp (Header file)

#ifndef __COVER_HPP__
#define __COVER_HPP__

#include "point.hpp"
#include "forward_deps.hpp"

#include <limits>
#include <utility> // For std::pair
#include <vector>

class BoundingBox; // forward declaration

class Cover
{
private:
    double _margin;

public:
    virtual double area() const;
    virtual double perimeter() const;
    virtual std::vector<std::vector<Point>> outline() const;
    virtual double & margin();
    virtual double margin() const;
    virtual bool is_inside(Point const & p) const;
    virtual std::pair<bool, double> intersect_ray(Point const & origin, Vector const & direction) const;
    virtual std::pair<Point, double> nearest(Point const &) const;

    Cover();
    virtual ~Cover() = default; // Add a virtual destructor
};

class MeshCover :
    public Cover
{
private:
    std::vector<Triangle> _tris;
    std::vector<Point> _outline;
protected:
    static std::vector<Triangle> ear_clip(std::vector<Point> const & outline);
public:
    MeshCover();
    MeshCover(std::vector<Point> const & outline);
    MeshCover(MeshCover const &) = default;
    MeshCover(MeshCover &&) = default;

    MeshCover& operator=(MeshCover const &) = default;
    MeshCover& operator=(MeshCover &&) = default;

    virtual double area() const override;
    virtual double perimeter() const override;
    virtual std::vector<std::vector<Point>> outline() const override;
    virtual bool is_inside(Point const & p) const override;
    virtual std::pair<bool, double> intersect_ray(Point const & origin, Vector const & ray) const override;
    virtual std::pair<Point, double> nearest(Point const &) const override;

    virtual ~MeshCover() = default;
};

class BoundingBox : 
    public Cover
{
public:
    Point p0, p1;

public:
    virtual double area() const override;
    virtual double perimeter() const override;
    Vector diagonal() const;
    std::vector<Point> corners() const;
    std::vector<Segment> sides() const;
    virtual std::vector<std::vector<Point>> outline() const override;
    virtual bool is_inside(Point const & p) const override;
    virtual std::pair<bool, double> intersect_ray(Point const & origin, Vector const & ray) const override;
    virtual std::pair<Point, double> nearest(Point const &) const override;

    BoundingBox(Point q0, Point q1);
    BoundingBox(double x0, double y0, double x1, double y1);
    BoundingBox();
};


#endif // __COVER_HPP__