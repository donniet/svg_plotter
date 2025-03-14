#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "drawable.hpp"
#include "point.hpp"
#include "bezier.hpp"
#include "formatters.hpp"
#include "cover.hpp"

#include <memory>

struct PathSegment;

struct MoveTo :
    public Drawable
{
protected:
    Point _from, _to;
public:
    // length is 0, no need to override
    virtual std::pair<bool, double> last_move_between(double t0, double t1) const override;
    virtual Event at(double t) const override;
    MoveTo(Point from, Point to);
};

struct LineTo :
    public Drawable
{
protected:
    Point _from, _to;
    double _length;
public:
    virtual double length(double t0, double t1) const override;
    virtual Event at(double t) const override;
    LineTo(Point from, Point to);
};


struct CubicBezier : 
    public DrawableFromFunction
{
protected:
    Point _from, _c1, _c2, _to;

    using DrawableFromFunction::reset;
private:
public:
    CubicBezier(Point from, Point c1, Point c2, Point to);

    Point operator()(double t) const;
};

struct QuadraticCurve : 
    public DrawableFromFunction
{
protected:
    Point _from, _c1, _to;

    using DrawableFromFunction::reset;
private:
public:
    QuadraticCurve(Point from, Point c1, Point to);
    Point operator()(double t) const;
};



struct Arc :
    public DrawableFromFunction
{
protected:
    Point _from, _to;

    Point _radius;
    double _angle;
    bool _large_arc;
    bool _sweep;

    Point _center;
    double _start_angle;
    double _sweep_angle;

    void init();

    using DrawableFromFunction::reset;
public:
    Point operator()(double t) const;

    Arc(Point from, Point radius, double angle, bool large_arc, bool sweep, Point to);
};



class SVGPath :
    public Drawable
{
public:

    SVGPath(const std::string& pathData, Point const & origin = Point{0,0});

    virtual double length(double t0, double t1) const override;
    virtual Event at(double t) const override;
    virtual BoundingBox bounding_box() const override;
    virtual std::pair<bool, double> last_move_between(double t0, double t1) const override;

private:
    // assumes _segments.size() > 0
    std::pair<size_t, double> segment_by_parameter(double t) const;
    std::pair<double, double> parameter_range(size_t i) const;

    double length_to(double t) const;

    void adjust_box(Point p);
    void adjust_box(BoundingBox const & b);

    void append_segments(std::string::value_type key, std::vector<double> const & coords, Point & pen, Vector & dir);
    void parsePath(const std::string& pathData);


    std::vector<std::unique_ptr<Drawable>> _segments;
    std::vector<double> _lengths;
    std::vector<double> _length_index;
    Point _from, _to;
    BoundingBox _box;
};



#endif