#ifndef __JSON_HPP__
#define __JSON_HPP__

#include "output.hpp"
#include "point.hpp"
#include "drawable.hpp"
#include "samplers.hpp"

#include <iostream>
#include <tuple>

using std::ostream;
using std::pair;

struct OutputJSON : 
    public Outputer
{
    Drawable const & drawable;
    size_t sample_count;
    pair<double,double> parameter_range;

    void print(ostream & os) const override;
};


void OutputJSON::print(ostream & os) const 
{

}

#endif