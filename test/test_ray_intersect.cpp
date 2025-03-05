
#include "point.hpp"
#include "plotter.hpp"
#include "utility.hpp"

#include "testing.hpp"

#include <iostream>

int main(int ac, char * av[]) 
{
    Point origin(0,0);
    Point p0(1,1), p1(1,-1);

    ASSERT_THROW(segment_intersects_horizontal_ray(p0, p1, origin).first);

    p1 = Point(2,1);
    ASSERT_THROW(!segment_intersects_horizontal_ray(p0, p1, origin).first);

    p1 = Point(1, -1);
    origin = Point(2,0);

    ASSERT_THROW(!segment_intersects_horizontal_ray(p0, p1, origin).first);



    return 0;
}