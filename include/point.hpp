#ifndef __POINT_HPP__
#define __POINT_HPP__

#include <string>

using std::string;

struct Point
{
    double x, y;

    Point(double x0, double y0);
    Point();
    Point(Point const & r) = default;
    Point(Point && r) = default;
    
    Point & operator=(Point const & r);
    Point & operator=(Point && r);

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

    bool operator==(Point const & p) const;
    bool operator!=(Point const & p) const;
};

using Vector = Point;


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