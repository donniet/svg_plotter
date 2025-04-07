#include "point.hpp"
#include "utility.hpp"

#include <numeric>
#include <cmath>
#include <string>
#include <format>
#include <utility>
#include <numbers>
#include <charconv>

using std::sqrt, std::clamp, std::min, std::max, std::sin, std::cos;
using std::format;
using std::string;
using std::pair;
using std::swap, std::tie;
using std::numbers::pi;
using std::from_chars;


/**
 * Transform class implementations
 */
Transform Transform::translate(double x)
{
    return Transform(
        1, 0, x,
        0, 1, 0
    );
}
Transform Transform::translate(Point const & p)
{
    return Transform(
        1, 0, p.x,
        0, 1, p.y
    );
}
Transform Transform::rotate(double deg)
{
    double rad = deg * pi / 180.;

    double cosA = cos(rad);
    double sinA = sin(rad);

    return Transform(
        cosA, -sinA, 0,
        sinA,  cosA, 0
    );
}
Transform Transform::rotate(double deg, Point const & p)
{
    return translate(p) * rotate(deg) * translate(-1 * p);
}
Transform Transform::skewX(double k)
{
    return Transform(
        1, 0, k,
        0, 1, 0
    );
}
Transform Transform::skewY(double k)
{
    return Transform(
        1, 0, 0,
        0, 1, k
    );
}
Transform Transform::scale(double s)
{
    return Transform(
        s, 0, 0,
        0, s, 0
    );

}
Transform Transform::scale(Vector const & v)
{
    return Transform(
        v.x,   0, 0,
          0, v.y, 0
    );
}
Transform Transform::matrix(double a, double b, double c, double d, double e, double f)
{
    return Transform(a,b,c,d,e,f);
}
Transform Transform::matrix(double a[6])
{
    return Transform(a[0], a[1], a[2], a[3], a[4], a[5]);
}

// identity
Transform::Transform() :
    mat{1,0,0,0,1,0}
{ }

Transform::Transform(double a, double b, double c, double d, double e, double f) :
    mat{a, b, c, d, e, f}
{ }

Point Transform::operator()(Point const & p) const
{
    return Point{
        mat[0] * p.x + mat[1] * p.y + mat[2],
        mat[3] * p.x + mat[4] * p.y + mat[5]
    };
}   
Transform Transform::operator()(Transform const & t) const
{
    return Transform(
        mat[0] * t.mat[0] + mat[1] * t.mat[3],  mat[0] * t.mat[1] + mat[1] * t.mat[4], mat[0] * t.mat[2] + mat[1] * t.mat[5] + mat[2],
        mat[3] * t.mat[0] + mat[4] * t.mat[3],  mat[3] * t.mat[1] + mat[4] * t.mat[4], mat[3] * t.mat[2] + mat[4] * t.mat[5] + mat[5]
    );
}


Point Transform::operator*(Point const & p) const
{
    return Point{
        mat[0] * p.x + mat[1] * p.y + mat[2],
        mat[3] * p.x + mat[4] * p.y + mat[5]
    };
}   
Transform Transform::operator*(Transform const & t) const
{
    return Transform(
        mat[0] * t.mat[0] + mat[1] * t.mat[3],  mat[0] * t.mat[1] + mat[1] * t.mat[4], mat[0] * t.mat[2] + mat[1] * t.mat[5] + mat[2],
        mat[3] * t.mat[0] + mat[4] * t.mat[3],  mat[3] * t.mat[1] + mat[4] * t.mat[4], mat[3] * t.mat[2] + mat[4] * t.mat[5] + mat[5]
    );
}

/**
 * Point class implementation
 * 
 */

Point::Point(double x0, double y0) : 
    x(x0), y(y0)
{ }

Point::Point() : 
    Point(0, 0)
{ }

Point & Point::operator=(Point const & r)
{
    x = r.x;
    y = r.y;
    return *this;
}
Point & Point::operator=(Point && r)
{
    x = r.x;
    y = r.y;
    return *this;
}

