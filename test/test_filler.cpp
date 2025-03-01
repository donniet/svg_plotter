
#include "plotter.hpp"
#include "parser.hpp"
#include "fills.hpp"
#include "bounding_box.hpp"

#include <iostream>
#include "testing.hpp"
#include "output/csv.hpp"

#include <numbers>


using namespace std;
using namespace numbers;


int main(int ac, char * av[]) 
{
    Plotter plotter;


    BoundingBox bbox(0, 0, 10, 10);
    bbox.margin() = -1e-2;
    // Spiral pattern(BoundingBox(Point(20,10), Point(40,30)), Point(30,20), 5. / 2 / pi);
    Lines pattern(bbox, 1., Vector(1,0));
    auto strokes = plotter.fill(bbox, pattern);

    cout << "x\ty\n";
    for(Event const & e : strokes) {
        cout << e.x << "\t" << e.y << "\n";
    }



    SVGPath circle("M 20 20 A 10 10 0 0 0 40 20 A 10 10 0 0 0 20 20 Z");
    strokes = plotter.plot(circle);

    // cout << OutputCSV(outline.outline());


    cout << "x\ty\n";
    for(Event const & e : strokes) {
        cout << e.x << "\t" << e.y << "\n";
    }


    // cout << "\njson:\n";
    // cout << "{ \"boundary\": [\n";
    // for(Event const & e : strokes)
    //     cout << "{ \"x\":" << e.x << ", \"y\":" << e.y << ", \"t\": " << e.t << "},\n";
    // cout << "\n], \"fill\": [\n";
    // for(Stroke const & s : strokes) {
    //     cout << "{ \"stroke\": [\n";
    //     for(Event const & e : s)
    //         cout << "{ \"x\":" << e.x << ", \"y\":" << e.y << ", \"t\": " << e.t << "},\n";
    //     cout << "]}\n";
    // }
    // cout << "]}";
        

    return 0;
}