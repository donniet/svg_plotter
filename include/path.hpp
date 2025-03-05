// path.hpp (Header file)

#ifndef __PATH_HPP__
#define __PATH_HPP__

#include "drawable.hpp"
#include "point.hpp"

#include <vector>
#include <utility> // Include the <utility> header for std::pair

class Path : public Drawable
{
protected:
    std::vector<Point> _points;
    std::vector<double> _length_index;

    void init();

public:
    virtual double length(double t0, double t1) const override;
    virtual Event at(double t) const override;

    Path();

    template<typename Iter>
    Path(Iter begin, Iter end);

    Path(std::vector<Point> const & p);
    Path(std::vector<Point> && p);
};

#endif // __PATH_HPP__