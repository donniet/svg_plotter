#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "drawable.hpp"
#include "point.hpp"
#include "bezier.hpp"
#include "formatters.hpp"

#include <string>
#include <vector>
#include <cmath>
#include <regex>
#include <format>
#include <functional>
#include <algorithm>
#include <execution>
#include <numbers>
#include <functional>

using namespace std;

using std::numbers::pi;

struct PathSegment;

struct MoveTo :
    public Drawable
{
protected:
    Point _from, _to;
public:
    // length is 0, no need to override

    virtual Event at(double t) const override 
    {
        Vector dp = _to - _from;
        dp = dp.normalized();

        return Event(_to, dp, 0.); // 0 means a move_to
    }

    MoveTo(Point from, Point to) :
        _from(from), _to(to)
    {  }
};

struct LineTo :
    public Drawable
{
protected:
    Point _from, _to;
    double _length;
public:
    virtual double length(double t0, double t1) const override
    {
        return (at(t1) - at(t0)).norm();
    }
    
    // HACK: these are assuming the vector ahead of the point
    //       but the move_to logic asssumes the derivative is
    //       at the start point
    virtual Event at(double t) const override 
    {

        Point p = _from + t * (_to - _from);
        Vector v = _to - _from;

        return Event(p, v);
    }

    LineTo(Point from, Point to) :
        _from(from), _to(to), _length((_to - _from).norm())
    {  }
};


struct CubicBezier : 
    public DrawableFromFunction
{
protected:
    Point _from, _c1, _c2, _to;

    using DrawableFromFunction::reset;
private:
public:
    CubicBezier(Point from, Point c1, Point c2, Point to) :
        DrawableFromFunction(),
        _from(from), _c1(c1), _c2(c2), _to(to)
    { 
        reset([&](double t) 
        {
            return (*this)(t);
        });
    }

    Point operator()(double t) const
    {
        return bezier_point(t, _from, _c1, _c2, _to);
    }
};

struct QuadraticCurve : 
    public DrawableFromFunction
{
protected:
    Point _from, _c1, _to;

    using DrawableFromFunction::reset;
private:
public:
    QuadraticCurve(Point from, Point c1, Point to) :
        DrawableFromFunction(),
        _from(from), _c1(c1), _to(to)
    { 
        reset([&](double t)
        {
            return (*this)(t);
        });
    }

    Point operator()(double t) const
    {
        return bezier_point(t, _from, _c1, _to);
    }
};



struct Arc :
    public DrawableFromFunction
{
protected:
    Point _from, _to;

    Point _radius;
    double _angle;
    bool _large_arc;
    bool _sweep;

    Point _center;
    double _start_angle;
    double _sweep_angle;

    void init() 
    {
        // https://svgwg.org/svg2-draft/implnote.html#ArcImplementationNotes
        double sinA = sin(_angle);
        double cosA = cos(_angle);
        Point xp = (_from - _to) / 2.;

        Point xq{
             cosA * xp.x + sinA * xp.y,
            -sinA * xp.x + cosA * xp.y
        };
        double rx = _radius.x;
        double ry = _radius.y;

        double s = sqrt((rx * rx * ry * ry - rx * rx * xq.y * xq.y - ry * ry * xq.x * xq.x) / (rx * rx * xq.y * xq.y + ry * ry * xq.x * xq.x));
        if(_large_arc == _sweep)
            s *= -1;
        
        Point c1 = s * Point{rx * xq.y / ry, -ry * xq.x / rx};
        xp = (_to + _from) / 2.;

        _center = xp + Point{
            cosA * c1.x - sinA * c1.y,
            sinA * c1.x + cosA * c1.y
        };

        Point s1{(xq.x - c1.x) / rx, (xq.y - c1.y) / ry};
        Point s2{(-xq.x - c1.x) / rx, (-xq.y - c1.y) / ry};
    
        _start_angle = Point{1,0}.angle(s1);
        _sweep_angle = s1.angle(s2);
    }

    using DrawableFromFunction::reset;
public:
    Point operator()(double t) const
    {
        // use the angle here
        // TODO: make this arclength compatible
        double a = _start_angle + t * _sweep_angle;

        Point p{ _radius.x * cos(a), _radius.y * sin(a) };

        double sinA = sin(_angle);
        double cosA = cos(_angle);

        return _center + Point{cosA * p.x - sinA * p.y, sinA * p.x + cosA * p.y};
    }

    Arc(Point from, Point radius, double angle, bool large_arc, bool sweep, Point to) :
        DrawableFromFunction(),
        _from(from), _to(to), _radius(radius), _angle(angle), _large_arc(large_arc), _sweep(sweep)
    { 
        init();

        reset([&](double t)
        {
            return (*this)(t);
        });
    }
};



