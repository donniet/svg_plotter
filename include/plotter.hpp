#ifndef __PLOTTER_HPP__
#define __PLOTTER_HPP__

#include <execution>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>

#include "parser.hpp"
#include "point.hpp"
#include "cover.hpp"
#include "shape.hpp"
#include "stroke.hpp"

using std::vector;
using std::for_each;


/* Plotter class

    Requires the Drawable to accept values [-epsilon, 1]
    for estimation of derivative at the endpoint

*/
class Plotter 
{
public:
    double draw_speed; // length/time
    size_t sample_count;
    double epsilon; 
    pair<double,double> parameter_interval;
 
    Plotter() :
        draw_speed(1), sample_count(100), 
        epsilon(1e-5), parameter_interval(0,1)
    { }

    Stroke plot(Drawable const & drawing) 
    {
        using std::execution::par_unseq;

        vector<Event> points(sample_count);

        double len = drawing.length(0, 1);

        // first stab: just plot the points
        for_each(par_unseq, 
                 points.begin(), points.end(), 
        [&](Event & e) 
        {
            size_t gid = &e - &points[0];

            // t \in [0,1]
            double t = (double)gid / (double)(sample_count - 1);
            t = parameter_interval.first + t * (parameter_interval.second - parameter_interval.first);

            Point p = drawing.at(t);
            Point p0 = drawing.at(t - epsilon);
            
            // |dp/dt| should equal draw_speed
            Vector ds(p.x - p0.x, p.y - p0.y);
            ds = ds.normalized();

            e = Event(p, ds * draw_speed);
        });

        return Stroke(move(points));
    }

    Stroke fill(Cover const & cover,                    // a 2D area to be filled
                Drawable const & pattern)               // a drawable which we will sample over the interval [0,1] to fill
    {
        using std::execution::par_unseq;

        vector<Event> events(sample_count);
        vector<int> is_inside(sample_count);
        vector<int> scanned(sample_count);
        
        for_each(/* par_unseq, */
                    events.begin(), events.end(),
        [&](Event & p) 
        {
            size_t gid = &p - &events[0];
            double t = (double)gid / (double)(sample_count - 1);

            p = pattern.at(t);
            if(gid > 0 && pattern.last_move_between((double)(gid - 1) / (double)(sample_count - 1), t).first)
            {
                p.dt = 0;
            }

            is_inside[gid] = cover.is_inside(p) ? 1 : 0;
        });

        inclusive_scan(par_unseq, 
                    is_inside.begin(), is_inside.end(), 
                    scanned.begin());
                        
        int last = 0;
        bool b = true;
        size_t count = 0;
        size_t total = scanned.back();

        vector<Event> inside(total);

        // pack the events
        for_each(/* par_unseq, */
                events.begin(), events.end(),
        [&](Event & e) 
        {
            size_t gid = distance(&events[0], &e);

            // is this event inside our cover?
            if(!is_inside[gid])
                return; // nope

            // use our scanned vector to calculate the index 
            // of this inside event in the final vector
            Event * pe = &inside[scanned[gid]-1];
            *pe = e;
            
            // are we the first event
            if(gid == 0 || !is_inside[gid-1])
                make_move_to(*pe); // turn this into a move_to

        });
        
        return Stroke(move(inside)); // turn this into a stroke and return it
    }
};

template<typename Iter>
struct PairwiseCircularAdapter
{
    using value_type = pair<typename Iter::value_type const &, typename Iter::value_type const &>;

    Iter _begin, _end;
    
    struct const_iterator 
    {
        Iter _i;
        PairwiseCircularAdapter const * _adapter;
        using iterator_category = typename Iter::iterator_category;
        using value_type = PairwiseCircularAdapter<Iter>::value_type;
        using difference_type = typename Iter::difference_type;
        using pointer = value_type*;
        using reference = value_type&;

        const_iterator() : _adapter(nullptr) { }
        const_iterator(Iter i, PairwiseCircularAdapter const & adapter) : 
            _i(i), _adapter(&adapter) 
        { }