void Point::swap(Point & p1)
{
    ::swap(x, p1.x);
    ::swap(y, p1.y);
}

double Point::norm2() const 
{ 
    return dot(*this); 
}
double Point::norm() const { 
    return sqrt(norm2()); 
}

Point Point::operator-(Point const & p) const 
{ 
    return { x - p.x, y - p.y }; 
}
Point Point::operator+(Point const & p) const 
{ 
    return { x + p.x, y + p.y }; 
}
Point & Point::operator+=(Point const & p) 
{ 
    x += p.x;
    y += p.y;
    return *this;
}
Point & Point::operator-=(Point const & p) 
{ 
    x -= p.x;
    y -= p.y;
    return *this;
}
Point Point::operator*(double a) const 
{ 
    return {a * x, a * y }; 
}
Point Point::operator/(double a) const 
{ 
    return { x / a, y / a }; 
}
Point & Point::operator/=(double a)
{
    x /= a;
    y /= a;
    return *this;
}

double Point::dot(Point p) const 
{ 
    return x * p.x + y * p.y; 
}


double Point::cross(Point const & p) const
{
    return x * p.y - y * p.x;
}

Point Point::normalized() const
{
    return *this / norm();
}
bool Point::is_parallel(Point v, double epsilon) const
{
    v = v.normalized();
    Point u = normalized();

    if(1.0 - abs(v.dot(u)) < epsilon)
        return true;
    
    return false;
}

Point Point::reflect(Point o)
{
    Point v = o - *this;
    return o + v;
}
double Point::angle(Point v) const {
    double s = 1;
    if(x * v.y - y * v.x < 0)
        s = -1;

    return s * acos(dot(v) / norm() / v.norm());
}
Point Point::rotate(double a) const
{
    double sinu = sin(a);
    double cosu = cos(a);

    return Point{
        dot(Point{cosu, sinu}),
        dot(Point{-sinu, cosu})
    };
}

// treat like complex numbers and multiply
Point Point::rotate(Point v) const
{
    // x + i y
    // w + i u
    // ( x w - y u ) + i ( x u + y w )
    return Point{
        x * v.x - y * v.y, 
        x * v.y + y * v.x
    };
}
Point Point::rotate_about(Point about, double angle) const
{
    Point d = *this - about;

    d = d.rotate(angle);

    return about + d;
}

bool Point::operator==(Point const & p) const 
{
    return x == p.x && y == p.y;
}
bool Point::operator!=(Point const & p) const 
{
    return !operator==(p);
}

void swap(Point & p0, Point & p1)
{
    swap(p0.x, p1.x);
    swap(p0.y, p1.y);
}


/**
 * Path class
 */
Path::Path(vector<Point> const & p, bool closed) :
    vector<Point>{p}, _closed{closed}
{ }

Path::Path(vector<Point> && p, bool closed) :
    vector<Point>{move(p)}, _closed{closed}
{ }

bool Path::closed() const
{
    return _closed;
}

// Path Path::offset(double d) const
// {
//     Path const & p = *this;

//     if(size() < 2)
//         return p;

//     size_t N = size();
//     vector<Vector> normals(N);
//     vector<Vector> tangents(N);
//     Path r = p;

//     for(size_t i = 0; i < N - 1; i++)
//     {
//         Vector t = (p[i+1] - p[i]).normalized();
//         normals[i] = normal(t);
//         tangents[i] = t;
//     }
    
//     // check for intersections
//     for(size_t i = 0; i < N - 1; i++)
//     {
//         auto l0 = Line{p[i], d * normals[i]};
//         auto l1 = Line{p[i+1], d * normals[i+1]};

//         auto a = l0.intersect(l1);

//         if(a.first && a.second > 0 && a.second < 1.)
//         {

//         }
//     }
        
    

//     for(size_t i = 0; i < size(); i++)
//     {
//         Vector n;

