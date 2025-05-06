#include "svg_path.hpp"


#include <iostream>

using std::cout, std::cerr, std::endl;

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

    PathParser parser(visitor);


    parser.parse("M 10 20, 20 10 c 40 30 40 40 30 40 Z");


    return 0;
}