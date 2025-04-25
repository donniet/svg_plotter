#include "cover.hpp"

// cover.cpp (Implementation file)

#include "cover.hpp"
// #include "path.hpp"
#include <limits>
#include <vector>
#include <list>
#include <stdexcept>
#include <cmath>

using std::vector;
using std::list;
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


MeshCover::MeshCover() : _tris() 
{ }

MeshCover::MeshCover(std::vector<Point> const & outline) : 
    _tris(move(ear_clip(outline))), _outline(outline)
{ }

double MeshCover::area() const 
{
    double area = 0.;
    for(Triangle const & tri : _tris)
        area += tri.area();

    return area;
}

double MeshCover::perimeter() const 
{
    double p = 0.;
    for(size_t i = 1; i < _outline.size(); i++)
        p += (_outline[i] - _outline[i-1]).norm();

    return p;
}

vector<vector<Point>> MeshCover::outline() const
{
    return vector<vector<Point>>{_outline};
}

bool MeshCover::is_inside(Point const & p) const
{
    for(Triangle const & t : _tris)
        if(t.contains(p))
            return true;

    return false;
}

pair<bool, double> MeshCover::intersect_ray(Point const & origin, Vector const & v) const
{
    if(is_inside(origin))
        return {true, 0.};
    
    Ray ray{origin, v};

    double s = numeric_limits<double>::max();
    for(Triangle const & t : _tris)
    {
        auto i = ray.intersect(t);

        if(i.first <= i.second)
            s = i.second;
    }

    if(s < numeric_limits<double>::max())
        return {true, s};

    return {false, 0.};
}

pair<Point, double> MeshCover::nearest(Point const & p) const
{
    throw std::logic_error("nearest point to a meshcover is not implemented");
}

vector<Triangle> MeshCover::ear_clip(vector<Point> const & outline)
{
    vector<Triangle> ret;

    // less than three points is not a mesh
    if(outline.size() < 3)
        return ret;

    size_t remaining = outline.size();

    struct List
    {
        Point p;
        List * next, * prev;
    };

    auto unlink = [&remaining](List * l)
    {
        // are we the last element?
        if(l->next == l)
        {
            l->next = nullptr;
            l->prev = nullptr;
            return;
        }

        List * n = l->next;
        List * p = l->prev;

        n->prev = p;
        p->next = n;
        l->next = l->prev = nullptr;
        remaining--;
    };

    auto is_ear = [](List * n) -> bool
    {
        List * l = n->next->next;
        
        if(n->next == n || l == n) 
            throw std::logic_error("is_ear detected less than 3 points in the outline");

        Triangle ear{n->prev->p, n->p, n->next->p};

        // loop through until we hit prev
        for(; l != n->prev; l = l->next)
            if(ear.contains(l->p))
                return false;

        // no interior points found
        return true;
    };

    List * head = new List{outline[0], nullptr, nullptr};
    head->prev = head->next = head;

    // create a cyclic list of all the points
    for(size_t i = 1; i < outline.size(); i++)
    {
        List * n = head;
        List * p = head->prev;

        List * l = new List{ outline[i], n, p};
        n->prev = l;
        p->next = l;
    }

    // remove any points that are straight (not triangles)
    for(size_t i = 0, N = remaining; i < N; i++)
    {
        Vector v0{head->prev->p - head->p};
        Vector v1{head->next->p - head->p};

        if(std::abs(cross(v0, v1)) < 1e-4)
        {
            List * n = head->next;
            unlink(head);
            delete head;
            head = n;

            if(remaining < 3)
                throw std::logic_error("ear_clip: less than 3 remaining non-linear vertices");
        }
        else 
        {
            head = head->next;
        }
    }
 
    // check each for ears
    while(head->next->next != head->prev)
    {
        List * n = head;

        for(; !is_ear(n); n = n->next)
            if(n->next == head)
                throw std::logic_error("no ears found");

        // create an ear an unlink
        ret.emplace_back(n->prev->p, n->p, n->next->p);
        head = n->next;
        unlink(n);
        delete n;
    }

    // add the last one
    ret.emplace_back(head->prev->p, head->p, head->next->p);

    // cleanup
    List * t = head->next;
    unlink(t);
    delete t;

    t = head->prev;
    unlink(t);
    delete t;

    delete head;

    return move(ret);
}

vector<Triangle> const & MeshCover::triangles() const
{
    return _tris;
}


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