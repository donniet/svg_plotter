#include "point.hpp"

#include "testing.hpp"


#include <iostream>

int main(int ac, char * av[])
{
    Point p0(0,0), p1(1,0), p2(1,1);
    Triangle t(p0, p1, p2);

    ASSERT_THROW(t.contains(Point(0.9, 0.2)));
    ASSERT_THROW(!t.contains(Point(0.2, 0.9)));

    ASSERT_THROW(t.contains(p0));
    ASSERT_THROW(t.contains(p1));
    ASSERT_THROW(t.contains(p2));

    ASSERT_THROW(!t.contains(p0 + Point(0, 1e-9)));
    ASSERT_THROW(!t.contains(p0 + Point(0, -1e-9)));
    ASSERT_THROW(!t.contains(p0 + Point(-1e-9, 0)));
    
    ASSERT_THROW(!t.contains(p1 + Point(1e-9, 0)));
    ASSERT_THROW(!t.contains(p1 + Point(0, -1e-9)));

    ASSERT_THROW(!t.contains(p2 + Point(1e-9, 0)));
    ASSERT_THROW(!t.contains(p2 + Point(0, 1e-9)));
    ASSERT_THROW(!t.contains(p2 + Point(-1e-9, 0)));

    return 0;
}