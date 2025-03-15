#ifndef __PLOT_COVER_HPP__
#define __PLOT_COVER_HPP__

#include "drawable.hpp"
#include "plotter.hpp"
#include "cover.hpp"
#include "point.hpp"

#include <vector>
#include <utility>

class PlotCover :
    public Cover
{
private:
    std::vector<std::vector<Point>> _plot;

public:
    PlotCover() = default;
    PlotCover(Drawable const & d, size_t sample_count = 1000, std::pair<double, double> parameter_interval = {0,1}, double epsilon = 1e-2);

    // virtual double area() const override; // not implemented
    // virtual double perimeter() const override; // not implemented
    virtual std::vector<std::vector<Point>> outline() const override;
    virtual bool is_inside(Point const & p) const override;
    virtual std::pair<bool, double> intersect_ray(Point const & origin, Vector const & direction) const override;

    virtual ~PlotCover();
};


#endif