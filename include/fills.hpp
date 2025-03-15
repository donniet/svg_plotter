#ifndef __FILLS_HPP__
#define __FILLS_HPP__

#include "path.hpp"
#include "point.hpp"
#include "drawable.hpp"
#include "cover.hpp"

#include <vector>

using std::pair;
using std::vector;



class Lines :
    public Drawable
{
protected:
    // given
    Point _origin;
    Vector _direction;
    BoundingBox _bbox;
    double _separation;

    Vector _separation_direction;

    vector<Segment> _segments;
    vector<double> _line_lengths;
    vector<double> _length_index;

private:
    static int find_minimum_index(BoundingBox const &, 
                                     Point const &, Vector const &);
    static std::pair<double,double> intersect(Line const &, BoundingBox const &);

    void add_segment(Segment && s);

    void init();
public:
    virtual double length(double t0, double t1) const override;
    virtual Event at(double t) const override;

    virtual pair<bool, double> last_move_between(double t0, double t1) const;

    Lines(BoundingBox bbox = BoundingBox(0, 0, 1, 1), 
          double separation = 1.,
          Vector direction = Vector(1, 0),
          Point origin = Point(0,0));
};

class Spiral : 
    public Drawable
{
protected:
    Point _center;
    BoundingBox _region;
    double _dradius;

    double _max_parameter;

    double find_max_parameter();
public:
    virtual Event at(double t) const override;

    Spiral(BoundingBox region, Point center, double dradius = 1.);
};


#endif