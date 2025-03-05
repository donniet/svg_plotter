#ifndef __PLOTTER_HPP__
#define __PLOTTER_HPP__

#include "point.hpp"
#include "cover.hpp"
#include "stroke.hpp"
#include "drawable.hpp"

#include <utility>

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

    Stroke plot(Drawable const & drawing) ;

    Stroke fill(Cover const & cover,                    // a 2D area to be filled
                Drawable const & pattern);
};









#endif