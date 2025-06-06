#include "parser.hpp"

#include <numbers>
#include <cmath>
#include <utility>
#include <numeric>
#include <algorithm>
#include <vector>
#include <regex>
#include <string>
#include <stdexcept>
#include <format>

// #include <format>
// #include <functional>
// #include <execution>
// #include <functional>

using std::numbers::pi;
using std::sin, std::cos, std::tan;
using std::tie, std::pair;
using std::min, std::max;
using std::lower_bound, std::upper_bound;
using std::vector;
using std::regex, std::sregex_iterator, std::smatch;
using std::string;
using std::logic_error;
using std::format;


Event MoveTo::at(double t) const
{
    Vector dp = _to - _from;
    dp = dp.normalized();

    return Event(_to, dp, 0.); // 0 means a move_to
}

std::pair<bool, double> MoveTo::last_move_between(double t0, double t1) const
{
    // TODO: should I check if [t0, t1] intersects [0,1]?
    return { true, t0 };
}
MoveTo::MoveTo(Point from, Point to) :
    _from(from), _to(to)
{  }

double LineTo::length(double t0, double t1) const
{
    return (at(t1) - at(t0)).norm();
}

// HACK: these are assuming the vector ahead of the point
//       but the move_to logic asssumes the derivative is
//       at the start point
Event LineTo::at(double t) const
{

    Point p = _from + t * (_to - _from);
    Vector v = _to - _from;

    return Event(p, v);
}

LineTo::LineTo(Point from, Point to) :
    _from(from), _to(to), _length((_to - _from).norm())
{  }


CubicBezier::CubicBezier(Point from, Point c1, Point c2, Point to) :
    DrawableFromFunction(),
    _from(from), _c1(c1), _c2(c2), _to(to)
{ 
    reset([&](double t) 
    {
        return (*this)(t);
    });
}

Point CubicBezier::operator()(double t) const
{
    return bezier_point(t, _from, _c1, _c2, _to);
}

QuadraticCurve::QuadraticCurve(Point from, Point c1, Point to) :
    DrawableFromFunction(),
    _from(from), _c1(c1), _to(to)
{ 
    reset([&](double t)
    {
        return (*this)(t);
    });
}

Point QuadraticCurve::operator()(double t) const
{
    return bezier_point(t, _from, _c1, _to);
}

