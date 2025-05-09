#include "svg_path.hpp"

#include "point.hpp"

#include <iostream>
#include <algorithm>
#include <array>

using std::cout, std::cerr, std::endl;
using std::for_each;

struct Outputer : public PathVisitor
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
    Outputer visitor;
    Plotter<Point> plotter;

    PathParser parser(plotter);


    // parser.parse("m 10 20, 20 10 C 40 30 40 40 30 40 Z");
    // parser.parse(R"(M 10,30
    //    A 20,20 0,0,1 50,30
    //    A 20,20 0,0,1 90,30
    //    Q 90,60 50,90
    //    Q 10,60 10,30 z)");

    parser.parse(R"(
        m 262.325,478.45 0.294,3.554 0.862,3.451 1.403,3.331 1.913,3.194 2.397,3.038 2.85,2.866 3.276,2.676 3.673,2.468 4.041,2.243 4.381,2.001 4.692,1.74 4.975,1.464 5.228,1.169 5.454,0.856 5.651,0.527 5.818,0.179 
        h 2.526 
        l 5.819,-0.179 5.65,-0.527 5.454,-0.856 5.229,-1.169 4.974,-1.464 4.692,-1.74 4.381,-2.001 4.041,-2.243 3.673,-2.468 3.276,-2.676 2.85,-2.866 2.396,-3.038 1.914,-3.194 1.402,-3.331 0.862,-3.451 0.294,-3.554 -0.091,-2.179 -0.267,-2.165 -0.432,-2.143 -0.583,-2.111 -1.582,-4.097 -2.057,-3.873 
        H 267.339 
        l -2.058,3.873 -1.582,4.097 -0.584,2.111 -0.432,2.143 -0.267,2.165 -0.091,2.179 
        v 0)");

    auto plot = plotter.plot();

    cout << "x, y" << endl;
    for_each(plot.begin(), plot.end(), [](auto const & path) {
        for_each(path.begin(), path.end(), [](auto const & p) {
            cout << p.x << ", " << p.y << endl;
        });
    });
    

    return 0;
}