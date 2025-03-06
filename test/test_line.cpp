
#include "point.hpp"
#include "testing.hpp"


#include <iostream>
#include <format>

using std::cout, std::endl;
using std::vformat;


int main(int ac, char * av[])
{
    Line line1 = {{-1, 0}, {1, 0}};
    Line line2 = {{0, 1}, {0, 1}};

    std::pair<bool, double> result = line1.intersect(line2);

    ASSERT_THROW(result.first);
    ASSERT_CLOSE(result.second, 1, 1e-9);

    result = line2.intersect(line1);

    ASSERT_THROW(result.first);
    ASSERT_CLOSE(result.second, -1, 1e-9);

    Line line3 = {{-1, 1}, {1, 0}};
    Line line4 = {{-2, 0}, {1, 0}};

    result = line1.intersect(line3);

    ASSERT_THROW(!result.first);

    result = line1.intersect(line4);

    ASSERT_THROW(result.first);
    ASSERT_THROW(result.second == 0.);

    double t = line1.nearest(Point{0,1});

    ASSERT_CLOSE(t, 1, 1e-9);

    Segment s{Point{0,0},Point{1,0}};

    ASSERT_CLOSE(s.distance(Point{-1,0}), 1, 1e-9);
    ASSERT_CLOSE(s.distance(Point{0.,-1}), 1, 1e-9);
    ASSERT_CLOSE(s.distance(Point{0.,1}), 1, 1e-9);
    ASSERT_CLOSE(s.distance(Point{0.5,1}), 1, 1e-9);
    ASSERT_CLOSE(s.distance(Point{0.5,-1}), 1, 1e-9);
    ASSERT_CLOSE(s.distance(Point{1,1}), 1, 1e-9);
    ASSERT_CLOSE(s.distance(Point{1,-1}), 1, 1e-9);
    ASSERT_CLOSE(s.distance(Point{2,0}), 1, 1e-9);


    return 0;
}