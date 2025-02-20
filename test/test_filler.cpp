
#include "plotter.hpp"
#include "parser.hpp"

#include <iostream>
#include "testing.hpp"

#include <numbers>


using namespace std;
using namespace numbers;


int main(int ac, char * av[]) 
{
    SVGPath circle("M 20 20 A 10 10 0 0 0 40 20 A 10 10 0 0 0 20 20 Z");
    Shape outline = plot(circle, {
        .draw_speed = circle.length(),
        .maximum_points = 100,
    });


    cout << "x\ty\tt\n";
    for(Event const & e : outline)
        cout << e.x << "\t" << e.y << "\t" << e.t << "\n";



    StrokeFiller filler;
    Spiral pattern(BoundingBox(Point(20,10), Point(40,30)), Point(30,20), 5. / 2 / pi);

    auto strokes = filler.fill(outline, pattern, 100);

    cout << "x\ty\tt\n";
    for(Stroke const & s : strokes) {
        // cout << "stroke: \n";
        for(Event const & e : s)
            cout << e.x << "\t" << e.y << "\t" << e.t << "\n";
    }


    cout << "\njson:\n";
    cout << "{ \"boundary\": [\n";
    for(Event const & e : outline)
        cout << "{ \"x\":" << e.x << ", \"y\":" << e.y << ", \"t\": " << e.t << "},\n";
    cout << "\n], \"fill\": [\n";
    for(Stroke const & s : strokes) {
        cout << "{ \"stroke\": [\n";
        for(Event const & e : s)
            cout << "{ \"x\":" << e.x << ", \"y\":" << e.y << ", \"t\": " << e.t << "},\n";
        cout << "]}\n";
    }
    cout << "]}";
        

    return 0;
}