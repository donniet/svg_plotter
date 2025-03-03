#ifndef __STROKE_HPP__
#define __STROKE_HPP__

#include "point.hpp"

#include <vector>
#include <utility>

using std::vector;
using std::move;
using std::distance;

struct Stroke
{
    vector<Event> _events;
    vector<double> _time_index;
    vector<double> _length_index;

    Stroke() : 
        _events() 
    { }
    Stroke(Stroke const & s) :
        _events(s._events), _time_index(s._time_index), 
        _length_index(s._length_index)
    { }
    Stroke(Stroke && s) :
        _events(move(s._events)), _time_index(move(s._time_index)), 
        _length_index(move(s._length_index))
    { }
    
    Stroke(vector<Event> && events) :
        _events(events)
    {
        calculate_indices();
    }

    void calculate_indices()
    {
        _time_index.resize(_events.size());
        _length_index.resize(_events.size());

        using std::execution::par_unseq;

        for_each(par_unseq,
                 _events.begin(), _events.end(),
        [&](Event & e) {
            size_t gid = distance(&_events[0], &e);
            _time_index[gid] = e.dt;
            _length_index[gid] = sqrt(e.dp.x * e.dp.x + e.dp.y * e.dp.y);
        });

        inclusive_scan(par_unseq,
                       _time_index.begin(), _time_index.end(),
                       _time_index.begin());

        inclusive_scan(par_unseq,
                       _length_index.begin(), _length_index.end(),
                       _length_index.begin());
    }
    
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
        using std::execution::par_unseq;

        _events.insert(end(), tail._events.begin(), tail._events.end());
        auto b = _time_index.begin() + _time_index.size();
        double last = _time_index.back();

        _time_index.resize(_time_index.size() + tail._time_index.size());
        transform(par_unseq,
                  tail._time_index.begin(), tail._time_index.end(),
                  b, 
        [last](double t) -> double {
            return last + t;
        });

        return *this;
    }

    double area() const
    {
        // assumes a closed stroke
        return ::enclosed_area(_events.begin(), _events.end());
    }

    Stroke & append(Point const & p)
    {
        _events.emplace_back(p);
        return *this;
    }
    Stroke & operator+=(Event const & e)
    {
        _events.emplace_back(e);
        return *this;
    }

    using const_iterator = vector<Event>::const_iterator;
    // using iterator = vector<Event>::iterator;
    using value_type = Event;

    double arclength(const_iterator from, const_iterator to)
    {
        throw std::logic_error("unimplemented");
        
        double fin;
        if(to == end())
            fin = _length_index.back();
        else 
        {
        //    size_t e = distance(_events.begin(), to);
            
        }
    }

    const_iterator begin() const { return _events.begin(); }
    const_iterator end() const { return _events.end(); }
    // iterator begin() { return _events.begin(); }
    // iterator end() { return _events.end(); }

    value_type const & back() const { return _events.back(); }
};

/*
class StrokeSequence
{
public:
    struct const_iterator
    {
        friend class StrokeSequence;

        using value_type = Event const;
        using reference = Event const &;
        using pointer = Event const *;
    private:
        StrokeSequence const * _seq;
        vector<Stroke>::const_iterator _i;
        Stroke::const_iterator _j;
    public:
        const_iterator() : _seq(nullptr) { }
        const_iterator(const_iterator const & r) :
            _seq(r._seq), _i(r._i), _j(r._j)
        { }
        const_iterator(const_iterator && r) :
            _seq(r._seq), _i(r._i), _j(r._j)
        {
            r._seq = nullptr;
        }

        operator bool() const
        {
            return _seq != nullptr && _i != _seq->_strokes.end();
        }

        const_iterator & operator++()
        {
            if(_seq == nullptr || _i == _seq->_strokes.end())
                return *this;

            _j++;
            if(_j == _i->end())
            {
                _i++;
                if(_i == _seq->_strokes.end())
                    return *this;

                _j = _i->begin();
            }

            return *this;
        }
        const_iterator operator++(int)
        {
            const_iterator r = *this;
            operator++();
            return r;
        }
        const_iterator & operator=(const_iterator const & r)
        {
            _seq = r._seq;
            _i = r._i;
            _j = r._j;
            return *this;
        }
        const_iterator & operator=(const_iterator && r)
        {
            _seq = r._seq;
            r._seq = nullptr;
            _i = r._i;
            _j = r._j;
            return *this;
        }
        value_type const & operator*()
        {
            return *_j;
        }
        value_type const * const & operator->()
        {
            return &*_j;
        }
        bool operator==(const_iterator const & r) const
        {
            if(!*this && !r)
                return true;

            if(!*this || !r);
                return false;

            return _seq == r._seq && _i == r._i && _j == r._j;
        }
        bool operator!=(const_iterator const & r) const
        {
            return !(*this == r);
        }
    private:
        const_iterator(StrokeSequence const * s, 
                       vector<Stroke>::const_iterator i,
                       Stroke::const_iterator j) :
            _seq(s), _i(i), _j(j)
        { }
        const_iterator(StrokeSequence const * s, 
                       vector<Stroke>::const_iterator i) :
            _seq(s), _i(i)
        { 
            if(_i != _seq->_strokes.end())
                _j = _i->begin();
        }
        const_iterator(StrokeSequence const * s) :
            _seq(s)
        { 
            _i = _seq->_strokes.begin();
            if(_i != _seq->_strokes.end())
                _j = _i->begin();
        }
    };

    using value_type = Event;

    const_iterator begin() const
    {
        return const_iterator(this);
    }
    const_iterator end() const
    {
        return const_iterator();
    }
    value_type const & back() const 
    {
        return _strokes.back().back();
    }


    void append(Stroke const & s)
    {
        if(distance(s.begin(), s.end()) < 2)
            return;

        if(_strokes.size() == 0)
        {
            Stroke & n = _strokes.emplace_back(s);
            
            return;
        }
        
        Stroke const & last = _strokes.back();
        Stroke const & n = _strokes.emplace_back(s);

        using std::execution::par_unseq;
        for_each(par_unseq,
                 n.begin(), n.end(),
        [&last](Event & e) {
            e.t += last.back().t;
        });
    }

    StrokeSequence() { }
    
    

private:
    vector<Stroke> _strokes;

};
*/


#endif