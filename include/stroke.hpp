#ifndef __STROKE_HPP__
#define __STROKE_HPP__

#include "point.hpp"

#include <vector>
#include <utility>


class Stroke 
{
private:
    std::vector<Event> _events;
    std::vector<double> _time_index;
    std::vector<double> _length_index;

public:
    using const_iterator = std::vector<Event>::const_iterator;
    // using iterator = vector<Event>::iterator;
    using value_type = Event;

    Stroke();
    Stroke(Stroke const & s);
    Stroke(Stroke && s);
    
    Stroke(std::vector<Event> && events);

    void calculate_indices();
    
    Stroke & operator=(Stroke const & s);
    Stroke & operator=(Stroke && s);

    Stroke & operator+=(Stroke && tail);

    double area() const;

    Stroke & append(Point const & p);
    Stroke & operator+=(Event const & e);
    

    double arclength(const_iterator from, const_iterator to);

    const_iterator begin() const;
    const_iterator end() const;
    // iterator begin() { return _events.begin(); }
    // iterator end() { return _events.end(); }

    value_type const & back() const;
};

#endif