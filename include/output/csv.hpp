#ifndef __CSV_HPP__
#define __CSV_HPP__

#include "output.hpp"
#include "drawable.hpp"
#include "samplers.hpp"

#include <iostream>

using std::ostream;

struct OutputCSV :
    public Outputer
{
    Drawable const & drawable;

    struct Options 
    {
        size_t sample_count;
        pair<double,double> parameter_range;
    } options;

    void print(ostream & os) const override;

    OutputCSV(Drawable const & d, Options o) :
        drawable(d), options(o)
    { }
};


void OutputCSV::print(ostream & os) const
{
    os << "x\ty\tt\n";
    for(size_t i = 0; i < options.sample_count; i++)
    {
        double t = (double)i / (double)(options.sample_count - 1);
        t = options.parameter_range.first + (options.parameter_range.second - options.parameter_range.first) * t;

        Point p = drawable.at(t);

        os << p.x << "\t" << p.y << "\t" << t << "\n";
    }
}


#endif