//         if(i == 0)
//             n = d * normal(p[i+1] - p[i]).normalized();
//         else if(i == size() - 1)
//             n = d * normal(p[i] - p[i-1]).normalized();
//         else
//         {
//             auto n0 = normal(p[i] - p[i-1]).normalized();
//             auto n1 = normal(p[i+1] - p[i]).normalized();

//             n = d * (n0 + n1).normalized();
//         }

//         r[i] += n;
//     }

//     return r;
// }

Vector normalize(Vector const & v)
{
    return v.normalized();
}
Vector normal(Vector const & v)
{
    return Vector(-v.y, v.x);
}
double dot(Vector const & u, Vector const & v)
{
    return u.dot(v);
}
double cross(Vector const & u, Vector const & v)
{
    return u.cross(v);
}

/**
 * RGB class implementation
 */
RGB::RGB() : r(0), g(0), b(0) { }

RGB::RGB(double r0, double g0, double b0) :
    r(r0), g(g0), b(b0)
{ }

void RGB::swap(RGB & rgb)
{
    ::swap(r, rgb.r);
    ::swap(g, rgb.g);
    ::swap(b, rgb.b);
}

/**
 * RGBA class implementation
 */
RGBA::RGBA() : RGB(), a(0) { }

RGBA::RGBA(double r0, double g0, double b0, double a0) :
    RGB(r0, g0, b0), a(a0)
{ }

RGBA::RGBA(RGB const & rgb, double a0) :
    RGB(rgb), a(a0)
{ }

void RGBA::swap(RGBA & rgb)
{
    RGB::swap(rgb);
    ::swap(a, rgb.a);
}

RGBA RGBA::from_hex(string const & hex)
{
    string h = hex;
    trim(h);

    if(h[0] == '#')
        h.erase(0, 1);

    int r, g, b, d;
    
    switch(h.size())
    {
    case 3:
        from_chars(&h[0], &h[1], r, 0x10);
        from_chars(&h[1], &h[2], g, 0x10);
        from_chars(&h[2], &h[3], b, 0x10);
        d = 16;
        break;
    case 6:
        from_chars(&h[0], &h[2], r, 0x10);
        from_chars(&h[2], &h[4], g, 0x10);
        from_chars(&h[4], &h[6], b, 0x10);
        d = 256;
        break;
    default:
        return RGBA{};
    }

    return RGBA{(double)r/(double)d, (double)g/(double)d, (double)b/(double)d, 1.};
}

/**
 * Line class implementation
 * 
 */

Line::Line(Point const & p0, Vector const & v0) :
    p(p0), v(v0)
{ }

void Line::swap(Line & l1)
{
    p.swap(l1.p);
    v.swap(l1.v);
}

Point Line::operator()(double t) const
{
    return p + t * v;
}

double Line::nearest(Point q) const
{
    if(v.x == 0 && v.y == 0)
        return 0.;

    Point d = q - p;
    double lv = v.norm();

    return dot(d, v) / lv / lv;
}
double Line::distance(Point q) const
{
    double t = nearest(q);

    return ((*this)(t) - q).norm();
}

pair<bool, double> Line::intersect(Line l1) const
{
    /*
        f(t) = p + t * v
        g(t) = q + u * w

        p - q = ( v , w ) ( t ; -u )

        p - q = ( v.x  -w.x   ( t 
                  v.y  -w.y )   u )
              
        |A| = -v.x * w.y + w.x * v.y

        A^{-1} = ( -w.y  w.x   
                   -v.y  v.x ) / |A|

        ( t   = A^{-1} ( p.x - q.x 
          u )            p.y - q.y )

     */

    double det = -v.x * l1.v.y + l1.v.x * v.y;
    if (abs(det) < 1e-9)
    {
        // Check for coincidence:
        double cross_product = (p.x - l1.p.x) * v.y - (p.y - l1.p.y) * v.x;
        if (std::abs(cross_product) < 1e-9)
            // Lines are coincident (overlapping)
            return { true, 0.0 }; // Indicate intersection (coincidence)
       
        // Lines are parallel (no intersection)
        return { false, 0.0 };
    }
    
    double t = ( (p.x - l1.p.x) * l1.v.y - (p.y - l1.p.y) * l1.v.x) / det;

    return { true, t };
}

