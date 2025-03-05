#ifndef __BEZIER_HPP__
#define __BEZIER_HPP__

#include "point.hpp"


Point bezier_point(double t, Point const & p0, Point const & p1);
Vector bezier_direction(double t, Point const & p0, Point const & p1);

Point bezier_point(double t, Point const & p0, Point const & p1, Point const & p2);
Vector bezier_direction(double t, Point const & p0, Point const & p1, Point const & p2);

Point bezier_point(double t, Point const & p0, Point const & p1, Point const & p2, Point const & p3);
Vector bezier_direction(double t, Point const & p0, Point const & p1, Point const & p2, Point const & p3);

#endif