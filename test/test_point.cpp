#include "point.hpp"

#include "testing.hpp"

using std::pair;

#include <cmath>
#include <iostream>

#define ASSERT_NEAR(A, B)  ASSERT_CLOSE(A, B, 1e-4)

int main(int ac, char * av[])
{
    Point p0(0,0), p1(1,0), p2(1,1);
    Triangle tri(p0, p1, p2);
    Segment seg{p0, p1};

    Segment seg2{p0, p2};

    pair<Point, double> n;
    double t;

    ASSERT_THROW(tri.contains(Point(0.9, 0.2)));
    ASSERT_THROW(!tri.contains(Point(0.2, 0.9)));

    ASSERT_THROW(tri.contains(p0));
    ASSERT_THROW(tri.contains(p1));
    ASSERT_THROW(tri.contains(p2));

    ASSERT_THROW(!tri.contains(p0 + Point(0, 1e-9)));
    ASSERT_THROW(!tri.contains(p0 + Point(0, -1e-9)));
    ASSERT_THROW(!tri.contains(p0 + Point(-1e-9, 0)));
    
    ASSERT_THROW(!tri.contains(p1 + Point(1e-9, 0)));
    ASSERT_THROW(!tri.contains(p1 + Point(0, -1e-9)));

    ASSERT_THROW(!tri.contains(p2 + Point(1e-9, 0)));
    ASSERT_THROW(!tri.contains(p2 + Point(0, 1e-9)));
    ASSERT_THROW(!tri.contains(p2 + Point(-1e-9, 0)));

    t = seg.nearest(p2);

    ASSERT_NEAR(t, 1);
    ASSERT_NEAR(seg.nearest(p1), 1);
    ASSERT_NEAR(seg.nearest(Point{0.5, 1}), 0.5);

    ASSERT_NEAR(seg2.nearest(p1), 0.5);
    

    return 0;
}