pair<bool, double> Line::intersect(Ray r) const
{
    // treat the ray as a line and intersect it, then check the intersection parameter
    Line lr{r.p, r.v};

    auto i = lr.intersect(*this);

    if(i.first && i.second >= 0)
        return intersect(lr);
    
    return { false, 0. };
        
}

pair<bool, double> Line::intersect(Segment s) const
{
    Line l(s.p0, s.p1 - s.p0);

    auto res = l.intersect(*this);

    if(!res.first || res.second < 0. || res.second > 1.)
        return { false, 0. };

    Point p1 = l(res.second);
    if(v.x == 0)
        return { true, (p1.y - p.y) / v.y };

    return { true, (p1.x - p.x) / v.x };
}

pair<double,double> Line::intersect(Triangle t) const
{
    double t0 = 1, t1 = 0;

    double t01, t12, t20;
    bool i01, i12, i20;

    tie(i01, t01) = intersect(Segment(t.p0, t.p1));
    tie(i12, t12) = intersect(Segment(t.p1, t.p2));
    tie(i20, t20) = intersect(Segment(t.p2, t.p0));

    if(!i01 && !i12 && !i20)
        return {t0, t1}; // empty range

    if(i01)
        t0 = t1 = t01;
    
    if(i12) 
    {
        t0 = min(t0, t12);
        t1 = max(t1, t12);
    }

    if(i20)
    {
        t0 = min(t0, t20);
        t1 = max(t0, t20);
    }

    return {t0, t1};
}

pair<double, double> Line::intersect(Circle c) const
{
    // p + t * v == o + r.rotate(u)
    // p.x + t * v.x == o.x + r.x * cos(u) + r.y * sin(u);
    // p.y + t * v.y == o.y - r.x * sin(u) + r.y * cos(u);
    double radius2 = c.r.norm2();
    if(radius2 == 0)
        return {1., 0.}; // no solution

    // Vector from line's starting point to circle's center
    Vector w = {c.o.x - p.x, c.o.y - p.y};

    // Coefficients of the quadratic equation
    double a = v.x * v.x + v.y * v.y;
    if(a < 1e-18)
        return {1., 0.}; // no solution

    double b = 2 * (v.x * w.x + v.y * w.y);
    double cc = w.x * w.x + w.y * w.y - radius2; // 'cc' to avoid naming conflict with 'c'

    double discriminant = b * b - 4 * a * cc;

    if (discriminant < 0)         // No intersection
        return {1., 0.};
    else if (discriminant < 1e-9) // Tangent (one intersection point)
    {
        double t = -b / (2 * a);
        return {t, t};
    } 

    double t1 = (-b - sqrt(discriminant)) / (2 * a);
    double t2 = (-b + sqrt(discriminant)) / (2 * a);

    // Return the smaller t value (closer intersection point)
    return {min(t1, t2), max(t1, t2)};
}

pair<double, double> Line::intersect(CircleSegment cs) const
{
    double t0, t1;
    tie(t0, t1) = intersect(Circle{cs.o, cs.r});

    if(t0 > t1)
        return {1., 0.}; // no solution

    // start with t0
    Point p0 = (*this)(t0);

    // what is the angle from R to OP?
    double a0 = cs.r.angle(p0 - cs.o);      // between -π and π

    // change to be an angle from 0 to 2π
    if(a0 < 0)
        a0 += 2. * pi;

    if(t1 == t0) 
    {
        if(a0 > cs.a)
            return {1., 0.};
        
        return {t0, t0};
    }

    // now check t1
    Point p1 = (*this)(t1);
    double a1 = cs.r.angle(p1 - cs.o);
    if(a1 < 0)
        a1 += 2. * pi;

    if(a0 > cs.a && a1 > cs.a)
        return {1., 0.};

    if(a1 > cs.a)
        return {t0, t0};
    
    if(a0 > cs.a)
        return {t1, t1};

    return {t0, t1};
}

