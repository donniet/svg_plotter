#ifndef __JSON_WRITER_HPP__
#define __JSON_WRITER_HPP__

/*

inspiration: 
    JSONWriter writer(os);

    auto root = writer.Object();
    root["draw_mode"] = (string)_mesh->draw_mode();
    root["vertex_count"] = _mesh->vertex_count();

    auto uniforms = root["uniforms"].Object();
    for(auto const & u : _uniforms)
        uniforms[u.name()] = u.value();

    size_t stride = calculate_stride();

    auto attributes = root["attributes"].Object();
    size_t offset = 0;

    attributes["a_position"].Array(
        2, stride, 0
    );

    for(auto const & a : _attributes)
    {
        auto arr = attributes[a.name()].Array(
            a.size(), stride, offset
        );

        offset += a.size();
    }
    
    auto buffer_data = root["buffer_data"].Array();
    for(size_t i = 0; i < _mesh->vertex_count(); ++i)
    {
        // get all the buffer data
    }

*/

// #include <iostream>
// #include <vector>
// #include <string>
// #include <map>

// class JSONWriterInterface
// {
// protected:

// public:
//     JSONWriterInterface Object();
//     JSONWriterInterface Array();

// };

// class JSONWriter
// {
// protected:
//     std::ostream * _o;
//     std::string _indent;

//     std::vector<State> _stack;

//     void write_indent();
// public:
//     JSONWriterInterface Object()
//     {
        
//     }
//     JSONWriterInterface Array();

//     JSONWriter() : _o(nullptr) { }
//     JSONWriter(std::ostream & os) : _o(&os) { }

//     JSONWriter(JSONWriter &&);
//     JSONWriter(JSONWriter const &);
// };


#endif