        const_iterator(const_iterator const & i) :
            _i(i._i), _adapter(i._adapter)
        { }
        const_iterator(const_iterator && i) :
            _i(i._i), _adapter(i._adapter)
        { }

        bool operator==(const_iterator const & r) 
        {
            // _adapter being null signifies end()

            if(_adapter == nullptr && r._adapter == nullptr)
                return true;

            if(_adapter == nullptr && r._i == r._adapter->_end)
                return true;

            if(_i == _adapter->_end && r._adapter == nullptr)
                return true;

            if(_adapter == nullptr || r._adapter == nullptr)
                return false;

            return _i == r._i;
        }
        bool operator!=(const_iterator const & r) 
        {
            return !(_i == r._i);
        }

        const_iterator & operator=(const_iterator const & i)
        {
            _i = i._i;
            return *this;
        }
        const_iterator & operator=(const_iterator && i)
        {
            _i = i._i;
            return *this;
        }
        const_iterator & operator++()
        {
            ++_i;
            return *this;
        }
        const_iterator operator++(int)
        {
            const_iterator r = *this;
            operator++();
            return r;
        }
        typename Iter::difference_type operator-(const_iterator const & r)
        {
            return _i - r._i;
        }
        
        value_type operator*()
        {
            auto j = _i;
            j++;

            if(j == _adapter->_end)
                j = _adapter->_begin;
            
            return { *_i, *j };
        }
    };


    PairwiseCircularAdapter(Iter begin, Iter end) :
        _begin(begin), _end(end)
    { }

    const_iterator begin() const 
    {
        return const_iterator(_begin, *this);
    }
    const_iterator end() const 
    {
        return const_iterator(_end, *this);
    }

    size_t size() const 
    {
        return distance(_begin, _end);
    }

};


template<typename T>
class Grid
{
private:
    BoundingBox _box;
    double _grid_size;
    size_t _sx, _sy;
    vector<T> _grid;

public:
    Grid(BoundingBox box, double grid_size) :
        _box(box), _grid_size(grid_size)
    { 
        Vector diag = _box.diagonal();

        _sx = ceil(diag.x / grid_size);
        _sy = ceil(diag.y / grid_size);

        _grid.resize(_sx * _sy);
    }

    T const & operator[](size_t x, size_t y) const
    {
        return _grid[y * _sx + x];
    }
    T & operator[](size_t x, size_t y) 
    {
        return _grid[y * _sx + x];
    }

    T const & operator()(Point p) const 
    {
        p -= _box.p0;

        size_t x = floor(p.x / _grid_size);
        size_t y = floor(p.y / _grid_size);

        return operator[](x, y);
    }
    T & operator()(Point p) 
    {
        p -= _box.p0;

        size_t x = floor(p.x / _grid_size);
        size_t y = floor(p.y / _grid_size);

        return operator[](x, y);
    }

    using const_iterator = vector<T>::const_iterator;
    using iterator = vector<T>::iterator;

    vector<T>::const_iterator begin() const { return _grid.begin(); }
    vector<T>::const_iterator end() const { return _grid.end(); }
    vector<T>::iterator begin() { return _grid.begin(); }
    vector<T>::iterator end() { return _grid.end(); }

    pair<size_t, size_t> index_of(vector<T>::const_iterator i) const 
    {
        size_t n = distance(begin(), i);

        return { n % _sx, n / _sx };
    }
    pair<size_t, size_t> index_of(T const & val) const 
    {
        size_t n = &val - &_grid[0];

        return { n % _sx, n / _sx };
    }    
    pair<size_t, size_t> index_of(Point p) const 
    {
        p -= _box.p0;

        return { 
            floor(p.x / _grid_size), 
            floor(p.y / _grid_size) 
        };
    }    
    Point center_of(vector<T>::const_iterator i) const 
    {
        auto d = index_of(i);

        return _box.p0 + Point(_grid_size * (0.5 + (double)d.first), 
                               _grid_size * (0.5 + (double)d.second));
    }
    Point center_of(T const & val) const 
    {
        auto d = index_of(val);

        return _box.p0 + Point(_grid_size * (0.5 + (double)d.first), 
                               _grid_size * (0.5 + (double)d.second));
    }
};









#endif