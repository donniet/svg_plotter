#ifndef __PLOTTER_HPP__
#define __PLOTTER_HPP__

#include <execution>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>

#include "parser.hpp"
#include "point.hpp"
#include "shape.hpp"
#include "stroke.hpp"

using std::vector;
using std::for_each;

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


struct Context 
{
    double draw_speed; // length/time
    size_t maximum_points;
};

Shape plot(Drawable const & drawing, Context ctx = { .draw_speed = 1., .maximum_points = 100 }) 
{
    using std::execution::par_unseq;

    vector<Event> points(ctx.maximum_points);

    double len = drawing.length();

    // first stab: just plot the points
    for_each(par_unseq, points.begin(), points.end(), [&](Event & e) {
        size_t gid = &e - &points[0];

        // t \in [0,1]
        double t = (double)gid / (double)(ctx.maximum_points - 1);

        Point p = drawing.at(t);

        e = Event(p, t * len / ctx.draw_speed);

    });

    return Shape(move(points));
}

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



class StrokeFiller 
{
private:
public:
    StrokeFiller() {}

public:
    vector<Stroke> fill(Shape const & shape, Drawable const & pattern, size_t point_count = 10000)
    {
        using std::execution::par_unseq;

        vector<Point> points(point_count);
        vector<int> is_inside(point_count);
        vector<int> scanned(point_count);
        
        for_each(/* par_unseq, */
                 points.begin(), points.end(),
        [&](Point & p) {
            size_t gid = &p - &points[0];
            double t = (double)gid / (double)(point_count - 1);

            p = pattern.at(t);
            is_inside[gid] = shape.is_inside(p) ? 1 : 0;
        });

        inclusive_scan(par_unseq, 
                       is_inside.begin(), is_inside.end(), 
                       scanned.begin());
                       
        vector<Stroke> ret;
        //TODO: make this parallel
        int last = 0;
        bool b = true;
        size_t count = 0;
        size_t total = scanned.back();

        for(size_t i = 0; i < point_count; i++)
        {
            if(scanned[i] == last) {
                b = true;
                continue;
            }

            ++count;
            double t = (double)count / (double)(total - 1);
            Event e(points[i], t);

            if(b) 
            {
                ret.push_back(Stroke());
                b = false;
            }
            else
                ret.back() += e;

            last = scanned[i];
        }

        return ret;
    }
};






#endif