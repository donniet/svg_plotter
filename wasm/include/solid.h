#ifndef __SOLID_H__
#define __SOLID_H__

#define GL_GLEXT_LEGACY
#include <GL/gl.h>

#include "mesh.h"

#define MAX_UNIFORMS 10
#define MAX_DEFINES 10

typedef struct
{
    const char * name;
    uint index;
    const char * shader;
    Define defines[MAX_DEFINES];
    uint define_count;
    DrawMode mode;
    IndexRange vertex_range;
    TimeRange time_range;   
    Uniform uniforms[MAX_UNIFORMS];
    uint uniform_count;
} Solid;

Solid solids[];
const uint solid_count;

Attribute solid_attributes[];
const uint solid_attribute_count;

float const solid_buffer[];
const uint solid_buffer_size;
const uint solid_stride;

extern float get_solid_count()
{
    return solid_count;
}

extern const char * get_solid_name_ptr(uint i)
{
    return solids[i].name;
}
extern uint get_solid_name_length(uint i)
{
    return string_length(solids[i].name);
}
extern const char * get_solid_shader_ptr(uint i)
{
    return solids[i].shader;
}
extern uint get_solid_shader_length(uint i)
{
    return string_length(solids[i].shader);
}
extern uint get_solid_vertex_start(uint i)
{
    return solids[i].vertex_range.start;
}
extern uint get_solid_vertex_end(uint i)
{
    return solids[i].vertex_range.end;
}
extern uint get_solid_time_start(uint i)
{
    return solids[i].time_range.start;
}
extern uint get_solid_time_end(uint i)
{
    return solids[i].time_range.end;
}
extern uint get_solid_time_duration(uint i)
{
    return solids[i].time_range.start - solids[i].time_range.end;
}
extern uint get_solid_vertex_count(uint i)
{
    return solids[i].vertex_range.end - solids[i].vertex_range.start;
}
extern GLuint get_solid_draw_mode(uint i)
{
    return solids[i].mode;
}
extern GLuint get_solid_uniform_count(uint i)
{
    return solids[i].uniform_count;
}

extern uint get_solid_define_count(uint i)
{
    return solids[i].define_count;
}
extern const char * get_solid_define_name_ptr(uint i, uint j)
{
    return solids[i].defines[j].name;
}
extern uint get_solid_define_name_length(uint i, uint j)
{
    return string_length(solids[i].defines[j].name);
}
extern const char * get_solid_define_value_ptr(uint i, uint j)
{
    return solids[i].defines[j].value;
}
extern uint get_solid_define_value_length(uint i, uint j)
{
    return string_length(solids[i].defines[j].value);
}

extern const char * get_solid_uniform_name_ptr(uint i, uint j)
{
    return solids[i].uniforms[j].name;
}
extern uint get_solid_uniform_name_length(uint i, uint j)
{
    return string_length(solids[i].uniforms[j].name);
}
extern const char * get_solid_uniform_base_type_ptr(uint i, uint j)
{
    return solids[i].uniforms[j].base_type;
}
extern uint get_solid_uniform_base_type_length(uint i, uint j)
{
    return string_length(solids[i].uniforms[j].base_type);
}
extern float const * get_solid_uniform_data_ptr(uint i, uint j)
{
    return solids[i].uniforms[j].data;
}
extern int const * get_solid_uniform_idata_ptr(uint i, uint j)
{
    return solids[i].uniforms[j].idata;
}
extern uint get_solid_uniform_data_length(uint i, uint j)
{
    return solids[i].uniforms[j].data_length; // float size
}
extern uint get_solid_stride()
{
    return solid_stride;
}
extern uint get_solid_buffer_size()
{
    return solid_buffer_size;
}
extern float const * get_solid_buffer()
{
    return solid_buffer;
}

#endif