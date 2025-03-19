#ifndef __JSON_MESH_HPP__
#define __JSON_MESH_HPP__

#include "output.hpp"

#include "mesh.hpp"
#include "attribute_mesh.hpp"
#include "point.hpp"

#include <string>

using std::string;

template<typename ... Attrs>
class JSONAttributeMeshOutput :
    public Outputer
{
private:
    AttributeMesh<Attrs...> const * _mesh;
    
    std::pair<double, double> _canvas_size;
    RGBA _clear_color;
public:
    JSONAttributeMeshOutput(std::pair<double, double> canvas_size = {1., 1.}, RGBA clear_color = RGBA(1, 1, 1, 1)) : 
        _mesh(nullptr), _canvas_size(canvas_size), _clear_color(clear_color)
    { }
    JSONAttributeMeshOutput(std::pair<double,double> canvas_size, RGBA clear_color, AttributeMesh<Attrs...> const & m) : 
        _mesh(&m), _canvas_size(canvas_size), _clear_color(clear_color)
    { }
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
            << ts(1) << "\"canvas\": [" << _canvas_size.first << ", " << _canvas_size.second << "],\n"
            << ts(1) << "\"clear_color\": [" << _clear_color.r << ", " << _clear_color.g << ", " << _clear_color.b << ", " << _clear_color.a << "],\n"
            << ts(1) << "\"draw_mode\": \"triangle_strip\",\n"
            << ts(1) << "\"vertex_count\": " << _mesh->size() << ",\n"
            << ts(1) << "\"stride\": " << _mesh->stride() << ",\n"
            << ts(1) << "\"attributes\": {\n"
            ;

        for(auto a : _mesh->attributes())
        {  
            os  << ts(2) << "\"" << a.name() << "\": [ " << a.size() << ", " << a.offset() << " ],\n";
        }
    
        os  << ts(1) << "},\n"
            << ts(1) << "\"buffer_data\": ["
            ;
     
        size_t c = 0;
        for(auto i = _mesh->buffer_begin(); i != _mesh->buffer_end(); ++i, ++c)
        {
            if(c % _mesh->stride() == 0)
                os << "\n" << ts(2);

            os << *i << ", ";
        }

        os  << ts(1) << "],\n"
            << ts(1) << "\"layers\": {\n"
            << ts(2) << "\"background\": {\n"
            << ts(3) << "\"draw_mode\": \"triangle_strip\",\n"
            << ts(3) << "\"range\": [0, " << _mesh->size() << "],\n"
            << ts(3) << "\"arclength\": " << get<1>(_mesh->back()).y << ",\n"
            << ts(3) << "\"uniforms\": {\n"
            << ts(4) << "\"u_brush_color\": { \"3f\": [  0.03515625, 0.1796875, 0.1640625 ] },\n"
            << ts(4) << "\"u_brush_size\": { \"1f\": [ 10. ] },\n"
            << ts(3) << "},\n"
            << ts(3) << "\"section_end\": [\n"
            << ts(4)
            ;

        size_t sec = 0;
        for(size_t i = 0; i < _mesh->size(); ++i)
        {
            size_t n = get<3>((*_mesh)[i]);

            if(sec != n) 
            {
                os << i << ",";
                sec = n;
            }
        }

        os  << "\n"
            << ts(3) << "],\n"
            << ts(3) << "\"section_length\": [\n"
            << ts(4)
            ;

        sec = 0;
        for(size_t i = 0; i < _mesh->size(); ++i)
        {
            Point uv = get<1>((*_mesh)[i]);
            size_t n = get<3>((*_mesh)[i]);

            if(sec != n) 
            {
                os << uv.y << ",";
                sec = n;
            }
        }

        os  << "\n"
            << ts(3) << "],\n"
            << ts(2) << "},\n"
            << ts(1) << "},\n"
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