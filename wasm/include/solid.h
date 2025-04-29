#ifndef __SOLID_H__
#define __SOLID_H__

#define GL_GLEXT_LEGACY
#include <GL/gl.h>

#include "mesh.h"

typedef struct
{
    const char * name;
    const char * shader;
    DrawMode mode;
    IndexRange vertex_range;
    TimeRange time_range;
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