bool Line::operator==(Line const & l) const 
{
    return p == l.p && v == l.v;
}

/** 
 *  Ray class implementation
 */

Ray::Ray(Point const & p0, Vector const & v0) :
    p(p0), v(v0)
{ }

Point Ray::operator()(double t) const
{
    return p + t * v;
}
void Ray::swap(Ray & r)
{
    ::swap(p, r.p);
    ::swap(v, r.v);
}

/// UNIMPLEMENTD
// double Ray::nearest(Point p) const { }
// double Ray::distance(Point p) const { }

pair<bool, double> Ray::intersect(Line l) const 
{
    Line rl{p, v};
    auto i = rl.intersect(l);

    if(i.first && i.second >= 0.)
        return i;

    return { false, i.second };
}
pair<bool, double> Ray::intersect(Ray r) const 
{
    Line l{p, v};
    Line rl{r.p, r.v};

    auto i = intersect(rl);
    if(!i.first)
        return i;
    
    auto j = rl.intersect(*this);
    if(!j.first)
        return j;

    return i;
}
pair<bool, double> Ray::intersect(Segment s) const 
{
    Line l{p, v};

    auto i = l.intersect(s);
    if(i.first && i.second >= 0)
        return i;

    return { false, i.second };
}

/// UNIMPLEMENTED
// pair<bool, double> Ray::intersect(HalfPlane) const {}
// pair<double, double> Ray::intersect(Triangle) const {}
// pair<double, double> Ray::intersect(Circle) const {}
// pair<double, double> Ray::intersect(CircleSegment) const {} 

bool HalfPlane::contains(Point const & q) const
{
    Vector d = q - p;

    return dot(d, n) >= 0;
}
    
/**
 * Segment class implementation
 */

Segment::Segment(Point const & q0, Point const & q1) :
    p0(q0), p1(q1)
{ }

double Segment::nearest(Point q) const
{
    double t = Line{p0, p1 - p0}.nearest(q);

    return min(max(t, 0.), 1.);
}

double Segment::length() const
{
    return (p1 - p0).norm();
}

double Segment::distance(Point q) const
{
    double t = Line{p0, p1 - p0}.nearest(q);

    if(t <= 0)
        return (p0 - q).norm();

    if(t >= 1)
        return (p1 - q).norm();

    return ((*this)(t) - q).norm();
}

pair<bool, double> Segment::intersect(Line l) const
{
    Line l0(p0, p1 - p0);

    auto res = l0.intersect(l);

    if(!res.first || res.second < 0. || res.second > 1.)
        return { false, 0. };

    return res;
}
pair<bool, double> Segment::intersect(Ray r) const
{
    Line l0(p0, p1 - p0);
    Line lr(r.p, r.v);

    auto res = l0.intersect(lr);

    if(!res.first || res.second < 0. || res.second > 1.)
        return { false, res.second };

    auto res2 = lr.intersect(*this);
    if(!res2.first || res2.second < 0)
        return { false, res.second };

    return res;
}
pair<bool, double> Segment::intersect(Segment s) const
{
    Line l0(p0, p1 - p0);
    Line l1(s.p0, s.p1 - s.p0);

    auto res = l1.intersect(l0);
    if(!res.first || res.second < 0. || res.second > 1.)
        return { false, 0. };

    Point pi = l1(res.second);
    double t0;

    if(l0.v.x == 0)
        t0 = (pi.y - l0.p.y) / l0.v.y;
    else
        t0 = (pi.x - l0.p.x) / l0.v.x;

    if(t0 < 0. || t0 > 1.)
        return { false, 0. };
    
    return { true, t0 };
}
pair<double,double> Segment::intersect(Triangle tri) const
{
    double t0 = 1, t1 = 0;

    double u0, u1;

    tie(u0, u1) = Line{p0, p1 - p0}.intersect(tri);

    if(u0 <= 1. && u1 >= 0.) 
    {
        t0 = min(t0, max(u0, 0.));
        t1 = max(t1, min(u1, 1.));
    }

    return {t0, t1};
}
Line Segment::line() const
{
    return Line{p0, p1 - p0};
}

