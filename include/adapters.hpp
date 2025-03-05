#ifndef __ADAPTERS_HPP__
#define __ADAPTERS_HPP__

#include "point.hpp"
#include "drawable.hpp"

#include <vector>
#include <execution>
#include <algorithm>
#include <iterator>
#include <utility>

using std::vector;
using std::execution::par_unseq;
using std::distance;
using std::reduce;
using std::for_each;
using std::reduce;
using std::inclusive_scan;
using std::pair;

struct ConstantArcLengthAdapter :
    public Drawable
{
    Drawable const & _d;
    size_t _maximum_points;
    pair<double,double> _interval;
    vector<double> _length_index;

    void init();

    ConstantArcLengthAdapter(Drawable const & d, size_t maximum_points = 1e5, pair<double,double> interval = {0,1});

    virtual double length(double t0, double t1) const override;
    virtual Event at(double t) const override;
};




#endif