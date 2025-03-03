#ifndef __OBJ_HPP__
#define __OBJ_HPP__

#include "output.hpp"


#include "mesh.hpp"

#include <utility>
#include <string>

using std::tie;
using std::string;

class OBJOutput : 
    public Outputer
{
private:
    TriangleStrip const & _strip;
    string _name;

public:
    OBJOutput(TriangleStrip const & strip, string const & name = "triangle_strip") : 
        _strip(strip), _name(name)
    { }

    virtual void print(ostream & os) const
    {
        os << "# OBJOutput: " << _name << "\n";

        for(size_t i = 0; i < _strip.size(); ++i) 
        {
            Point v = _strip[i].first;
            Point u = _strip[i].second;

            os << "v  " << v.x << " " << v.y << " 0\n";
            os << "vt " << u.x << " " << u.y << "\n";
            os << "vn 0 0 1\n";
        }

        os << "\n";

        for(size_t i = 2; i < _strip.size(); ++i)
        {
            os << "f ";
            for(size_t j = 0; j < 3; ++j)
            {
                size_t k = (i % 2 == 0 ? i - j : i - (2 - j));
                os << k + 1 << " ";
            }
            os << "\n";
        }
    }
};


#endif