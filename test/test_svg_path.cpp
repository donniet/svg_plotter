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
    parser.parse(R"(M 10,30
       A 20,20 0,0,1 50,30
       A 20,20 0,0,1 90,30
       Q 90,60 50,90
       Q 10,60 10,30 z)");

    auto plot = plotter.plot();

    cout << "x, y" << endl;
    for_each(plot.begin(), plot.end(), [](auto const & path) {
        for_each(path.begin(), path.end(), [](auto const & p) {
            cout << p.x << ", " << p.y << endl;
        });
    });
    

    return 0;
}