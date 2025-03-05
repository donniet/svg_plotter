#include "point.hpp"

#include <numeric>
#include <cmath>
#include <string>
#include <format>

using std::sqrt;
using std::format;
using std::string;

Point::Point(double x0, double y0) : 
    x(x0), y(y0) 
{ }

Point::Point() : 
    x(0), y(0) 
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

double Point::norm2() const { return dot(*this); }
double Point::norm() const { return sqrt(norm2()); }

Point Point::operator-(Point const & p) const { return { x - p.x, y - p.y }; }
Point Point::operator+(Point const & p) const { return { x + p.x, y + p.y }; }
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
Point Point::operator*(double a) const { return {a * x, a * y }; }
Point Point::operator/(double a) const { return { x / a, y / a }; }
Point & Point::operator/=(double a)
{
    x /= a;
    y /= a;
    return *this;
}

double Point::dot(Point p) const { return x * p.x + y * p.y; }


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

bool Point::operator==(Point const & p) const 
{
    return x == p.x && y == p.y;
}
bool Point::operator!=(Point const & p) const 
{
    return !operator==(p);
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
}


