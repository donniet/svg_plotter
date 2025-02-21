
#include "adapters.hpp"
#include "parser.hpp"
#include "output/csv.hpp"

#include <iostream>

using std::cout;

int main(int ac, char * av[])
{
    SVGPath circle("M 10 20 L 20 20 A 10 10 0 0 0 40 20 A 30 30 0 0 0 10 10 Z");
    
    OutputCSV csv(circle, {
        .sample_count = 100,
        .parameter_range = {0., 1.},
    });

    cout << csv << endl;

    return 0;
}
