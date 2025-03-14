// drawable.hpp (Header file)

#ifndef __DRAWABLE_HPP__
#define __DRAWABLE_HPP__

#include "point.hpp"
#include "forward_deps.hpp"

#include <algorithm>
#include <vector>
#include <utility>
#include <functional>
#include <limits>

class Drawable 
{
public:
    virtual double length(double t0 = 0., double t1 = 1.) const;
    virtual Event at(double t) const;
    virtual BoundingBox bounding_box() const;
    virtual std::pair<bool, double> last_move_between(double t0 = 0., double t1 = 1.) const;
    virtual ~Drawable() = default;
};

class DrawableFromFunction : public Drawable
{
protected:
    std::function<Point(double)> _f;
    size_t _sample_count;
    std::pair<double, double> _interval;
    std::vector<Event> _samples;
    std::vector<double> _lengths;
    std::vector<double> _length_index;

public:
    DrawableFromFunction(std::function<Point(double)> f, 
                         std::pair<double, double> interval = {0, 1}, 
                         size_t sample_count = 1000);

    DrawableFromFunction(std::pair<double, double> interval = {0, 1}, 
                         size_t sample_count = 1000);

    void reset(std::function<Point(double)> f);

    virtual Event at(double t) const override;
    virtual double length(double t0, double t1) const override;

protected:
    std::pair<size_t, double> segment_by_parameter(double t) const;
    double length_to(double t) const;

private:
    void init();
};

#endif // __DRAWABLE_HPP__