void Arc::init() 
{
    // https://svgwg.org/svg2-draft/implnote.html#ArcImplementationNotes
    double sinA = sin(_angle );
    double cosA = cos(_angle );
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

Point Arc::operator()(double t) const
{
    // use the angle here
    // TODO: make this arclength compatible
    double a = _start_angle + t * _sweep_angle;

    Point p{ _radius.x * cos(a), _radius.y * sin(a) };

    double sinA = sin(_angle);
    double cosA = cos(_angle);

    return _center + Point{cosA * p.x - sinA * p.y, sinA * p.x + cosA * p.y};
}

Arc::Arc(Point from, Point radius, double angle, bool large_arc, bool sweep, Point to) :
    DrawableFromFunction(),
    _from(from), _to(to), _radius(radius), _angle(angle* pi / 180.), _large_arc(large_arc), _sweep(sweep)
{ 
    init();

    reset([&](double t)
    {
        return (*this)(t);
    });
}

SVGPath::SVGPath(const string& pathData, Point const & origin) :
    _from(origin), _to(origin), _box(origin, origin)
{
    parsePath(pathData);
}

double SVGPath::length(double t0, double t1) const
{
    double l0 = length_to(t0);
    double l1 = length_to(t1);
    
    return l1 - l0;
}

Event SVGPath::at(double t) const 
{
    if(_length_index.size() == 0)
        return _to;

    size_t gid;
    double u;

    tie(gid, u) = segment_by_parameter(t);
    
    return _segments[gid]->at(u);
}

BoundingBox SVGPath::bounding_box() const
{
    return _box;
}

vector<vector<Point>> SVGPath::line_segments() const
{
    vector<vector<Point>> ret;

    vector<Point> cur;
    cur.emplace_back(_from);

    for(auto const & d : _segments)
    {
        if(d->at(0) == cur.back()) 
        {
            cur.emplace_back(d->at(1));
        }
        else
        {
            if(cur.size() > 1)
                ret.emplace_back(cur);

            cur.clear();
            cur.emplace_back(d->at(0));
        }
    }

    ret.emplace_back(cur);
    return ret;
}

std::pair<bool, double> SVGPath::last_move_between(double t0, double t1) const
{
    size_t seg0, seg1;
    double _;

    tie(seg0, _) = segment_by_parameter(t0);
    tie(seg1, _) = segment_by_parameter(t1);

    if(seg1 >= _segments.size() && seg1 > 0)
        seg1--;

    for(; seg0 <= seg1; --seg1)
    {
        auto m = _segments[seg1]->last_move_between(0, 1);
        if(m.first) 
        {
            // calculate the parameter range of this segment
            auto r = parameter_range(seg1);
            
            // interpolate using m.second
            return { true, r.first + m.second * (r.second - r.first) };
        }

        if(seg1 == 0)
            break;
    }

    return { false, t1 };
}

pair<double, double> SVGPath::parameter_range(size_t i) const
{
    if(i >= _segments.size())
        return {1., 1.};

    double l0 = 0.;
    if(i > 0)
        l0 = _length_index[i-1];

    double l1 = _length_index[i];
    double l = length(0., 1.);

    return { l0 / l, l1 / l };
}

vector<vector<Point>> SVGPath::plot(size_t count, double t0, double t1) const
{
    vector<vector<Point>> ret;
    vector<Point> cur;

    double dt = (t1 - t0) / (double)count;

    for(size_t i = 0; i < count; i++)
    {
        double t = t0 + dt * (double)i;
        auto d = segment_by_parameter(t);

        if(i == 0)
        {
            cur.push_back(_segments[d.first]->at(d.second));
            continue;
        }

        double tp = t0 + dt * (double)(i - 1);
        auto dp = segment_by_parameter(tp);

        if(d.first != dp.first)
        {
            cur.push_back(_segments[dp.first]->at(1));

            for(; dp.first < d.first; dp.first++)
            {
                auto a = _segments[dp.first]->last_move_between(0., 1.);
                auto b = a;
                for(auto c = b; c.first; b = c, c = _segments[dp.first]->last_move_between(0., c.second))
                    continue;

                if(a.first && cur.size() > 0)
                {
                    Point p0 = _segments[dp.first]->at(0.);
                    Point p1 = _segments[dp.first]->at(b.second);
                    cur.push_back(p0);
                    if(p0 != p1)
                        cur.push_back(p1);
                }
            }
                    
            ret.emplace_back(std::move(cur));
            // cur.clear();
            cur.push_back(_segments[d.first]->at(0));
        }
    }

    ret.emplace_back(std::move(cur));
    return ret;
}

// assumes _segments.size() > 0
pair<size_t, double> SVGPath::segment_by_parameter(double t) const 
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

double SVGPath::length_to(double t) const 
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

void SVGPath::adjust_box(Point p)
{
    if(p.x < _box.p0.x)
        _box.p0.x = p.x;
    
    if(p.y < _box.p0.y)
        _box.p0.y = p.y;

    if(p.x > _box.p1.x)
        _box.p1.x = p.x;

    if(p.y > _box.p1.y)
        _box.p1.y = p.y;
}

void SVGPath::adjust_box(BoundingBox const & b)
{
    _box.p0.x = min(_box.p0.x, b.p0.x);
    _box.p0.y = min(_box.p0.y, b.p0.y);
    _box.p1.x = max(_box.p1.x, b.p1.x);
    _box.p1.y = max(_box.p1.y, b.p1.y);
}

void SVGPath::append_segments(string::value_type key, vector<double> const & coords, Point & pen, Vector & dir)
{
    auto upper_key = toupper(key);
    Drawable * seg;

    bool rel = (key != upper_key);

    double len = 0.;

    switch(upper_key) {
    case 'M':
        for(int i = 0; i < coords.size(); i += 2) {
            Point to{coords[i], coords[i+1]};
            if(rel) to += pen;

            adjust_box(to);

            seg = dynamic_cast<Drawable*>(new MoveTo(pen, to));

            _segments.emplace_back(seg);
            len = 0.;
            _lengths.push_back(len);

            pen = to;
        }
        dir = Vector{0,0};
        break;
    case 'L':
        for(int i = 0; i < coords.size(); i += 2) {
            Point to{coords[i], coords[i+1]};
            if(rel) to += pen;

            adjust_box(to);

            seg = dynamic_cast<Drawable*>(new LineTo(pen, to));
            _segments.emplace_back(seg);
            len = seg->length(0, 1);
            _lengths.push_back(len);

            pen = to;
        }
        dir = Vector{0,0};
        break;
    case 'H':
        for(int i = 0; i < coords.size(); i++) {
            Point to{coords[i], pen.y};
            if(rel) to.x += pen.x;

            adjust_box(to);

            seg = dynamic_cast<Drawable*>(new LineTo(pen, to));
            _segments.emplace_back(seg);
            len = seg->length(0, 1);
            _lengths.push_back(len);

            pen = to;
        }
        dir = Vector{0,0};
        break;
    case 'V':
        for(int i = 0; i < coords.size(); i++) {
            Point to{pen.x, coords[i]};
            if(rel) to.y += pen.y;

            adjust_box(to);

            seg = dynamic_cast<Drawable*>(new LineTo(pen, to));
            _segments.emplace_back(seg);
            len = seg->length(0, 1);
            _lengths.push_back(len);

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

            adjust_box(to);

            seg = dynamic_cast<Drawable*>(new Arc(pen, radius, angle, large_arc, clockwise, to));
            len = seg->length(0, 1);
            if(len != len)
                throw logic_error(format("the arc defined by \"{} {} {} {} {} {} {}\" is invalid.", radius.x, radius.y, angle, large_arc, clockwise, to.x, to.y));
            _segments.emplace_back(seg);
            _lengths.push_back(len);

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
            len = seg->length(0, 1);
            _lengths.push_back(len);

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
            len = seg->length(0, 1);
            _lengths.push_back(len);

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
            len = seg->length(0, 1);
            _lengths.push_back(len);

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
            len = seg->length(0, 1);
            _lengths.push_back(len);

            pen = p1;
            dir = p1 - pen - dir;
        }
        break;
    case 'Z':
        seg = dynamic_cast<Drawable*>(new LineTo(pen, _from));
        _segments.emplace_back(seg);
        len = seg->length(0, 1);
        _lengths.push_back(len);

        pen = _from;
        dir = Vector{0,0};
        break;
    }

    double current_length = 0;
    if(_length_index.size() > 0)
        current_length = _length_index.back();

    _length_index.push_back(current_length + _lengths.back());

}
void SVGPath::parsePath(const string& pathData) {
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