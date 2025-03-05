// path.cpp (Implementation file)

#include "path.hpp"
#include <algorithm>
#include <execution>

using std::vector;
using std::distance;
using std::max;
using std::min;

void Path::init() {
    if (_points.size() < 2)
        return;

    using std::execution::par_unseq;

    _length_index.resize(_points.size());

    std::transform(par_unseq,
                   _points.begin(), _points.end(),
                   _length_index.begin(),
                   [&](Point & p) -> double {
                       size_t gid = distance(&_points[0], &p);

                       if (gid == 0)
                           return 0;

                       return (p - _points[gid - 1]).norm();
                   });

    std::inclusive_scan(par_unseq,
                        _length_index.begin(), _length_index.end(),
                        _length_index.begin());
}

double Path::length(double t0, double t1) const {
    if (_length_index.size() == 0)
        return 0;

    return _length_index.back() * (min(1., t1) - max(0., t0));
}

Event Path::at(double t) const {
    if (_points.size() == 0)
        return Point{};

    if (_points.size() == 1)
        return _points[0];

    if (t <= 0)
        return Event{_points.front(), _points[1] - _points[0], 0};

    if (t >= 1)
        return Event{_points.back(), _points.back() - _points[_points.size() - 2], 1.};

    double s = t * _length_index.back();

    auto i = std::lower_bound(_length_index.begin(), _length_index.end(), s);

    size_t gid = distance(_length_index.begin(), i);

    Vector dp = _points[gid] - _points[gid - 1];

    s = (s - _length_index[gid - 1]) /
        (_length_index[gid] - _length_index[gid - 1]);

    return Event(_points[gid - 1] + s * dp, dp);
}

Path::Path() : _points(0) { };

template<typename Iter>
Path::Path(Iter begin, Iter end) :
        _points(begin, end) {
    init();
}

Path::Path(vector<Point> const & p) :
        _points(p) {
    init();
}

Path::Path(vector<Point> && p) :
        _points(move(p)) {
    init();
}