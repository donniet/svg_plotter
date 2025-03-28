#include "cover.hpp"

// cover.cpp (Implementation file)

#include "cover.hpp"
#include "path.hpp"
#include <limits>
#include <vector>

using std::vector;
using std::swap;
using std::min, std::max;
using std::pair;

using std::numeric_limits;

double Cover::area() const {
    return 0.;
}

double Cover::perimeter() const {
    return 0.;
}

vector<vector<Point>> Cover::outline() const {
    return vector<vector<Point>>{};
}

double & Cover::margin() {
    return _margin;
}

double Cover::margin() const {
    return _margin;
}

bool Cover::is_inside(Point const & p) const {
    return false;
}

pair<bool, double> Cover::intersect_ray(Point const & origin, Vector const & direction) const {
    return {false, 0.};
}

pair<Point,double> Cover::nearest(Point const & p) const { 
    return { Point{}, 0. }; 
}

Cover::Cover() : _margin(-numeric_limits<double>::epsilon()) 
{ }


double BoundingBox::area() const {
    Point a = (p1 - p0);
    return a.x * a.y;
}

double BoundingBox::perimeter() const {
    Point a = (p1 - p0);
    return 2 * a.x + 2 * a.y;
}

Vector BoundingBox::diagonal() const {
    return Vector(p1 - p0);
}

vector<Point> BoundingBox::corners() const {
    return {
        Point{p0.x, p0.y},
        Point{p0.x, p1.y},
        Point{p1.x, p1.y},
        Point{p1.x, p0.y}
    };
}

vector<Segment> BoundingBox::sides() const
{
    Point p01{p0.x, p1.y};
    Point p10{p1.x, p0.y};

    return {
        Segment{p0, p01},
        Segment{p01, p1},
        Segment{p1, p10},
        Segment{p10, p0}
    };
}

vector<vector<Point>> BoundingBox::outline() const {
    return vector<vector<Point>>({corners()});
}

bool BoundingBox::is_inside(Point const & p) const {
    return p.x >= p0.x + margin() && p.x <= p1.x - margin() &&
           p.y >= p0.y + margin() && p.y <= p1.y - margin();
}

pair<Point,double> BoundingBox::nearest(Point const & p) const 
{
    if(is_inside(p))
        return { p, 0. };

    auto a = sides();
    double d = numeric_limits<double>::max();
    Point r = p;

    for(Segment const & s : a)
    {
        double t = s.nearest(p);
        Point rr = s(t);
        double dd = (p - rr).norm();

        if(dd >= d)
            continue;

        r = rr;
        d = dd;
    }

    return { r, d };
}

std::pair<bool, double> BoundingBox::intersect_ray(Point const & origin, Vector const & ray) const {
    if (ray.is_parallel(Vector{1, 0})) {
        if (origin.y < p0.y || origin.y > p1.y)
            return {false, 0};

        double t0 = (p0.x - origin.x) / ray.x;
        double t1 = (p1.x - origin.x) / ray.x;

        if (t0 > t1)
            swap(t0, t1);

        if (t1 <= 0)
            return {false, 0};

        if (t0 <= 0)
            return {true, t1};

        return {true, t0};
    }

    double t0 = (p0.y - origin.y) / ray.y;
    double t1 = (p1.y - origin.y) / ray.y;

    if (t0 > t1)
        swap(t0, t1);

    if (ray.is_parallel(Vector{0, 1})) {
        if (origin.x < p0.x || origin.x > p1.x)
            return {false, 0};

        if (t1 <= 0)
            return {false, 0};

        if (t0 <= 0)
            return {true, t1};

        return {true, t0};
    }

    double u0 = (p0.x - origin.x) / ray.x;
    double u1 = (p1.x - origin.x) / ray.x;

    if (u0 > u1)
        swap(u0, u1);

    auto intersect_intervals = [](std::pair<double, double> a, std::pair<double, double> b) {
        return std::make_pair(std::max(a.first, b.first), std::min(a.second, b.second));
    };

    std::pair<double, double> intersection = intersect_intervals({t0, t1}, {u0, u1});

    if (intersection.first > intersection.second)
        return {false, 0};

    if (intersection.second <= 0)
        return {false, 0};

    if (intersection.first <= 0)
        return {true, intersection.second};

    return {true, intersection.first};
}

BoundingBox::BoundingBox(Point q0, Point q1) :
    p0(min(q0.x, q1.x), min(q0.y, q1.y)),
    p1(max(q0.x, q1.x), max(q0.y, q1.y)) {}

BoundingBox::BoundingBox(double x0, double y0, double x1, double y1) :
    p0(min(x0, x1), min(y0, y1)),
    p1(max(x0, x1), max(y0, y1)) {}

BoundingBox::BoundingBox() :
    p0(0, 0), p1(0, 0) {}