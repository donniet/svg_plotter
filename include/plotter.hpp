#ifndef __PLOTTER_HPP__
#define __PLOTTER_HPP__

#include "point.hpp"
#include "cover.hpp"
#include "stroke.hpp"
#include "drawable.hpp"

#include <utility>
#include <vector>

/* Plotter class

    Requires the Drawable to accept values [-epsilon, 1]
    for estimation of derivative at the endpoint

*/
class Plotter 
{
public:
    double draw_speed; // length/time
    size_t sample_count;
    double epsilon; 
    std::pair<double,double> parameter_interval;
 
    Plotter();

    std::vector<std::vector<Point>> plot(Drawable const & drawing) ;

    std::vector<std::vector<Point>> fill(Cover const & cover,                    // a 2D area to be filled
                Drawable const & pattern);
};


std::vector<std::vector<Point>> simplify_plot(std::vector<std::vector<Point>> const &, 
                                              bool is_closed = false, 
                                              double eps = 1e-5);

std::vector<Point> simplify_path(std::vector<Point> const &,
                                 double eps = 1e-5);



#endif