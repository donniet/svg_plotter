#ifndef __SVG_PATH_HPP__
#define __SVG_PATH_HPP__


#include <string>
#include <istream>
#include <iterator>
#include <limits>
#include <array>
#include <memory>
#include <cmath>
#include <utility>
#include <vector>
#include <algorithm>
#include <execution>

class PathVisitor
{
public:
    virtual void begin(double, double) = 0;
    virtual void move(double, double, double, double) = 0; 
    virtual void line(double, double, double, double) = 0;
    virtual void arc(double, double, double, double, double, bool, bool, double, double) = 0;
    virtual void bezier(double, double, double, double, double, double, double, double) = 0;
    virtual void quadratic(double, double, double, double, double, double) = 0;
    virtual void end() = 0;
};

class PathParser : 
    public PathVisitor
{
private:
    typedef enum 
    {
        Unknown                 =  0x0000,

        Number                  =  0x0001,

        MoveTo                  =  0x0002,
        LineTo                  =  0x0004,
        Horizontal              =  0x0008,
        Vertical                =  0x000B,
        Arc                     =  0x0010,
        Bezier                  =  0x0020,
        Quadratic               =  0x0040,
        Close                   =  0x0080,
        Command                 =  0x00FF,

        Absolute                =  0x0100,
        Relative                =  0x0200,
        Relativity              =  0x0F00,

        Distinct                =  0x1000,
        Continuation            =  0x2000,
        Continuity              =  0xF000,

        EndOfFile               =  0xFFFF,
    } Part;


    struct ParseError : public std::logic_error
    { 
        ParseError(std::istream::pos_type pos, std::string const & msg);
    };

    static void eat_spaces_and_commas(std::istream &);
    static void eat_spaces(std::istream &);

    static Part part_from(char);
private:
    virtual void begin(double, double) override {};
    virtual void move(double, double, double, double) override {}; 
    virtual void line(double, double, double, double) override {};
    virtual void arc(double, double, double, double, double, bool, bool, double, double) override {};
    virtual void bezier(double, double, double, double, double, double, double, double) override {};
    virtual void quadratic(double, double, double, double, double, double) override {};
    virtual void end() override {};

private:
    struct Point { double x, y; };
    using Vector = Point;

    PathVisitor * _visitor;

    Part _current;
    
    Point _path_start;
    Point _pen;
    Vector _direction;

    Part expect_command(std::istream &);
    double expect_number(std::istream &);
    Part peek(std::istream &);

    Point pen();
    Vector direction();
    Point path_start_point();


    void begin_path(double, double);
    void line_to(double, double);
    void arc_to(double, double, double, bool, bool, double, double);
    void bezier_to(double, double, double, double, double, double);
    void quadratic_to(double, double, double, double);
    void end_path();
    

    void do_parse(std::istream &);

public:
    PathParser();
    PathParser(PathVisitor &);
    PathParser(PathParser const &) = default;
    PathParser(PathParser &&) = default;

    PathParser & operator=(PathParser const &) = default;
    PathParser & operator=(PathParser &&) = default;
    
    std::istream & parse(std::istream &);
    void parse(std::string const &);

};



template<typename Point>
class PathBezier
{
private:
    std::vector<Point> _controls;

    static Point interpolate(Point const & p0, Point const & p1, double t)
    {
        using std::get;

        return Point{
            p0[0] + t * (p1[0] - p0[0]),
            p0[1] + t * (p1[1] - p0[1])
        };
    }
public:
    PathBezier(std::initializer_list<Point> && points) : _controls(points) { }

    Point operator()(double t) const
    {
        std::vector<Point> last = _controls;
        std::vector<Point> next(0);

        for(size_t i = _controls.size() - 1; i >= 1; i--)
        {
            next.resize(i);

            for(size_t j = 0; j < last.size() - 1; j++)
                next[j] = interpolate(last[j], last[j+1], t);

            last = std::move(next);
        }

        return last[0];
    }
};

template<typename Point>
class PathArc 
{
private:
    double _rx;
    double _ry;
    Point _center;
    double _angle;
    double _start_angle;
    double _sweep_angle;

public:
    PathArc(double x0, double y0, double rx, double ry, double angle, bool large, bool sweep, double x, double y) 
        : _rx(rx), _ry(ry), _angle(angle)
    {
        using std::sin, std::cos, std::sqrt, std::acos;

        // https://svgwg.org/svg2-draft/implnote.html#ArcImplementationNotes
        double sinA = sin( angle );
        double cosA = cos( angle );

        Point xp =  { (x0 - x) * 0.5, (y0 - y) * 0.5 };

        Point xq {
             cosA * xp[0] + sinA * xp[1],
            -sinA * xp[0] + cosA * xp[1]
        };

        double s = sqrt(
            (rx * rx * ry * ry - rx * rx * xq[1] * xq[1] - ry * ry * xq[0] * xq[0]) / 
            (rx * rx * xq[1] * xq[1] + ry * ry * xq[0] * xq[0])
        );

        if(large == sweep)
            s *= -1;
        
        Point c1 = s * Point{rx * xq[1] / ry, -ry * xq[0] / rx};
        xp = { (x0 + x) * 0.5, (y0 + y) * 0.5 };

        _center = xp + Point{
            cosA * c1[0] - sinA * c1[1],
            sinA * c1[0] + cosA * c1[1]
        };

        Point s1{( xq[0] - c1[0]) / rx, ( xq[1] - c1[1]) / ry};
        Point s2{(-xq[0] - c1[0]) / rx, (-xq[1] - c1[1]) / ry};

        double s1n = sqrt(s1[0] * s1[0] + s1[1] * s1[1]);
        double s2n = sqrt(s2[0] * s2[0] + s2[1] * s2[1]);

        _start_angle = acos(s1[0] / s1n);
        _sweep_angle = acos((s1[0] * s2[0] + s1[1] * s2[1]) / s1n / s2n);
    }

