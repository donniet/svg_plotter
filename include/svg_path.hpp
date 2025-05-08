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
#include <cmath>

namespace detail 
{
    template<class T>
    concept Vector = requires(T a, T b, T c, double s, size_t i)
    {
        c = a;
        c = a + b;
        c = a - b;
        c = a * s;
        c = a / s;
        a == b;
        a != b;
        c += a;
        c -= a;

        s = a[i];
        i = a.size();
    };
}

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
    struct Point { 
        double x, y; 

        Point() = default;
        Point(Point const &) = default;
        Point(Point &&) = default;
        Point(double, double);

        Point & operator=(Point const &) = default;
        Point & operator=(Point &&) = default;
        

        // required interface for a point
        constexpr double & operator[](int i);
        constexpr double const & operator[](int i) const;
    };
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
    double _sinA;
    double _cosA;
    double _start_angle;
    double _sweep_angle;

public:
    PathArc(double x0, double y0, double rx, double ry, double angle, bool large, bool sweep, double x, double y) 
        : _rx(rx), _ry(ry), _angle(angle), _sinA(std::sin(angle)), _cosA(std::cos(angle))
    {
        using std::sin, std::cos, std::sqrt, std::acos;

        // https://svgwg.org/svg2-draft/implnote.html#ArcImplementationNotes

        Point xp =  { (x0 - x) * 0.5, (y0 - y) * 0.5 };

        Point xq {
             _cosA * xp[0] + _sinA * xp[1],
            -_sinA * xp[0] + _cosA * xp[1]
        };

        double s = sqrt(
            (rx * rx * ry * ry - rx * rx * xq[1] * xq[1] - ry * ry * xq[0] * xq[0]) / 
            (rx * rx * xq[1] * xq[1] + ry * ry * xq[0] * xq[0])
        );

        if(large == sweep)
            s *= -1;
        
        Point c1 = Point{rx * s * xq[1] / ry, -ry * s * xq[0] / rx};
        xp = { (x0 + x) * 0.5, (y0 + y) * 0.5 };

        _center = Point{
            xp[0] + _cosA * c1[0] - _sinA * c1[1],
            xp[1] + _sinA * c1[0] + _cosA * c1[1]
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

        Point p{ 
            _rx * cos(a), 
            _ry * sin(a) 
        };

        return Point{ 
            _center[0] + _cosA * p[0] - _sinA * p[1], 
            _center[1] + _sinA * p[0] + _cosA * p[1] 
        };
    }

    Point dv(double t) const 
    {
        using std::sin, std::cos;

        double a = _start_angle + t * _sweep_angle;

        Point p{
            -_rx * _sweep_angle * sin(a),
             _ry * _sweep_angle * cos(a),
        };

        return Point{
            _cosA * p[0] - _sinA * p[1],
            _sinA * p[0] + _cosA * p[1]
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

template<detail::Vector Point>
class Plot
{
private:
    std::vector<std::vector<Point>> _paths;
    std::vector<std::vector<double>> _lengths;

    static double norm2(Point const & a)
    {
        double l = 0.;
        for(size_t i = 0; i < a.size(); i++)
            l += a[i] * a[i];
        
        return l;
    }
    
    static double norm(Point const & a)
    {
        return std::sqrt(norm2(a));
    }

    static bool compare_fronts(std::vector<double> const & a, std::vector<double> const & b)
    {
        return a.front() < b.front();
    }
    static bool compare_backs(std::vector<double> const & a, std::vector<double> const & b)
    {
        return a.back() < b.back();
    }

public:
    using value_type = std::vector<Point>;
    using iterator = std::vector<std::vector<Point>>::iterator;
    using const_iterator = std::vector<std::vector<Point>>::const_iterator;

    iterator begin()
    {
        return _paths.begin();
    }
    iterator end()
    {
        return _paths.end();
    }
    const_iterator cbegin()
    {
        return _paths.cbegin();
    }
    const_iterator cend()
    {
        return _paths.cend();
    }
    const_iterator begin() const
    {
        return _paths.begin();
    }
    const_iterator end() const
    {
        return _paths.end();
    }
    size_t paths_size() const
    {
        return _paths.size();
    }

    void push_point(Point const & p)
    {
        double l = 0.;
        std::vector<Point> & path = _paths.back();
        std::vector<double> & len = _lengths.back();

        if(path.size() > 0)
        {
            // what is our distance from the last point?
            l = norm(path.back() - p);

            // if we are at the same point as the end of this path, don't add it
            // this ensures we never have duplicate sequential points
            if(l == 0.)
                return;

            // add the total length so to the length from the last point
            l += len.back();
        }
        else if(_lengths.size() > 1)
        {
            l = _lengths[_lengths.size() - 2].back();
        }

        len.emplace_back(l);
        path.emplace_back(p);
    }

    void begin_path(Point const & p)
    {
        // if our current path has only a single point, erase it before starting a new path
        if(_paths.size() > 0 && _paths.back().size() == 1)
            _paths.pop_back();
        
        _paths.emplace_back(1, p);
        _lengths.emplace_back(1, 0.);
    }

    double total_length() const
    {
        if(_lengths.size() > 0)
            return _lengths.back().back();

        return 0;
    }

    double length_of(size_t path) const
    {
        return _lengths[path].back() - _lengths[path].front();
    }

    size_t path_index_at_length(double s) const
    {
        // temporary vector used to compare to our _lengths
        std::vector<double> l(1, s);

        if(s <= 0) 
            return _paths.front();

        if(s >= total_length()) 
            return _paths.back();

        auto path = std::lower_bound(_lengths.begin(), _lengths.end(), 
                                     l, compare_fronts);

        // the minus 1 is due to lower_bound returning a pointer to a >= element
        // since we are comparing front we subtract one from our lower_bound 
        // to get the index of the path that crosses s in total length
        return std::distance(_lengths.begin(), path) - 1;
    }

    Point at_length(double s) const 
    {
        size_t path_index = path_index_at_length(s);

        std::vector<Point> const & path = _paths[path_index];
        std::vector<double> const & length = _lengths[path_index];

        if(s <= length.front())
            return path.front();

        if(s >= length.back())
            return path.back();

        auto p = std::lower_bound(length.begin(), length.end(), s);
      
        // index of the first point whose total length along our plot
        // is greater than or equal to s
        size_t point_index = std::distance(length.begin(), p);

        // interpolate
        double ds =                 s - path[point_index - 1];
        double us = path[point_index] - path[point_index - 1];

        return   path[point_index - 1] * (1. - ds / us) 
               + path[point_index    ] * (     ds / us);
    }

    Point at(double t) const 
    {
        if(t <= 0)
            return _paths.front().front();
            
        if(t >= 1)
            return _paths.back().back();

        double s = t * total_length();

        return at_length(s);
    }

    size_t moves_between(double t0 = 0., double t1 = 1.)
    {
        double s0 = t0 * total_length();
        double s1 = t1 * total_length();

        return path_index_at_length(s1) - path_index_at_length(s0);
    }

    std::vector<Point> const & operator[](size_t path) const
    {
        return _paths[path];
    }
};

template<typename Point>
class Plotter : 
    public PathVisitor
{
private:
    Plot<Point> _plot;
    Point _final;
    bool _add_final;

    double _curvature_limit;

public:
    Plotter(double max_curvature = 0.02) : 
        _add_final(false), _curvature_limit(max_curvature) 
    { }

    virtual void begin(double x, double y) override
    {
        _plot.begin_path(Point{x, y});
    }
    virtual void move(double x0, double y0, double x, double y) override
    {
        _plot.begin_path(Point{x, y});
    }
    virtual void line(double x0, double y0, double x, double y) override
    {
        _plot.push_point(Point{x, y});
    }
    virtual void arc(double x0, double y0, double rx, double ry, double angle, bool large, bool sweep, double x, double y) override
    {
        // no need to add final here since the start point will be included in the plot
        PathArc<Point> a(x0, y0, rx, ry, angle, large, sweep, x, y);

        auto points = plot_with_curvature_limit<Point>(a);
        std::for_each(points.begin(), points.end(), [this](Point const & p) {
            _plot.push_point(p);
        });
    }
    virtual void bezier(double x0, double y0, double x1, double y1, double x2, double y2, double x, double y) override
    {
        PathBezier b{Point{x0,y0}, Point{x1, y1}, Point{x2, y2}, Point{x, y}};

        auto points = plot_with_curvature_limit<Point>(b);
        std::for_each(points.begin(), points.end(), [this](Point const & p) {
            _plot.push_point(p);
        });
    }
    virtual void quadratic(double x0, double y0, double x1, double y1, double x, double y) override
    {
        PathBezier q{Point{x0, y0}, Point{x1,y1}, Point{x, y}};

        auto points = plot_with_curvature_limit<Point>(q);

        std::for_each(points.begin(), points.end(), [this](Point const & p) {
            _plot.push_point(p);
        });
    }
    virtual void end() override
    { }

    Plot<Point> const & plot() const
    {
        return _plot;
    }
};


#endif