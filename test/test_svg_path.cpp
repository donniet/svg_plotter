#include "svg_path.hpp"

#include "point.hpp"
#include "cover.hpp"
#include "output/stl.hpp"

#include <iostream>
#include <algorithm>
#include <array>

using std::cout, std::cerr, std::endl;
using std::for_each;

struct OutputVisitor : public PathVisitor
{
    virtual void begin(double x, double y) { 
        cout << "begin_path(" << x << " " << y << ");\n"; 
    }
    virtual void move(double x0, double y0, double x, double y) { 
        cout << "move(" << x << " " << y << ");\n";
    }
    virtual void line(double, double, double x, double y) {
        cout << "line(" << x << " " << y << ");\n";
    }
    virtual void arc(double, double, double rx, double ry, double a, bool large, bool sweep, double x, double y) {
        cout << "arc(" << rx << " " << ry << " " << a << " " << large << " " << sweep << " " << x << " " << y << ");\n";
    }
    virtual void bezier(double, double, double x1, double y1, double x2, double y2, double x, double y) {
        cout << "bezier(" << x1 << " " << y1 << " " << x2 << " " << y2 << " " << x << " " << y << ");\n";
    }
    virtual void quadratic(double, double, double x1, double y1, double x, double y) {
        cout << "quadratic(" << x1 << " " << y1 << " " << x << " " << y << ");\n";
    }
    virtual void end() {
        cout << "end_path();\n" << endl;
    }
};

int main(int ac, char * av[])
{
    OutputVisitor visitor;
    Plotter<Point> plotter;

    PathParser parser(plotter);


    // parser.parse("m 10 20, 20 10 C 40 30 40 40 30 40 Z");
    // parser.parse(R"(M 10,30
    //    A 20,20 0,0,1 50,30
    //    A 20,20 0,0,1 90,30
    //    Q 90,60 50,90
    //    Q 10,60 10,30 z)");

    string anis_bottom_glass = R"(
        m 262.325,478.45 0.294,3.554 0.862,3.451 1.403,3.331 1.913,3.194 2.397,3.038 2.85,2.866 3.276,2.676 3.673,2.468 4.041,2.243 4.381,2.001 4.692,1.74 4.975,1.464 5.228,1.169 5.454,0.856 5.651,0.527 5.818,0.179 
        h 2.526 
        l 5.819,-0.179 5.65,-0.527 5.454,-0.856 5.229,-1.169 4.974,-1.464 4.692,-1.74 4.381,-2.001 4.041,-2.243 3.673,-2.468 3.276,-2.676 2.85,-2.866 2.396,-3.038 1.914,-3.194 1.402,-3.331 0.862,-3.451 0.294,-3.554 -0.091,-2.179 -0.267,-2.165 -0.432,-2.143 -0.583,-2.111 -1.582,-4.097 -2.057,-3.873 
        H 267.339 
        l -2.058,3.873 -1.582,4.097 -0.584,2.111 -0.432,2.143 -0.267,2.165 -0.091,2.179 
        v 0)";
    string anis_top_glass = "m 355.535,435.764 c -3.689,-5.272 -7.359,-10.555 -11.036,-15.834 h -48.005 c -3.676,5.279 -7.346,10.562 -11.036,15.834 -5.44,7.772 -10.721,15.672 -16.298,23.347 -0.644,0.888 -1.246,1.819 -1.821,2.771 h 106.315 c -0.576,-0.952 -1.178,-1.884 -1.822,-2.771 -5.577,-7.675 -10.856,-15.575 -16.297,-23.347 z";
    string anis_glass_highlight = "m 284.82,474.084 c 0,-6.084 2.163,-12.11 5.278,-16.982 4.31,-6.732 8.385,-13.666 12.587,-20.488 1.963,-3.189 3.917,-6.39 5.874,-9.585 l -8.629,-0.02 c -2.842,4.186 -5.674,8.377 -8.524,12.562 -4.199,6.168 -8.275,12.434 -12.581,18.52 -3.118,4.402 -5.28,9.849 -5.28,15.347 0,12.633 13.546,23.253 32.011,26.517 -12.433,-5.391 -20.736,-14.946 -20.736,-25.871 z";

    parser.parse(anis_glass_highlight);

    auto plot = plotter.plot();

    // cout << "x, y" << endl;
    // for_each(plot.begin(), plot.end(), [](auto const & path) {
    //     for_each(path.begin(), path.end(), [](auto const & p) {
    //         cout << p.x << ", " << p.y << endl;
    //     });
    // });
    
    // auto triangles = mapbox::earcut(plot);

    MeshCover c(plot.paths());

    // for(Triangle const & c : c.triangles())
    // {
    //     cout << c.p0.x << ", " << c.p0.y << ",\n"
    //          << c.p1.x << ", " << c.p1.y << ",\n"
    //          << c.p2.x << ", " << c.p2.y << ",\n";
    // }

    cout << STLOutput(c.triangles()) << endl;


    return 0;
}