#ifndef __DRAWABLE_HPP__
#define __DRAWABLE_HPP__

#include "point.hpp"

struct Drawable;

struct Lengthy {
    virtual double length() const { return 0.; }
};

struct Drawable : 
    public Lengthy 
{
    // s is between 0 and 1
    virtual Point at(double s) const 
    { 
        return Point{}; 
    }
};

#endif