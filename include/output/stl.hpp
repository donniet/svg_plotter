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
    vector<Point> const * _strip;
    vector<Triangle> const * _triangles;
    string _name;
public:

    STLOutput(vector<Triangle> const & triangles, string const & name = "triangles") :
        _strip(nullptr), _triangles(&triangles), _name(name)
    { }
    

    STLOutput(vector<Point> const & strip, string const & name = "triangle_strip") : 
        _strip(&strip), _triangles(nullptr), _name(name)
    { }

    virtual void print(ostream & os) const
    {
        os << "solid " << _name << "\n\n";

        if(_strip != nullptr)
            print_strip(os);

        else if(_triangles != nullptr)
            print_triangles(os);

        os << "endsolid\n"; 
    }

    virtual void print_strip(ostream & os) const
    {
        // output the vertices
        for(size_t i = 2; i < _strip->size(); ++i)
        {
            os << "facet normal 0 0 1\n";
            os << "\touter loop\n";
            for(size_t j = 0; j < 3; ++j) 
            {
                // even implies counter-clockwise == decreasing
                // odd  implies counter-clockwise == increasing
                size_t k = (i % 2 == 0 ? i - j : i - (2 - j));

                Point p = (*_strip)[k];

                os << "\t\tvertex " << p.x << " " << p.y << " 0\n";
            }
            os << "\tendloop\n";
            os << "endfacet\n";
        }
    }

    virtual void print_triangles(ostream & os) const
    {
        // output the vertices
        for(size_t i = 0; i < _triangles->size(); ++i)
        {
            os << "facet normal 0 0 1\n";
            os << "\touter loop\n";

            Triangle const & t = (*_triangles)[i];

            os << "\t\tvertex " << t.p0.x << " " << t.p0.y << " 0\n";
            os << "\t\tvertex " << t.p1.x << " " << t.p1.y << " 0\n";
            os << "\t\tvertex " << t.p2.x << " " << t.p2.y << " 0\n";

            os << "\tendloop\n";
            os << "endfacet\n";
        }      
    }
};


#endif