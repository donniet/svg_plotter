#include "bezier.hpp"


Point bezier_point(double t, Point const & p0, Point const & p1)
{
    return p0 + (p1 - p0) * t;
}

Vector bezier_direction(double t, Point const & p0, Point const & p1)
{
    return p1 - p0;
}

Point bezier_point(double t, Point const & p0, Point const & p1, Point const & p2)
{
    Point q0 = bezier_point(t, p0, p1);
    Point q1 = bezier_point(t, p1, p2);

    return bezier_point(t, q0, q1);
}

Vector bezier_direction(double t, Point const & p0, Point const & p1, Point const & p2)
{
    Point q0 = bezier_point(t, p0, p1);
    Point q1 = bezier_point(t, p1, p2);

    return (q1 - q0) + t * (bezier_direction(t, p0, p1) + bezier_direction(t, p1, p2));
}

Point bezier_point(double t, Point const & p0, Point const & p1, Point const & p2, Point const & p3)
{
    Point q0 = bezier_point(t, p0, p1, p2);
    Point q1 = bezier_point(t, p1, p2, p3);

    return bezier_point(t, q0, q1);
}

Vector bezier_direction(double t, Point const & p0, Point const & p1, Point const & p2, Point const & p3)
{
    Point q0 = bezier_point(t, p0, p1, p2);
    Point q1 = bezier_point(t, p1, p2, p3);

    return (q1 - q0) + t * (bezier_direction(t, p0, p1, p2) + bezier_direction(t, p1, p2, p3));
}
