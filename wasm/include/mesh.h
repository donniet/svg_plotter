#ifndef __MESH_H__
#define __MESH_H__

typedef unsigned int uint;

typedef enum {
    BrushRound = 0
} BrushStyle;

typedef struct
{
    float r, g, b, a;
} RGBA;

typedef struct 
{
    float start, end;
} TimeRange;

typedef enum {
    TRIANGLE_STRIP = 5
} DrawMode;

typedef struct
{
    uint start, end;
} IndexRange;

typedef struct
{
    const char * name;
    BrushStyle brush_style;
    float brush_size;
    RGBA brush_color;
    TimeRange time_range;
    DrawMode draw_mode;
    float arclength;
    IndexRange vertex_range;
    IndexRange section_range;
} Stroke;

typedef struct
{
    const char * name;
    uint size;
    uint offset;
} Attribute;

typedef struct
{
    const char * name;
    uint size;
    const float * data;
} Uniform;

// extern char buffer[255];

/**
 * These will be filled in by the output of stroke_mesh
 */
Stroke const _strokes[];
uint _stroke_count;
Attribute const _attributes[];
uint _attribute_count;
Uniform const _uniforms[];
uint _uniform_count;
float const _buffer[];
uint _buffer_size;
uint _stride;

uint _section_end_vertex[];
float _section_cumulative_arclength[];

float _drawing_width;
float _drawing_height;

float _draw_time;
float _clear_color[4];

uint string_length(const char * s)
{
    uint l = 0;
    while(s[l] != '\0')
        l++;

    return l;
}

extern float drawing_width()
{
    return _drawing_width;
}

extern float drawing_height()
{
    return _drawing_height;
}

extern float draw_time()
{
    return _draw_time;
}

extern const float * clear_color_ptr()
{
    return _clear_color;
}
extern uint clear_color_length()
{
    return 4;
}

extern uint stroke_count()
{
    return _stroke_count;
}

extern const char * stroke_name_ptr(uint i)
{
    return _strokes[i].name;
}

extern uint stroke_name_length(uint i)
{
    return string_length(_strokes[i].name);
}

extern uint stroke_vertex_start(uint i)
{
    return _strokes[i].vertex_range.start;
}

extern uint stroke_vertex_end(uint i)
{
    return _strokes[i].vertex_range.end;
}

extern uint stroke_section_count(uint i)
{
    return _strokes[i].section_range.end - _strokes[i].section_range.start;
}

extern BrushStyle stroke_brush_style(uint i)
{
    return _strokes[i].brush_style;
}

extern float stroke_brush_size(uint i)
{
    return _strokes[i].brush_size;
}

extern const float * stroke_brush_color_ptr(uint i)
{
    return &_strokes[i].brush_color.r;
}
extern uint stroke_brush_color_length(uint i)
{
    return 4;
}

extern float stroke_brush_color_r(uint i)
{
    return _strokes[i].brush_color.r;
}
extern float stroke_brush_color_g(uint i)
{
    return _strokes[i].brush_color.g;
}
extern float stroke_brush_color_b(uint i)
{
    return _strokes[i].brush_color.b;
}
extern float stroke_brush_color_a(uint i)
{
    return _strokes[i].brush_color.a;
}

extern float stroke_time_start(uint i)
{
    return _strokes[i].time_range.start;
}
extern float stroke_time_end(uint i)
{
    return _strokes[i].time_range.end;
}

extern DrawMode stroke_draw_mode(uint i)
{
    return _strokes[i].draw_mode;
}

extern float stroke_arclength(uint i)
{
    return _strokes[i].arclength;
}

extern float stroke_section_start(uint i)
{
    return _strokes[i].section_range.start;
}
extern float stroke_section_end(uint i)
{
    return _strokes[i].section_range.end;
}

extern uint stroke_section_end_vertex(
    uint stroke_index, uint section_index)
{
    return _section_end_vertex[
        _strokes[stroke_index].section_range.start + section_index
    ];
}

extern float stroke_section_cumulative_arclength(
    uint stroke_index, uint section_index)
{
    return _section_cumulative_arclength[
        _strokes[stroke_index].section_range.start + section_index
    ];
}

extern uint attribute_count()
{
    return _attribute_count;
}
extern const char * attribute_name_ptr(uint i)
{
    return _attributes[i].name;
}
extern uint attribute_name_length(uint i)
{
    const char * s = attribute_name_ptr(i);
    
    return string_length(s);
}
extern uint attribute_size(uint i)
{
    return _attributes[i].size;
}
extern uint attribute_offset(uint i)
{
    return _attributes[i].offset * 4;
}

extern uint uniform_count()
{
    return _uniform_count;
}

extern const char * uniform_name_ptr(uint i)
{
    return _uniforms[i].name;
}
extern uint uniform_name_length(uint i)
{
    const char * s = uniform_name_ptr(i);
    
    return string_length(s);
}

extern uint uniform_size(uint i)
{
    return _uniforms[i].size;
}
extern const float * uniform_data(uint i)
{
    return _uniforms[i].data;
}

extern uint stride()
{
    return _stride * 4;
}

extern uint buffer_size()
{
    return _buffer_size;
}
extern float const * buffer()
{
    return _buffer;
}

#endif