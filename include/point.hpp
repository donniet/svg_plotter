#ifndef __POINT_HPP__
#define __POINT_HPP__

#include <vector>
#include <string>
#include <utility>

#include "earcut.hpp"

class Point;
using Vector = Point;
struct Line;
struct Ray;
struct Segment;
struct HalfPlane;
struct Triangle;
struct Circle;
struct CircleSegment;

struct Transform
{
    double mat[6];

    static Transform translate(double);
    static Transform translate(Point const &);
    static Transform rotate(double);
    static Transform rotate(double, Point const &);
    static Transform skewX(double);
    static Transform skewY(double);
    static Transform scale(double);
    static Transform scale(Vector const &);
    static Transform matrix(double, double, double, double, double, double);
    static Transform matrix(double[6]);

    Transform();
    Transform(Transform const &) = default;
    Transform(Transform &&) = default;
    Transform(double, double, double, double, double, double);

    Transform & operator=(Transform const &) = default;
    Transform & operator=(Transform &&) = default;

    Point operator()(Point const &) const;
    Transform operator()(Transform const &) const;

    Point operator*(Point const &) const;
    Transform operator*(Transform const &) const;
};

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

    double cross(Point const & p) const;

    bool operator==(Point const & p) const;
    bool operator!=(Point const & p) const;
};



class Point3
{
public:
    double x, y, z;

    Point3();
    Point3(Point3 const &) = default;
    Point3(Point3 &&) = default;

};

class Path : 
    public std::vector<Point>
{
public:
    Path(Path const &) = default;
    Path(Path &&) = default;
    Path(vector<Point> const &, bool = false);
    Path(vector<Point> &&, bool = false);

    Path offset(double) const;
    bool closed() const;
private:
    bool _closed;
};

namespace mapbox {
    namespace util {
        template <>
        struct nth<0, Point> {
            inline static auto get(const Point &t) {
                return t.x;
            };
        };
        template <>
        struct nth<1, Point> {
            inline static auto get(const Point &t) {
                return t.y;
            };
        };
        
    }
}
namespace std 
{
    // template<size_t I>
    // auto get(Point const & p);

    template<>
    struct tuple_element<0, Point>
    {
        using type = double;
    };

    template<>
    struct tuple_element<1, Point>
    {
        using type = double;
    };



    // template<>
    // double const & get<0, Point const>(Point const & p)
    // {
    //     return p.x;
    // }

    // template<>
    // double const & get<1, Point const>(Point const & p)
    // {
    //     return p.y;
    // }
}

void swap(Point & p0, Point & p1);
Vector normalize(Vector const &);
Vector normal(Vector const &);
double dot(Vector const &, Vector const &);
double cross(Vector const & u, Vector const & v);


struct RGB
{
    double r, g, b;

    RGB();
    RGB(double, double, double);
    RGB(RGB const &) = default;
    RGB(RGB &&) = default;

    RGB & operator=(RGB const &) = default;
    RGB & operator=(RGB &&) = default;

    void swap(RGB &);
};

struct RGBA :
    public RGB
{
    double a;

    static RGBA from_hex(std::string const &);

    RGBA();
    RGBA(double, double, double, double);
    RGBA(RGB const &, double);
    RGBA(RGBA const &) = default;
    RGBA(RGBA &&) = default;

    RGBA & operator=(RGBA const &) = default;
    RGBA & operator=(RGBA &&) = default;

    void swap(RGBA &);
};

struct Line 
{  
    Point p;
    Vector v;

    Line() = default;
    Line(Line &&) = default;
    Line(Line const &) = default;
    Line(Point const &, Vector const &);

    Line & operator=(Line const &) = default;
    Line & operator=(Line &&) = default;

    double nearest(Point) const;
    double distance(Point) const;

    std::pair<bool, double> intersect(Line) const;
    std::pair<bool, double> intersect(Ray) const;
    std::pair<bool, double> intersect(Segment) const;
    std::pair<bool, double> intersect(HalfPlane) const;

    std::pair<double, double> intersect(Triangle) const;
    std::pair<double, double> intersect(Circle) const;
    std::pair<double, double> intersect(CircleSegment) const;

    bool operator==(Line const &) const;

    Point operator()(double) const;
    void swap(Line &);

    ~Line() = default;
};

struct Ray
{
    Point p;
    Vector v;
    
    Ray() = default;
    Ray(Ray &&) = default;
    Ray(Ray const &) = default;
    Ray(Point const &, Vector const &);

    double nearest(Point) const;
    double distance(Point) const;

    std::pair<bool, double> intersect(Line) const;
    std::pair<bool, double> intersect(Ray) const;
    std::pair<bool, double> intersect(Segment) const;
    std::pair<bool, double> intersect(HalfPlane) const;

    std::pair<double, double> intersect(Triangle const &) const;
    std::pair<double, double> intersect(Circle) const;
    std::pair<double, double> intersect(CircleSegment) const;

    Point operator()(double) const;
    void swap(Ray &);
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

    double nearest(Point p) const;
    double distance(Point p) const;
    double length() const;
    Line line() const;

    
    std::pair<bool, double> intersect(Line) const;
    std::pair<bool, double> intersect(Ray) const;
    std::pair<bool, double> intersect(Segment) const;
    std::pair<bool, double> intersect(HalfPlane) const;

    std::pair<double, double> intersect(Triangle) const;
    std::pair<double, double> intersect(Circle) const;
    std::pair<double, double> intersect(CircleSegment) const;

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

    // natural uv coordinates of the given point in (or out) of this triangle
    Point uv(Point const &) const;

    bool contains(Point const &) const;
    // does not count the boundary
    bool contains_exclusive(Point const &) const;
    std::pair<bool, Segment> intersect(Line) const;
    std::pair<bool, Segment> intersect(Ray) const;
    std::pair<bool, Segment> intersect(Segment) const;
    std::pair<bool, CircleSegment> intersect(Circle) const;
    std::pair<bool, CircleSegment> intersect(CircleSegment) const;

    double area() const;

    Point operator()(double, double) const;
    Point const & operator[](size_t i) const;
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
    string to_string(RGBA const & c);
    string to_string(RGB const & c);
}


#endif