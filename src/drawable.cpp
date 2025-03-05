// drawable.cpp (Implementation file)

#include "drawable.hpp"
#include "cover.hpp"
#include <algorithm>
#include <execution>
#include <stdexcept>

using std::vector;
using std::swap, std::tie;
using std::distance;
using std::numeric_limits;
using std::min, std::max;

double Drawable::length(double t0, double t1) const {
    return 0.;
}

Event Drawable::at(double t) const {
    return Event{};
}

BoundingBox Drawable::bounding_box() const {
    BoundingBox ret;

    for (double t = 0; t <= 1.; t += 1. / 1024.) {
        Point p = at(t);

        if (p.x < ret.p0.x)
            ret.p0.x = p.x;
        if (p.y < ret.p0.y)
            ret.p0.y = p.y;
        if (p.x > ret.p1.x)
            ret.p1.x = p.x;
        if (p.y > ret.p1.y)
            ret.p1.y = p.y;
    }

    return ret;
}

std::pair<bool, double> Drawable::last_move_between(double t0, double t1) const {
    if (abs(t1 - t0) <= numeric_limits<double>::epsilon())
        return {false, t1};

    if (length(t0, t1) >= (at(t1) - at(t0)).norm())
        return {false, t1};

    for (;;) {
        double tp = (t0 + t1) / 2.;

        if (abs(tp - t0) <= numeric_limits<double>::epsilon())
            return {true, tp};

        if (length(tp, t1) < (at(t1) - at(tp)).norm()) {
            t0 = tp;
            continue;
        }

        t1 = tp;
        continue;
    }

    throw std::logic_error("there must be a problem above if it got here!  it should only return from the loop");
}

DrawableFromFunction::DrawableFromFunction(std::function<Point(double)> f,
                                             std::pair<double, double> interval,
                                             size_t sample_count) :
        _f(f), _sample_count(sample_count), _interval(interval), _samples(sample_count),
        _lengths(sample_count), _length_index(sample_count) {
    init();
}

DrawableFromFunction::DrawableFromFunction(std::pair<double, double> interval,
                                             size_t sample_count) :
        _f(), _sample_count(sample_count), _interval(interval), _samples(0),
        _lengths(0), _length_index(0) {
}

void DrawableFromFunction::reset(std::function<Point(double)> f) {
    _f = f;
    _samples.resize(_sample_count);
    _lengths.resize(_sample_count);
    _length_index.resize(_sample_count);
    init();
}

Event DrawableFromFunction::at(double t) const {
    if (_samples.size() < 2)
        throw std::logic_error("uninitialized drawable function");

    size_t gid;
    double u;
    tie(gid, u) = segment_by_parameter(t);

    Point p0 = _samples[gid];
    Point p1 = p0;
    if (gid < _samples.size() - 1)
        p1 = _samples[gid + 1];

    Point p = p0 + u * (p1 - p0);
    double dt = (_interval.second - _interval.first) /
                ((double) _sample_count - 1);

    Vector v = (p1 - p0) / dt;
    if (gid == _samples.size() - 1)
        v = p0 - _samples[gid - 1];

    if (t == 0)
        return Event{p, v, 0.};

    return Event{p, v, 1.};
}

double DrawableFromFunction::length(double t0, double t1) const {
    if (_samples.size() < 2)
        throw std::logic_error("uninitialized drawable function");

    if (t0 == t1)
        return 0;

    return length_to(t1) - length_to(t0);
}

std::pair<size_t, double> DrawableFromFunction::segment_by_parameter(double t) const {
    t = max(_interval.first, min(t, _interval.second));
    t /= (_interval.second - _interval.first);

    if (t <= 0)
        return {0, 0};

    if (t >= 1)
        return {_length_index.size() - 1, 1};

    double len = _length_index.back();
    double s = len * t;

    size_t low_index = std::upper_bound(&_length_index[0], &_length_index[0] + _length_index.size(), s) -
                       &_length_index[0];

    if (low_index > 0)
        s -= _length_index[low_index - 1];

    return {low_index, s / _lengths[low_index]};
}

double DrawableFromFunction::length_to(double t) const {
    size_t gid;
    double u;

    tie(gid, u) = segment_by_parameter(t);
    Point p0 = _samples[gid];
    Point p1 = p0;
    if (gid < _samples.size() - 1)
        p1 = _samples[gid + 1];

    Point p = p0 + u * (p1 - p0);

    return _length_index[gid] + (p - p0).norm();
}

void DrawableFromFunction::init() {
    if (_sample_count < 2)
        throw std::logic_error("sample_count must be greater than 2");

    using std::execution::par_unseq;

    std::for_each(par_unseq, _samples.begin(), _samples.end(), [&](Event &e) {
        size_t gid = distance(&_samples[0], &e);
        double u = (double) gid / ((double) _sample_count - 1.);
        u = _interval.first + u * (_interval.second - _interval.first);

        e = _f(u);
    });

    std::transform(par_unseq, _samples.begin(), _samples.end(), _lengths.begin(), [&](Event &e) -> double {
        size_t gid = distance(&_samples[0], &e);
        if (gid == 0)
            return 0;

        return (_samples[gid] - _samples[gid - 1]).norm();
    });

    std::inclusive_scan(par_unseq, _lengths.begin(), _lengths.end(), _length_index.begin());
}