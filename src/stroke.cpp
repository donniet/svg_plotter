#include "stroke.hpp"

#include "utility.hpp"

#include <execution>
#include <utility>
#include <iterator>
#include <cmath>

using std::vector;
using std::move;
using std::distance;
using std::sqrt;

StrokeIterator::StrokeIterator(std::vector<Point>::const_iterator && i) :
    _i(i)
{ }

StrokeIterator::reference StrokeIterator::operator*()
{
    return *_i;
}
StrokeIterator::pointer StrokeIterator::operator->()
{
    return &*_i;
}
StrokeIterator StrokeIterator::operator++(int)
{
    StrokeIterator ret = *this;
    ++(*this);
    return ret;
}
StrokeIterator & StrokeIterator::operator+=(size_t j)
{
    _i += j;
    return *this;
}
StrokeIterator & StrokeIterator::operator-=(size_t j)
{
    _i -= j;
    return *this;
}
StrokeIterator StrokeIterator::operator+(size_t j) const
{
    return StrokeIterator(_i + j);
}
StrokeIterator StrokeIterator::operator-(size_t j) const
{
    return StrokeIterator(_i - j);
}
StrokeIterator::difference_type 
StrokeIterator::operator-(StrokeIterator const & r) const
{
    return _i - r._i;
}

bool StrokeIterator::operator==(StrokeIterator const & r) const
{
    return _i == r._i;
}
bool StrokeIterator::operator!=(StrokeIterator const & r) const
{
    return _i != r._i;
}

TriangleIterator::TriangleIterator(std::function<Triangle const &(size_t)> accessor, 
                                   size_t i) :
    _accessor(accessor), _i(i)
{ }

TriangleIterator::reference TriangleIterator::operator*() { return _accessor(_i); }
TriangleIterator::pointer TriangleIterator::operator->() { return &_accessor(_i); }
TriangleIterator & TriangleIterator::operator++() { ++_i; return *this; }
TriangleIterator TriangleIterator::operator++(int)
{
    TriangleIterator ret = *this;
    ++_i;
    return ret;
}
TriangleIterator & TriangleIterator::operator+=(size_t j) { _i += j; return *this; }
TriangleIterator & TriangleIterator::operator-=(size_t j) { _i -= j; return *this; }
TriangleIterator TriangleIterator::operator+(size_t j) const
{
    TriangleIterator ret = *this;
    ret += j;
    return ret;
}
TriangleIterator TriangleIterator::operator-(size_t j) const
{
    TriangleIterator ret = *this;
    ret -= j;
    return ret;
}
TriangleIterator::difference_type 
TriangleIterator::operator-(TriangleIterator const & r) const
{
    return (difference_type)_i - (difference_type)r._i;
}

bool TriangleIterator::operator==(TriangleIterator const & r) const
{
    return _i == r._i;
}
bool TriangleIterator::operator!=(TriangleIterator const & r) const
{
    return _i != r._i;
}

// Stroke::Stroke() : 
//     _events() 
// { }
// Stroke::Stroke(Stroke const & s) :
//     _events(s._events), _time_index(s._time_index), 
//     _length_index(s._length_index)
// { }
// Stroke::Stroke(Stroke && s) :
//     _events(move(s._events)), _time_index(move(s._time_index)), 
//     _length_index(move(s._length_index))
// { }

// Stroke::Stroke(vector<Event> && events) :
//     _events(events)
// {
//     calculate_indices();
// }

// void Stroke::calculate_indices()
// {
//     _time_index.resize(_events.size());
//     _length_index.resize(_events.size());

//     using std::execution::par_unseq;

//     for_each(par_unseq,
//                 _events.begin(), _events.end(),
//     [&](Event & e) {
//         size_t gid = distance(&_events[0], &e);
//         _time_index[gid] = e.dt;
//         _length_index[gid] = sqrt(e.dp.x * e.dp.x + e.dp.y * e.dp.y);
//     });

//     inclusive_scan(par_unseq,
//                     _time_index.begin(), _time_index.end(),
//                     _time_index.begin());

//     inclusive_scan(par_unseq,
//                     _length_index.begin(), _length_index.end(),
//                     _length_index.begin());
// }

// Stroke & Stroke::operator=(Stroke const & s)
// {
//     _events = s._events;
//     return *this;
// }
// Stroke & Stroke::operator=(Stroke && s)
// {
//     _events = move(s._events);
//     return *this;
// }

// Stroke & Stroke::operator+=(Stroke && tail)
// {
//     using std::execution::par_unseq;

//     _events.insert(end(), tail._events.begin(), tail._events.end());
//     auto b = _time_index.begin() + _time_index.size();
//     double last = _time_index.back();

//     _time_index.resize(_time_index.size() + tail._time_index.size());
//     transform(par_unseq,
//                 tail._time_index.begin(), tail._time_index.end(),
//                 b, 
//     [last](double t) -> double {
//         return last + t;
//     });

//     return *this;
// }

// double Stroke::area() const
// {
//     // assumes a closed stroke
//     return ::enclosed_area(_events.begin(), _events.end());
// }

// Stroke & Stroke::append(Point const & p)
// {
//     _events.emplace_back(p);
//     return *this;
// }
// Stroke & Stroke::operator+=(Event const & e)
// {
//     _events.emplace_back(e);
//     return *this;
// }

// double Stroke::arclength(Stroke::const_iterator from, Stroke::const_iterator to)
// {
//     throw std::logic_error("unimplemented");
    
//     double fin;
//     if(to == end())
//         fin = _length_index.back();
//     else 
//     {
//     //    size_t e = distance(_events.begin(), to);
        
//     }
// }

// Stroke::const_iterator Stroke::begin() const { return _events.begin(); }
// Stroke::const_iterator Stroke::end() const { return _events.end(); }
// // iterator begin() { return _events.begin(); }
// // iterator end() { return _events.end(); }

// Stroke::value_type const & Stroke::back() const { return _events.back(); }