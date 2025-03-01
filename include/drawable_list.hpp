#ifndef __DRAWABLE_LIST_HPP__
#define __DRAWABLE_LIST_HPP__

#include "drawable.hpp"

#include <vector>
#include <algorithm>
#include <execution>
#include <memory>

using std::vector;
using std::lower_bound;
using std::shared_ptr;

/*
TODO: this has fundamental memory management issues

class DrawableList :
    public Drawable
{
protected:
    vector<shared_ptr<Drawable>> _drawables;
    vector<double> _length_index;

public:
    void append(Drawable const & d)
    {
        double l = d.length();
        
        _drawables.emplace_back()
        _length_index.push_back(length() + l);
    }

    virtual double length() const override
    {
        return _length_index.back();
    }

    virtual Event at(double t) const override
    {
        double u = t * length();

        auto i = lower_bound(_length_index.begin(), _length_index.end(), u);
        if(i == _length_index.end())
            return Point{};

        size_t j = distance(_length_index.begin(), i);

        // TODO: unsure if we need to adjust the j index given lower_bound and 
        //       the extra 0 in _length_index
        Drawable const * d = _drawables[j]; 
        double upto = _length_index[j-1];
        
        double l = _length_index[j] - upto;
        u = (u - upto) / l; // make a perlength unit

        Event e = d->at(u);
        e.dp = e.dp * d->length() / length();

        return e;
    }
    

    DrawableList() : 
        _drawables(), _length_index()
    {
        // push a zero to initialize the length() function
        _length_index.push_back(0);
    }
    DrawableList(DrawableList const & r) :
        _drawables(r._drawables), _length_index(r._length_index)
    { }
    DrawableList(DrawableList && r) :
        _drawables(move(r._drawables)), _length_index(move(r._length_index))
    { }

    template<typename Iter>
    void append_range(Iter begin, Iter end)
    {
        for(; begin != end; begin++)
            append(*begin);
    }
};

*/

#endif