class SVGPath :
    public Drawable
{
public:

    SVGPath(const string& pathData, Point const & origin = Point{0,0}) :
        _from(origin), _to(origin)
    {
        parsePath(pathData);
    }

    virtual double length(double t0, double t1) const override
    {
        double l0 = length_to(t0);
        double l1 = length_to(t1);
        
        return l1 - l0;
    }

    virtual Event at(double t) const override 
    {
        if(_length_index.size() == 0)
            return _to;

        size_t gid;
        double u;

        tie(gid, u) = segment_by_parameter(t);
        
        return _segments[gid]->at(u);
    }

private:
    // assumes _segments.size() > 0
    pair<size_t, double> segment_by_parameter(double t) const 
    {
        // if t is less than 0 return the first element with no remainder
        if(t <= 0)
            return { 0, 0 };
        
        // if t is greater or equal to 1 return the last segment with parameter 1 (endpoint)
        if(t >= 1)
            return { _segments.size() - 1, 1 };


        double len = _length_index.back();
        double s = len * t;

        size_t low_index = upper_bound(&_length_index[0], &_length_index[0] + _length_index.size(), s) -
                           &_length_index[0];

        // if this isn't the first one, subtract the length of the previous segments
        // if it is the first one these previous lengths are simply 0
        if(low_index > 0)
            s -= _length_index[low_index-1];

        // return a pointer to the segment and a remaining parameter normalized by the length of this segment
        return { low_index, s / _lengths[low_index] };
    }

    double length_to(double t) const 
    {
        if(_length_index.size() == 0)
            return 0;

        if(t <= 0)
            return 0;

        if(t >= 1)
            return _length_index.back();

        size_t gid;
        double u;

        tie(gid, u) = segment_by_parameter(t);

        double ret = _segments[0]->length(0, u);

        if(gid > 0)
            ret += _length_index[gid - 1];

        return ret;
    }

    void append_segments(string::value_type key, vector<double> const & coords, Point & pen, Vector & dir)
    {
        auto upper_key = toupper(key);
        Drawable * seg;

        bool rel = (key != upper_key);

        switch(upper_key) {
        case 'M':
            for(int i = 0; i < coords.size(); i += 2) {
                Point to{coords[i], coords[i+1]};
                if(rel) to += pen;

                seg = dynamic_cast<Drawable*>(new MoveTo(pen, to));

                _segments.emplace_back(seg);
                _lengths.push_back(0.);

                pen = to;
            }
            dir = Vector{0,0};
            break;
        case 'L':
            for(int i = 0; i < coords.size(); i += 2) {
                Point to{coords[i], coords[i+1]};
                if(rel) to += pen;

                seg = dynamic_cast<Drawable*>(new LineTo(pen, to));
                _segments.emplace_back(seg);
                _lengths.push_back(seg->length(0, 1));

                pen = to;
            }
            dir = Vector{0,0};
            break;
        case 'H':
            for(int i = 0; i < coords.size(); i++) {
                Point to{coords[i], pen.y};
                if(rel) to.x += pen.x;

                seg = dynamic_cast<Drawable*>(new LineTo(pen, to));
                _segments.emplace_back(seg);
                _lengths.push_back(seg->length(0, 1));

                pen = to;
            }
            dir = Vector{0,0};
            break;
        case 'V':
            for(int i = 0; i < coords.size(); i++) {
                Point to{pen.x, coords[i]};
                if(rel) to.y += pen.y;

                seg = dynamic_cast<Drawable*>(new LineTo(pen, to));
                _segments.emplace_back(seg);
                _lengths.push_back(seg->length(0, 1));

                pen = to;
            }
            dir = Vector{0,0};
            break;
        case 'A':
            for(int i = 0; i < coords.size(); i += 7) {
                Point radius{coords[i], coords[i+1]};
                double angle = coords[i+2];
                bool large_arc = (coords[i+3] != 0);
                bool clockwise = (coords[i+4] != 0);
                Point to{coords[i+5], coords[i+6]};

                seg = dynamic_cast<Drawable*>(new Arc(pen, radius, angle, large_arc, clockwise, to));
                _segments.emplace_back(seg);
                _lengths.push_back(seg->length(0, 1));

                pen = to;
            }
            dir = Vector{0,0};
            break;
        case 'C':
            for(int i = 0; i < coords.size(); i += 6) {
                Point p1{coords[i], coords[i+1]};
                Point p2{coords[i+2], coords[i+3]};
                Point p3{coords[i+4], coords[i+5]};

                if(rel) {
                    p1 += pen;
                    p2 += pen;
                    p3 += pen;
                }

                seg = dynamic_cast<Drawable*>(new CubicBezier(pen, p1, p2, p3));
                _segments.emplace_back(seg);
                _lengths.push_back(seg->length(0, 1));

                pen = p3;
                dir = p3 - p2;
            }
            break;
        case 'S':
            for(int i = 0; i < coords.size(); i += 4) {
                Point p1{coords[i], coords[i+1]};
                Point p2{coords[i+2], coords[i+3]};

                if(rel) {
                    p1 += pen;
                    p2 += pen;
                }

                seg = dynamic_cast<Drawable*>(new CubicBezier(pen, pen + dir, p1, p2));
                _segments.emplace_back(seg);
                _lengths.push_back(seg->length(0, 1));

                pen = p2;
                dir = p2 - p1;
            }
            break;
        case 'Q':
            for(int i = 0; i < coords.size(); i += 4) {
                Point p1{coords[i], coords[i+1]};
                Point p2{coords[i+2], coords[i+3]};

                if(rel) {
                    p1 += pen;
                    p2 += pen;
                }

                seg = dynamic_cast<Drawable*>(new QuadraticCurve(pen, p1, p2));
                _segments.emplace_back(seg);
                _lengths.push_back(seg->length(0, 1));

                pen = p2;
                dir = p2 - p1;
            }
            break;
        case 'T':
            for(int i = 0; i < coords.size(); i += 2) {
                Point p1{coords[i], coords[i+1]};

                if(rel) {
                    p1 += pen;
                }

                seg = dynamic_cast<Drawable*>(new QuadraticCurve(pen, pen + dir, p1));
                _segments.emplace_back(seg);
                _lengths.push_back(seg->length(0, 1));

                pen = p1;
                dir = p1 - pen - dir;
            }
            break;
        case 'Z':
            seg = dynamic_cast<Drawable*>(new LineTo(pen, _from));
            _segments.emplace_back(seg);
            _lengths.push_back(seg->length(0, 1));

            pen = _from;
            dir = Vector{0,0};
            break;
        }

        double current_length = 0;
        if(_length_index.size() > 0)
            current_length = _length_index.back();

        _length_index.push_back(current_length + _lengths.back());

    }
    void parsePath(const string& pathData) {
        // Regular expression for parsing path segments.  Handles various coordinate formats
        regex pathRegex(R"(([MmCcLlHhVvAaQqTtSsZz])\s*((?:[-+]?(?:\d*\.\d+|\d+)(?:[eE][-+]?\d+)?)(?:[, ]*[-+]?(?:\d*\.\d+|\d+)(?:[eE][-+]?\d+)?)*))"); // Improved regex

        sregex_iterator it(pathData.begin(), pathData.end(), pathRegex);
        sregex_iterator end;

        Point pen = _from;
        Point dir = Point{0,0};

        while (it != end) {
            smatch match = *it;
            char type = match[1].str()[0];
            string coordsStr = match[2].str();

            vector<double> coords;
            regex coordRegex(R"([-+]?(?:\d*\.\d+|\d+)(?:[eE][-+]?\d+)?)"); // regex for individual coords
            sregex_iterator coordIt(coordsStr.begin(), coordsStr.end(), coordRegex);
            sregex_iterator coordEnd;

            while (coordIt != coordEnd) {
                coords.push_back(stod(coordIt->str()));
                ++coordIt;
            }

            append_segments(type, coords, pen, dir);
            _to = pen;

            ++it;
        }
    }


    vector<unique_ptr<Drawable>> _segments;
    vector<double> _lengths;
    vector<double> _length_index;
    Point _from, _to;
};



#endif