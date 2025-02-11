#include <iostream>

#include "parser.hpp"

int main() 
{
    string pathData = "A 4 4 0 1 0 8 0";

    SVGPath path(pathData);

    cout << "length: " << path.length() << endl;

    cout << "s\tx\ty\n";

    for(double s = 0; s <= 1.0; s += 0.1) {
        Point p = path.at(s);
        cout << s << "\t" << p.x() << "\t" << p.y() << "\n";    
    }
    cout << endl;


    return 0;
}