Point Segment::operator()(double t) const
{
    t = clamp(t, 0., 1.);
    return p0 + t * (p1 - p0);
}
void Segment::swap(Segment & s)
{
    p0.swap(s.p0);
    p1.swap(s.p1);
}

Triangle::Triangle(Point const & q0, Point const & q1, Point const & q2) :
    p0(q0), p1(q1), p2(q2)
{ }

bool Triangle::contains(Point const & p) const
{
    // Calculate vectors
    Point v0 = p2 - p0;
    Point v1 = p1 - p0;
    Point v2 = p - p0;

    // Calculate dot products
    double dot00 = dot(v0, v0);
    double dot01 = dot(v0, v1);
    double dot02 = dot(v0, v2);
    double dot11 = dot(v1, v1);
    double dot12 = dot(v1, v2);

    // Calculate barycentric coordinates
    double invDenom = 1. / (dot00 * dot11 - dot01 * dot01);
    double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

pair<bool, Segment> Triangle::intersect(Line l) const 
{
    double t0, t1;
    tie(t0, t1) = l.intersect(*this);

    if(t0 > t1)
        return {false, Segment{}};

    return {true, Segment(l(t0), l(t1))};
}

pair<bool, Segment> Triangle::intersect(Segment s) const 
{
    Segment r;
    double t0, t1;
    tie(t0, t1) = s.intersect(*this);

    t0 = max(0., t0);
    t1 = min(1., t1);

    if(t0 > t1)
        return {false, Segment{}};
    
    return {true, Segment(s(t0), s(t1))};
    
}


Circle::Circle(Point origin, Vector radius) :
    o(origin), r(radius)
{ }

// goes from 0 to 1
Point Circle::operator()(double t) const
{
    t *= 2. * pi;

    return o + r.rotate(t);
}

CircleSegment::CircleSegment() :
    o{}, r{}, a(0)
{ }

CircleSegment::CircleSegment(Point origin, Vector radius, double angle) :
    o{origin}, r{radius}, a{angle}
{ }

Point CircleSegment::operator()(double t) const
{
    double u = t * a;

    return o + r.rotate(u);
}


Event::Event(double x0, double y0, double dx0, double dy0, double dt0) :
    Point(x0, y0), dp(dx0, dy0), dt(dt0)
{ }
Event::Event(Point p0, double dx0, double dy0, double dt0) :
    Point(p0), dp(dx0, dy0), dt(dt0)
{ }
Event::Event(Point p0, Vector v0, double dt0) :
    Point(p0), dp(v0), dt(dt0)
{ }
Event::Event() : 
    Point(), dp(0,0), dt(0)
{ }

Vector Event::dp_dt() const
{
    if(is_move_to())
        return Vector(0,0);

    return dp / dt;
}

bool Event::is_move_to() const 
{ 
    return dt <= 0.; // a time derivative of zero or less isn't realistic and represents a move
}

void make_move_to(Event & e)
{
    e.dt = 0;
}


Point operator*(double a, Point const & p) { return p * a; }
double abs(Point p) { return p.norm(); }


namespace std {
    string to_string(Point const & p)
    {
        return format("({},{})", p.x, p.y);
    }
    string to_string(Event const & p)
    {
        if(p.is_move_to())
            return format("move({},{})", p.x, p.y);

        return format("draw({},{},{})", p.x, p.y, p.dt);
    }

    string to_string(RGBA const & c)
    {
        return format("RGBA{{ .r={},.g={},.b={},.a={} }}", c.r, c.g, c.b, c.a);
    }
    string to_string(RGB const & c)
    {
        return format("RGB{{ .r={},.g={},.b={} }}", c.r, c.g, c.b);
    }
}