    Point operator()(double t) const 
    {
        using std::sin, std::cos, std::sqrt, std::acos;

        // use the angle here
        // TODO: make this arclength compatible
        double a = _start_angle + t * _sweep_angle;

        Point p{ _rx * cos(a), _ry * sin(a) };

        double sinA = sin(_angle);
        double cosA = cos(_angle);

        return Point{ 
            _center[0] + cosA * p[0] - sinA * p[1], 
            _center[1] + sinA * p[0] + cosA * p[1] 
        };
    }
};

template<typename Point, typename Func>
std::vector<Point> plot_with_curvature_limit(Func && f, double max_curvature = 0.02, double t0 = 0., double t1 = 1., size_t maximum_points = 1UL << 15)
{
    using std::for_each;
    using std::transform;
    using std::max_element;

    using std::sqrt, std::abs;

    std::vector<Point> ret;
    std::vector<double> curv;

    for(size_t N = 2; N < maximum_points; N <<= 1)
    {
        ret.resize(N);
        for_each(ret.begin(), ret.end(), [&f, &ret, t0, t1, N](Point & p) 
        {
            size_t gid = std::distance(&ret[0], &p);

            p = f(t0 + (double)gid / (double)N * (t1 - t0));
        });
        
        curv.resize(N);
        curv[0] = 0.;
        transform(ret.begin() + 1, ret.end(), curv.begin() + 1, [ret](Point const & p) -> double
        {
            size_t gid = std::distance(&ret[0], &p);

            Point p0 = ret[gid - 1];

            double l0 = sqrt(p0[0] * p0[0] + p0[1] * p0[1]);
            double l = sqrt(p[0] * p[0] + p[1] * p[1]);
            
            // take the cross product
            return abs(p0[0] * p[1] - p0[1] * p[0]) / l0 / l;
        });

        auto i = max_element(curv.begin(), curv.end());

        if(*i <= max_curvature)
            break;
    }

    return ret;
}

template<typename Point>
class Plotter : 
    public PathVisitor
{
private:
    std::vector<std::vector<Point>> _plot;

public:
    virtual void begin(double x, double y) override
    {
        if(_plot.size() > 0 && _plot.back().size() == 1)
            _plot.pop_back();

        _plot.emplace_back();
        _plot.back().emplace_back(x, y);
    }
    virtual void move(double x0, double y0, double x, double y) override
    {
        if(_plot.size() > 0 && _plot.back().size() == 1)
            _plot.pop_back();

        _plot.emplace_back();
        _plot.back().emplace_back(x, y);
    }
    virtual void line(double x0, double y0, double x, double y) override
    {
        if(_plot.size() == 0)
        {
            _plot.emplace_back();
            _plot.back().emplace_back(x, y);
        }
        
        _plot.back().emplace_back(x, y);
    }
    virtual void arc(double x0, double y0, double rx, double ry, double angle, bool large, bool sweep, double x, double y) override
    {
        PathArc<Point> a(x0, y0, rx, ry, angle, large, sweep, x, y);

        if(_plot.size() == 0)
        {
            _plot.emplace_back();
            _plot.back().emplace_back(x, y);
        }
        
        auto points = plot_with_curvature_limit<Point>(a);
        _plot.back().insert(_plot.back().end(), points.cbegin(), points.cend());
    }
    virtual void bezier(double x0, double y0, double x1, double y1, double x2, double y2, double x, double y) override
    {
        if(_plot.size() == 0)
        {
            _plot.emplace_back();
            _plot.back().emplace_back(x, y);
        }

        PathBezier b{Point{x0,y0}, Point{x1, y1}, Point{x2, y2}, Point{x, y}};

        auto points = plot_with_curvature_limit<Point>(b);
        _plot.back().insert(_plot.back().end(), points.cbegin(), points.cend());
    }
    virtual void quadratic(double x0, double y0, double x1, double y1, double x, double y) override
    {
        if(_plot.size() == 0)
        {
            _plot.emplace_back();
            _plot.back().emplace_back(x, y);
        }

        PathBezier q{Point{x0, y0}, Point{x1,y1}, Point{x, y}};

        auto points = plot_with_curvature_limit<Point>(q);
        _plot.back().insert(_plot.back().end(), points.cbegin(), points.cend());
    }
    virtual void end() override
    {

    }

    std::vector<std::vector<Point>> plot() const
    {
        return _plot;
    }
};


#endif