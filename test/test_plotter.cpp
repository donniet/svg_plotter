#include "plotter.hpp"
#include "parser.hpp"
#include "testing.hpp"
#include "adapters.hpp"

#include <iostream>
#include <numbers>

using namespace std;
using namespace std::numbers;


int main(int ac, char * av[])
{
    SVGPath p("M 10 10 L 20 10 A 10 10 0 0 0 20 30 L 10 30 A 10 10 0 0 0 10 10 Z");

    Plotter plotter;

    Stroke pill = plotter.plot(ConstantArcLengthAdapter(p, 100000));

    double pill_area = pill.area();

    cout << "area: " << pill_area << endl;

    ASSERT_CLOSE((pi * 10 * 10 + 10 * 20) / pill_area, 1., 1e-3);
    


    // cout << "x\ty\n";
    // for(Event const & p : pill) {
    //     cout << p.x << "\t" << p.y << "\t" << p.t << "\n";
    // }
    // cout << endl;
}