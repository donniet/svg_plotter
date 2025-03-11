#ifndef __POINT_HPP__
#define __POINT_HPP__

#include <string>
#include <utility>



struct Point 
{
    double x, y;
    
    Point(double x0, double y0);
    Point();
    Point(Point const & r) = default;
    Point(Point && r) = default;
    
    Point & operator=(Point const & r);
    Point & operator=(Point && r);

    void swap(Point & p1);

    double norm2() const;
    double norm() const;

    Point operator-(Point const & p) const;
    Point operator+(Point const & p) const;
    Point & operator+=(Point const & p);
    Point & operator-=(Point const & p);
    Point operator*(double a) const;
    Point operator/(double a) const;
    Point & operator/=(double a);

    double dot(Point p) const;
    Point normalized() const;
    bool is_parallel(Point v, double epsilon = 1e-5) const;
    
    Point reflect(Point o = {0,0});
    double angle(Point v) const;
    Point rotate(double) const;
    Point rotate(Point) const;
    Point rotate_about(Point, double) const;

    bool operator==(Point const & p) const;
    bool operator!=(Point const & p) const;
};

using Vector = Point;

Vector normalize(Vector const &);
Vector normal(Vector const &);
double dot(Vector const &, Vector const &);

struct Line;
struct Segment;
struct HalfPlane;
struct Triangle;
struct Circle;
struct CircleSegment;

struct Line 
{  
    Point p;
    Vector v;

    Line() = default;
    Line(Line &&) = default;
    Line(Line const &) = default;
    Line(Point const &, Vector const &);

    double nearest(Point) const;
    double distance(Point) const;

    std::pair<bool, double> intersect(Line) const;
    std::pair<bool, double> intersect(Segment) const;
    std::pair<bool, double> intersect(HalfPlane) const;

    std::pair<double, double> intersect(Triangle) const;
    std::pair<double, double> intersect(Circle) const;
    std::pair<double, double> intersect(CircleSegment) const;

    Point operator()(double) const;
    void swap(Line &);
};

struct HalfPlane
{
    Point p;
    Vector n;

    HalfPlane() = default;
    HalfPlane(HalfPlane &&) = default;
    HalfPlane(HalfPlane const &) = default;
    HalfPlane(Point const &, Vector const &);

    bool contains(Point const &) const;

};

struct Segment
{
    Point p0, p1;

    Segment() = default;
    Segment(Segment &&) = default;
    Segment(Segment const &) = default;
    Segment(Point const &, Point const &);

    double distance(Point p) const;

    std::pair<bool, double> intersect(Line) const;
    std::pair<bool, double> intersect(Segment) const;
    std::pair<double,double> intersect(Triangle) const;
    std::pair<bool, double> intersect(Circle) const;
    std::pair<bool, double> intersect(CircleSegment) const;

    Point operator()(double) const;
    void swap(Segment &);
};

struct Triangle
{
    Point p0, p1, p2;

    Triangle() = default;
    Triangle(Triangle &&) = default;
    Triangle(Triangle const &) = default;
    Triangle(Point const &, Point const &, Point const &);

    bool contains(Point const &) const;
    std::pair<bool, Segment> intersect(Line) const;
    std::pair<bool, Segment> intersect(Segment) const;
    std::pair<bool, CircleSegment> intersect(Circle) const;
    std::pair<bool, CircleSegment> intersect(CircleSegment) const;

    Point operator()(double, double) const;
};

struct Circle
{
    Point o;
    Vector r;

    Circle() = default;
    Circle(Circle &&) = default;
    Circle(Circle const &) = default;
    Circle(Point, Vector);

    Point operator()(double) const;

    std::pair<bool, double> intersect(Line) const;
    std::pair<bool, double> intersect(Segment) const;
    std::pair<double, double> intersect(Triangle) const;
    std::pair<bool, double> intersect(Circle) const;
    std::pair<bool, double> intersect(CircleSegment) const;
};

struct CircleSegment
{
    Point o;
    Vector r;
    double a;

    CircleSegment();
    CircleSegment(CircleSegment &&) = default;
    CircleSegment(CircleSegment const &) = default;
    CircleSegment(Point, Vector, double);

    Point operator()(double) const;

    std::pair<bool, double> intersect(Line) const;
    std::pair<bool, double> intersect(Segment) const;
    std::pair<double, double> intersect(Triangle) const;
    std::pair<bool, double> intersect(Circle) const;
    std::pair<bool, double> intersect(CircleSegment) const;

};

struct Event :
    public Point 
{
    Vector dp;
    double dt;

    Event(double x0, double y0, double dx0 = 0., double dy0 = 0., double dt0 = 1.);
    Event(Point p0, double dx0 = 0., double dy0 = 0., double dt0 = 1.);
    Event(Point p0, Vector v0, double dt0 = 1.);
    Event();

    Vector dp_dt() const;
    bool is_move_to() const;
};

void make_move_to(Event & e);

Point operator*(double a, Point const & p);
double abs(Point p);


namespace std {
    string to_string(Point const & p);
    string to_string(Event const & p);
}


#endif