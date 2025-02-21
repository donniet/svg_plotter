
#include "testing.hpp"

#include "adapters.hpp"
#include "parser.hpp"
#include "output/csv.hpp"

#include <iostream>
#include <vector>
#include <execution>
#include <algorithm>

using namespace std;

using std::execution::par_unseq;

struct Function :
    public Drawable
{
    virtual Point at(double t) const override 
    {
        return Point(t, 0.1 * t*t*t);
    }
};


int main(int ac, char * av[])
{
    Function par;

    pair<double,double> interval{0, 3};
    double range = interval.second - interval.first;
    double eps = 1e-4 * range;

    auto d0 = distrib(par, 100, interval);

    cout << "Function:\n"
         << "mean:  " << d0.first  << "\n"
         << "stdev: " << d0.second << "\n";

    // we should use many more samples than we need here 
    auto parc = ConstantArcLengthAdapter(par, 100000, interval);

    auto d1 = distrib(parc, 100, interval);
    cout << "Constant Arc Length:\n" 
         << "mean:  " << d1.first  << "\n"
         << "stdev: " << d1.second << "\n";

    
    // the standard deviation of lengths sampled should be close to zero
    // if our equal arc length adapter is working properly
    ASSERT_CLOSE((parc.at(interval.first) - par.at(interval.first)).norm(), 0, eps);
    ASSERT_CLOSE((parc.at(interval.second) - par.at(interval.second)).norm(), 0, eps);
    ASSERT_CLOSE(d0.first, d1.first, eps);
    ASSERT_CLOSE(d1.second, 0, eps);

    
    OutputCSV csv(parc, {
        .sample_count = 100,
        .parameter_range = interval,
    });

    cout << csv << endl;



    return 0;
}
