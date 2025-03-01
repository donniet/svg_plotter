#ifndef __COVER_HPP__
#define __COVER_HPP__

#include "path.hpp"
#include "point.hpp"

#include <limits>

using std::numeric_limits;

class Cover
{
private:
    double _margin;
public:
    virtual double area() const { return 0.; }
    virtual double perimeter() const { return 0.; }
    virtual Path outline() const { return Path{}; }
    virtual double & margin() { return _margin; }
    virtual double margin() const { return _margin; }
    virtual bool is_inside(Point const & p) const { return false; }
    virtual pair<bool,double> intersect_ray(Point const & origin,
                                            Vector const & direction) 
    const
    {
        return {false, 0.};
    }

    Cover() : _margin(-numeric_limits<double>::epsilon()) { }
};

#endif