#ifndef __STL_HPP__
#define __STL_HPP__

#include "output.hpp"

#include "mesh.hpp"

#include <utility>
#include <string>

using std::tie;
using std::string;

class STLOutput : 
    public Outputer
{
private:
    vector<Point> const & _strip;
    string _name;
public:
    

    STLOutput(vector<Point> const & strip, string const & name = "triangle_strip") : 
        _strip(strip), _name(name)
    { }

    virtual void print(ostream & os) const
    {
        os << "solid " << _name << "\n\n";

        // output the vertices
        for(size_t i = 2; i < _strip.size(); ++i)
        {
            os << "facet normal 0 0 1\n";
            os << "\touter loop\n";
            for(size_t j = 0; j < 3; ++j) 
            {
                // even implies counter-clockwise == decreasing
                // odd  implies counter-clockwise == increasing
                size_t k = (i % 2 == 0 ? i - j : i - (2 - j));

                Point p = _strip[k];

                os << "\t\tvertex " << p.x << " " << p.y << " 0\n";
            }
            os << "\tendloop\n";
            os << "endfacet\n";
        }

        os << "endsolid\n";        
    }
};


#endif