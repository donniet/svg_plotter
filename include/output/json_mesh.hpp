#ifndef __JSON_MESH_HPP__
#define __JSON_MESH_HPP__

#include "output.hpp"

#include "mesh.hpp"
#include "attribute_mesh.hpp"

#include <string>

using std::string;

template<typename ... Attrs>
class JSONAttributeMeshOutput :
    public Outputer
{
private:
    AttributeMesh<Attrs...> const * _mesh;
public:
    JSONAttributeMeshOutput() : _mesh(nullptr) { }
    JSONAttributeMeshOutput(AttributeMesh<Attrs...> const & m) : _mesh(&m) { }
    JSONAttributeMeshOutput(JSONAttributeMeshOutput &&) = default;
    JSONAttributeMeshOutput(JSONAttributeMeshOutput const &) = default;

    JSONAttributeMeshOutput & operator=(JSONAttributeMeshOutput &&) = default;
    JSONAttributeMeshOutput & operator=(JSONAttributeMeshOutput const &) = default;

    virtual void print(std::ostream & os) const override
    {
        string indent = "  ";

        auto ts = [&](int count) -> string
        {
            string ret = "";
            for(int i = 0; i < count; i++)
                ret += indent;
            return ret;
        };

        os  << "{\n"
            << ts(1) << "\"draw_mode\": \"triangle_strip\",\n"
            << ts(1) << "\"vertex_count\": " << _mesh->size() << ",\n"
            << ts(1) << "\"attributes\": {\n"
            ;

        for(auto a : _mesh->attributes())
        {  
            os  << ts(2) << "\"" << a.name() << "\": [ " << a.size() << ", " << _mesh->stride << ", " << a.offset() << " ],\n";
        }
    
        os  << ts(1) << "},\n"
            << ts(1) << "\"buffer_data\": ["
            ;
     
        size_t c = 0;
        for(auto i = _mesh->buffer_begin(); i != _mesh->buffer_end(); ++i, ++c)
        {
            if(c % _mesh->stride == 0)
                os << "\n" << ts(2);

            os << *i << ", ";
        }

        os  << "\n" 
            << ts(1) << "],\n"
            << ts(1) << "\"stroke_range\": [0,1],\n"
            << ts(1) << "\"brush_color\": [1,0,0,1],\n"
            << "}"
            ;
    }

};

class JSONMeshOutput :
    public Outputer
{
private:
    TriangleStrip const & _strip;
public:
    JSONMeshOutput(TriangleStrip const & strip) : _strip(strip) { }

    virtual void print(ostream & os) const 
    {
        string indent = "  ";

        auto ts = [&indent](int count) -> string
        {
            string ret = "";
            for(int i = 0; i < count; i++)
                ret += indent;
            return ret;
        };


        os << "{\n"
           << ts(1) << "\"draw_mode\": \"triangle_strip\",\n"
           << ts(1) << "\"vertex_count\": " << _strip.size() << ",\n"
           << ts(1) << "\"attributes\": {\n"
           << ts(2) << "\"a_position\": [ 2, 7, 0 ],\n"
           << ts(2) << "\"a_uv\": [2, 7, 2],\n"
           << ts(2) << "\"a_arclength\": [1, 7, 4],\n"
           << ts(2) << "\"a_brush\": [2, 7, 5]\n"
           << ts(1) << "},\n"
           << ts(1) << "\"buffer_data\": [\n"
           ;
        
        for(size_t i = 0; i < _strip.size(); ++i)
        {
            Point p = get<0>(_strip[i]);
            Point u = get<1>(_strip[i]);
            float s = get<2>(_strip[i]);
            Point b = get<3>(_strip[i]);

            os << ts(2) << p.x << ", " << p.y << ", "
                        << u.x << ", " << u.y << ", "
                        << s << ", "
                        << b.x << ", " << b.y << ",\n"
                ;
        }

        os << ts(1) << "],\n"
           << ts(1) << "\"stroke_range\": [0,1],\n"
           << ts(1) << "\"brush_color\": [1,0,0,1],\n"
           << "}"
           ;
    }
};


#endif