#ifndef __STROKE_HPP__
#define __STROKE_HPP__

#include "point.hpp"

#include <vector>
#include <list>
#include <utility>
#include <functional>

class StrokeIterator;
class Stroke;

class StrokeIterator
{
private:
    std::vector<Point>::const_iterator _i;
public:
    using value_type = Point const;
    using reference = Point const &;
    using pointer = Point const *;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;

    StrokeIterator() = default;
    StrokeIterator(StrokeIterator &&) = default;
    StrokeIterator(StrokeIterator const &) = default;

    StrokeIterator(std::vector<Point>::const_iterator &&);

    StrokeIterator & operator=(StrokeIterator &&) = default;
    StrokeIterator & operator=(StrokeIterator const &) = default;

    reference operator*();
    pointer operator->();
    StrokeIterator & operator++();
    StrokeIterator   operator++(int);
    StrokeIterator & operator+=(size_t);
    StrokeIterator & operator-=(size_t);
    StrokeIterator   operator+(size_t) const;
    StrokeIterator   operator-(size_t) const;
    difference_type   operator-(StrokeIterator const &) const;

    bool operator==(StrokeIterator const & r) const;
    bool operator!=(StrokeIterator const & r) const;
};

class StrokeView
{
private:
    StrokeIterator _begin, _end;
public:
    StrokeView() = default;
    StrokeView(StrokeView &&) = default;
    StrokeView(StrokeView const &) = default;

    StrokeIterator begin() const;
    StrokeIterator end() const;
};


class TriangleIterator
{
private:
    std::function<Triangle const &(size_t)> _accessor;
    size_t _i;
public:
    using value_type = Triangle const;
    using reference = Triangle const &;
    using pointer = Triangle const *;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;

    TriangleIterator() = default;
    TriangleIterator(TriangleIterator &&) = default;
    TriangleIterator(TriangleIterator const &) = default;

    TriangleIterator(std::function<Triangle const &(size_t)> accessor, size_t i = 0);

    TriangleIterator & operator=(TriangleIterator &&) = default;
    TriangleIterator & operator=(TriangleIterator const &) = default;

    reference operator*();
    pointer operator->();
    TriangleIterator & operator++();
    TriangleIterator   operator++(int);
    TriangleIterator & operator+=(size_t);
    TriangleIterator & operator-=(size_t);
    TriangleIterator   operator+(size_t) const;
    TriangleIterator   operator-(size_t) const;
    difference_type    operator-(TriangleIterator const &) const;

    bool operator==(TriangleIterator const & r) const;
    bool operator!=(TriangleIterator const & r) const;
};

// class Stroke 
// {
// private:
//     std::vector<Event> _events;
//     std::vector<double> _time_index;
//     std::vector<double> _length_index;

// public:
//     using const_iterator = std::vector<Event>::const_iterator;
//     // using iterator = vector<Event>::iterator;
//     using value_type = Event;

//     Stroke();
//     Stroke(Stroke const & s);
//     Stroke(Stroke && s);
    
//     Stroke(std::vector<Event> && events);

//     void calculate_indices();
    
//     Stroke & operator=(Stroke const & s);
//     Stroke & operator=(Stroke && s);

//     Stroke & operator+=(Stroke && tail);

//     double area() const;

//     Stroke & append(Point const & p);
//     Stroke & operator+=(Event const & e);
    

//     double arclength(const_iterator from, const_iterator to);

//     const_iterator begin() const;
//     const_iterator end() const;
//     // iterator begin() { return _events.begin(); }
//     // iterator end() { return _events.end(); }

//     value_type const & back() const;
// };

#endif