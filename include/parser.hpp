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

using namespace std;

struct PathSegment;



struct MoveTo :
    public Drawable
{
protected:
    Point _from, _to;
public:
    // length is 0, no need to override

    virtual Point at(double t) const override 
    {
        return _to;
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
public:
    virtual double length() const override
    {
        return (_to - _from).norm();
    }
    
    virtual Point at(double t) const override 
    {
        double len = length();
        return _from + t * (_to - _from);
    }

    LineTo(Point from, Point to) :
        _from(from), _to(to)
    {  }
};


struct CubicBezier : 
    public LineTo
{
protected:
    Point _c1, _c2;
public:
    virtual double length() const override
    {
        static constexpr size_t subdivide = 1e5;

        vector<double> segment_lengths(subdivide);
        vector<Point> end_points(subdivide + 1);

        end_points[0] = _from;
        for_each(execution::par_unseq, end_points.begin() + 1, end_points.end(), [&](Point & p) {
            size_t gid = &p - &end_points[0];
            double s = (double)gid / (double)subdivide;
            p = at(s);
        });

        return ::path_length(end_points.begin(), end_points.end(), false);
    }
    virtual Point at(double s) const override
    {
        return bezier_point(s, _from, _c1, _c2, _to);
    }

    CubicBezier(Point from, Point c1, Point c2, Point to) :
        LineTo(from, to), _c1(c1), _c2(c2)
    { }
};

struct QuadraticCurve :
    public LineTo
{
protected:
    Point _c1;
public:
    virtual double length() const override
    {
        static constexpr size_t subdivide = 1e5;

        vector<Point> end_points(subdivide + 1);

        end_points[0] = _from;
        for_each(execution::par_unseq, end_points.begin() + 1, end_points.end(), [&](Point & p) {
            size_t gid = &p - &end_points[0];
            double s = (double)gid / (double)subdivide;
            p = at(s);
        });

        return ::path_length(end_points.begin(), end_points.end(), false);
    }

    virtual Point at(double t) const override 
    {
        return bezier_point(t, _from, _c1, _to);
    }


    QuadraticCurve(Point from, Point c1, Point to) :
        LineTo(from, to), _c1(c1)
    { }
};



struct Arc :
    public LineTo
{
protected:
    Point _radius;
    double _angle;
    bool _large_arc;
    bool _sweep;

    Point _center;
    double _start_angle;
    double _sweep_angle;

    double _length;

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

        _length = calculate_length();
    }
public:
    virtual Point at(double t) const override
    {
        // use the angle here
        // TODO: make this arclength compatible
        double a = _start_angle + t * _sweep_angle;

        Point p{_radius.x * cos(a), _radius.y * sin(a)};

        double sinA = sin(_angle);
        double cosA = cos(_angle);

        return _center + Point{cosA * p.x - sinA * p.y, sinA * p.x + cosA * p.y};
    }
    virtual double length() const override {
        return _length;
    }

    Arc(Point from, Point radius, double angle, bool large_arc, bool sweep, Point to) :
        LineTo(from, to), _radius(radius), _angle(angle), _large_arc(large_arc), _sweep(sweep)
    { 
        init();
    }

protected:
    double calculate_length() const
    {
        // HACK: get this constant outta here
        static constexpr size_t subdivide = 1e5;
        vector<Point> end_points(subdivide + 1);

        end_points[0] = _from;
        for_each(execution::par_unseq, end_points.begin() + 1, end_points.end(), [&](Point & p) {
            size_t gid = &p - &end_points[0];
            double s = (double)gid / (double)subdivide;
            p = at(s);
        });

        return ::path_length(end_points.begin(), end_points.end(), false);
    }

};



class SVGPath :
    public LineTo
{
public:

    SVGPath(const string& pathData, Point const & origin = Point{0,0}) :
        LineTo(origin, origin)
    {
        parsePath(pathData);
    }

    virtual double length() const override 
    {
        if(_lengths.size() == 0)
            return 0.;

        return _lengths.back();
    }

    virtual Point at(double t) const override 
    {
        double len = length();
        double s = len * t;

        auto low = lower_bound(_lengths.begin(), _lengths.end(), s);

        if(low == _lengths.end())
            return _to;

        size_t low_index = &*low - &_lengths[0];

        if(low_index > 0)
            s -= _lengths[low_index-1];

        auto & seg = _segments[low_index];
        return seg->at(s / seg->length());
    }

private:
    void append_segments(string::value_type key, vector<double> const & coords, Point const & begin, Point & pen, Point & dir)
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
            dir = Point{0,0};
            break;
        case 'L':
            for(int i = 0; i < coords.size(); i += 2) {
                Point to{coords[i], coords[i+1]};
                if(rel) to += pen;

                seg = dynamic_cast<Drawable*>(new LineTo(pen, to));
                _segments.emplace_back(seg);
                _lengths.push_back(seg->length());

                pen = to;
            }
            dir = Point{0,0};
            break;
        case 'H':
            for(int i = 0; i < coords.size(); i++) {
                Point to{coords[i], pen.y};
                if(rel) to.x += pen.x;

                seg = dynamic_cast<Drawable*>(new LineTo(pen, to));
                _segments.emplace_back(seg);
                _lengths.push_back(seg->length());

                pen = to;
            }
            dir = Point{0,0};
            break;
        case 'V':
            for(int i = 0; i < coords.size(); i++) {
                Point to{pen.x, coords[i]};
                if(rel) to.y += pen.y;

                seg = dynamic_cast<Drawable*>(new LineTo(pen, to));
                _segments.emplace_back(seg);
                _lengths.push_back(seg->length());

                pen = to;
            }
            dir = Point{0,0};
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
                _lengths.push_back(seg->length());

                pen = to;
            }
            dir = Point{0,0};
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
                _lengths.push_back(seg->length());

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
                _lengths.push_back(seg->length());

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
                _lengths.push_back(seg->length());

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
                _lengths.push_back(seg->length());

                pen = p1;
                dir = p1 - pen - dir;
            }
            break;
        case 'Z':
            seg = dynamic_cast<Drawable*>(new LineTo(pen, _from));
            _segments.emplace_back(seg);
            _lengths.push_back(seg->length());

            pen = _from;
            dir = Point{0,0};
            break;
        }
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

            append_segments(type, coords, _origin, pen, dir);
            _to = pen;

            ++it;
        }

        inclusive_scan(execution::par_unseq, _lengths.begin(), _lengths.end(), _lengths.begin());
    }


    vector<unique_ptr<Drawable>> _segments;
    vector<double> _lengths;
    Point _origin;
};



#endif