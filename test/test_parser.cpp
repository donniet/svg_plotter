#include "parser.hpp"
#include <iostream>
#include <string>

#include "testing.hpp"

using namespace std;

int main(int ac, char * av[]) 
{
    string path_data = "";
    SVGPath p(path_data);

    cout << "testing length..." << endl;
    cout << "empty path: " << p.length() << endl;
    ASSERT_CLOSE(p.length(), 0., 1e-5);

    p = SVGPath("M 10 10");
    cout << "move path: " << p.length() << endl;
    ASSERT_CLOSE(p.length(), 0, 1e-5);

    p = SVGPath("L 10 10");
    cout << "line path (10,10): " << p.length() << endl;
    ASSERT_CLOSE(p.length(), sqrt(200), 1e-5);

    p = SVGPath("H 10");
    cout << "horizontal 10: " << p.length() << endl;
    ASSERT_CLOSE(p.length(), 10, 1e-5);
    
    p = SVGPath("V 10");
    cout << "vertical 10: " << p.length() << endl;
    ASSERT_CLOSE(p.length(), 10, 1e-5);

    p = SVGPath("C 10 0 10 10 0 10");
    cout << "cubic bezier (10,0), (10,10), (0, 10): " << p.length() << endl;
    // ASSERT_CLOSE(p.length(), 5 * 3.141592, 1e-5);


    p = SVGPath("A 10 10 0 0 0 20 0");
    cout << "arc (10,10) 0 0 0 (10,0): " << p.length() << endl;
    ASSERT_CLOSE(p.length(), 10 * 3.1415927, 1e-5);
    


    cout << "testing combined paths..." << endl;
    p = SVGPath("L 10 0 A 10 10 0 0 0 30 0");
    cout << "L(10,0); A(10,10,0,0,0,30,0): " << p.length() << endl;
    ASSERT_CLOSE(p.length(), 10 + 10 * 3.1415927, 1e-5);


    cout << "s\tx\ty\n";
    for(double s = 0; s <= 1.0; s += 0.1) {
        Point q = p.at(s);
        cout << s << "\t" << q.x() << "\t" << q.y() << "\n";
    }
    cout << endl;

    return 0;
}