#ifndef __PLOTTER_HPP__
#define __PLOTTER_HPP__

#include <execution>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>

#include "parser.hpp"
#include "point.hpp"

using std::vector;
using std::for_each;

struct Stroke
{
    list<Event> _events;

    Stroke() : _events() { }
    Stroke(Event const & e) : _events{e} { }
    Stroke(Stroke const & s) : _events(s._events) { }
    Stroke(Stroke && s) : _events(move(s._events)) { }
    
    Stroke & operator=(Stroke const & s)
    {
        _events = s._events;
        return *this;
    }
    Stroke & operator=(Stroke && s)
    {
        _events = move(s._events);
        return *this;
    }

    Stroke & operator+=(Stroke && tail)
    {
        _events.splice(end(), tail._events);
        return *this;
    }

    Stroke & operator+=(Event && e)
    {
        _events.emplace_back(e);
        return *this;
    }
    Stroke & operator+=(Event const & e)
    {
        _events.emplace_back(e);
        return *this;
    }

    using const_iterator = list<Event>::const_iterator;
    using iterator = list<Event>::iterator;

    const_iterator begin() const { return _events.begin(); }
    const_iterator end() const { return _events.end(); }
    iterator begin() { return _events.begin(); }
    iterator end() { return _events.end(); }
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


bool segment_intersects_horizontal_ray(Point p0, Point p1, Point ray_origin)
{
    p0 -= ray_origin;
    p1 -= ray_origin;

    // ensure p0 has a lower y value than p1
    if(p0.y > p1.y) 
        swap(p0, p1);

    // is our ray in the right y-range?
    if(p0.y > 0 || p1.y < 0)
        return false;
    
    // are they parallel?
    if(p0.y == p1.y) {
        if(p0.x > 0 && p1.x > 0)
            return true;
        
        if(p0.x > 0)
            return true;

        if(p1.x > 0)
            return true;
            
        return false;
    }

    // calculate x-intercept
    double t = -p0.y / (p1.y - p0.y);

    if(t < 0. || t > 1.)
        return false;

    // is this to the right of our point?
    if(p0.x + t * (p1.x - p0.x) < 0)
        return false;

    return true;
}

struct Shape
{
    vector<Event> _outline;
    vector<Velocity> _velocities;
    vector<Acceleration> _accelerations;
    BoundingBox _bounding_box;

    double area() const
    {
        return ::area(_outline.begin(), _outline.end());
    }

    double perimeter() const
    {
        return ::path_length(_outline.begin(), _outline.end(), true);
    }

    using const_iterator = vector<Event>::const_iterator;

    const_iterator begin() const { return _outline.begin(); }
    const_iterator end() const { return _outline.end(); }

    PairwiseCircularAdapter<const_iterator> 
    segments() const
    {
        return PairwiseCircularAdapter<const_iterator>(begin(), end());
    }


    vector<Velocity> calculate_velocities(vector<Event> const & events)
    {
        using std::execution::par_unseq;

        vector<Velocity> velocity(events.size());

        for_each(par_unseq, velocity.begin(), velocity.end(), [&](Velocity & p) {
            size_t gid = &p - &velocity[0];
            size_t next = (gid + 1) % events.size();

            p = Velocity(events[gid], events[next]);
        });

        return velocity;
    }

    vector<Acceleration> calculate_accelerations(vector<Velocity> const & events)
    {
        using std::execution::par_unseq;

        vector<Acceleration> accel(events.size());

        for_each(par_unseq, accel.begin(), accel.end(), [&](Acceleration & p) {
            size_t gid = &p - &accel[0];
            size_t next = (gid + 1) % events.size();

            p = Acceleration(events[gid], events[next]);
        });

        return accel;
    }

    BoundingBox calculate_bounding_box(vector<Event> const & events)
    {
        using std::execution::par_unseq;

        auto iters = minmax_element(par_unseq, 
                                    events.begin(), events.end(), 
        [&](Event const & e0, Event const & e1) -> bool {
            return e0.x < e1.x || e0.y < e1.y;
        });

        return BoundingBox(static_cast<Point>(*iters.first), static_cast<Point>(*iters.second));
    }

    Shape() : _outline(), _velocities() { }

    Shape(vector<Event> && outline) :
        _outline(outline), 
        _velocities(calculate_velocities(_outline)),
        _accelerations(calculate_accelerations(_velocities)),
        _bounding_box(calculate_bounding_box(_outline))
    { }

    BoundingBox bounding_box() const { return _bounding_box; }

    bool is_inside(Point p) const 
    {
        using std::execution::par_unseq;

        
        Event last = _outline.back();
        int count = 0;
        for(Event const & e : _outline)
        {
            if(segment_intersects_horizontal_ray(last, e, p))
                count++;

            last = e;
        }

        return (count % 2) == 1;

        // PairwiseCircularAdapter<const_iterator> edges(begin(), end());
        // vector<unsigned char> intersections(edges.size());

        // transform(/*par_unseq, */
        //          edges.begin(), edges.end(), 
        //          intersections.begin(),
        // [&](pair<Point const &, Point const &> edge) -> unsigned char {
        //     if(segment_intersects_horizontal_ray(edge.first, edge.second, p))
        //         return 1;
            
        //     return 0;
        // });

        // unsigned char odd = reduce(par_unseq, 
        //                     intersections.begin(), intersections.end(), 0, 
        // [](unsigned char c, unsigned char d) {
        //     return (c + d) % 2;
        // });

        // return odd == 1;
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