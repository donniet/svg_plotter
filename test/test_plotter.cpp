#include "plotter.hpp"
#include "testing.hpp"

#include <iostream>

using namespace std;

int main(int ac, char * av[])
{
    SVGPath p("M 10 10 L 20 10 A 10 10 0 0 0 20 30 L 10 30 A 10 10 0 0 0 10 10 Z");

    Shape pill = plot(ConstantArcLengthAdapter(p));

    cout << "x\ty\n";
    for(Event const & p : pill) {
        cout << p.x << "\t" << p.y << "\t" << p.t << "\n";
    }
    cout << endl;
}