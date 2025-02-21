#ifndef __STROKE_HPP__
#define __STROKE_HPP__

#include "point.hpp"

#include <list>
#include <utility>

using std::list;
using std::move;

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



#endif