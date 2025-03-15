#include "plot_cover.hpp"

#include "utility.hpp"

#include <limits>
#include <vector>
#include <utility>

using std::numeric_limits;
using std::pair;
using std::vector;

PlotCover::~PlotCover() { }

PlotCover::PlotCover(Drawable const & outline, size_t sample_count, pair<double, double> parameter_interval, double epsilon) :
    Cover(), _plotter(sample_count, parameter_interval, epsilon)
{
    _plot = _plotter.plot(outline);
}

vector<vector<Point>> PlotCover::outline() const
{
    return _plot;
}

bool PlotCover::is_inside(Point const & p) const
{
    size_t count = 0;

    for(size_t j = 0; j < _plot.size(); j++) 
        for(size_t i = 1; i < _plot[j].size(); i++)
            if(segment_intersects_horizontal_ray(_plot[j][i-1], _plot[j][i], p, _plotter.epsilon()).first)
                count++;

    return count % 2 == 1;
}

pair<bool, double> PlotCover::intersect_ray(Point const & origin, Vector const & direction) const
{
    double tmin = numeric_limits<double>::max();

    Line r{origin, direction};

    for(size_t j = 0; j < _plot.size(); j++) 
        for(size_t i = 1; i < _plot[j].size(); i++)
        {
            Segment s{_plot[j][i-1], _plot[j][i]};
            
            auto n = r.intersect(s);

            if(n.first && n.second >= 0 && n.second < tmin)
                tmin = n.second;
        }

    return { tmin != numeric_limits<double>::max(), tmin };
}