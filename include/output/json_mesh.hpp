#ifndef __JSON_MESH_HPP__
#define __JSON_MESH_HPP__

#include "output.hpp"

#include "mesh.hpp"

class JSONMeshOutput :
    public Outputer
{
private:
    TriangleStrip const & _strip;
public:
    JSONMeshOutput(TriangleStrip const & strip) : _strip(strip) { }

    virtual void print(ostream & os) const 
    {
        os << "{\n"
           << "\t\"draw_mode\": \"triangle_strip\",\n"
           << "\t\"vertex_count\": " << _strip.size() << ",\n"
           << "\t\"buffer_data\": [\n";
        for(size_t i = 0; i < _strip.size(); ++i)
        {
            Point p = _strip[i].first;
            Point u = _strip[i].second;

            os << "\t\t" << p.x << ", " << p.y << ", " << u.x << ", " << u.y << ",\n";
        }
        os << "\t]\n"
           << "}\n";
    }
};


#endif