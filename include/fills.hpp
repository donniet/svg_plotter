#ifndef __FILLS_HPP__
#define __FILLS_HPP__

#include "point.hpp"
#include "bounding_box.hpp"
#include "drawable.hpp"

class Spiral : 
    public Drawable
{
protected:
    Point _center;
    BoundingBox _region;
    double _dradius;

    double _max_parameter;

    double find_max_parameter()
    {
        // estimate the length of the spiral from concentric circles
        // which corner of the bounding box is farthest from the center?
        
        auto corners = _region.corners();
        double max = 0.;

        for_each(corners.begin(), corners.end(), [&](Point const & c) {
            double d = (_center - c).norm2();
            if(d > max)
                max = d;
        });

        return sqrt(max) / _dradius;
    }
public:
    virtual Point at(double t) const override
    {
        double theta = t * _max_parameter;
        double r = _dradius * theta;

        return Point(r * cos(theta), r * sin(theta)) + _center;
    }

    Spiral(BoundingBox region, Point center, double dradius = 1.) :
        _center(center), _region(region), _dradius(dradius), 
        _max_parameter(find_max_parameter())
    { }
};


#endif