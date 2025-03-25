#ifndef __MESH_H__
#define __MESH_H__

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
    unsigned int start, end;
} VertexRange;

typedef struct
{
    unsigned int start, end;
} SectionRange;

typedef struct
{
    BrushStyle brush_style;
    float brush_size;
    RGBA brush_color;
    TimeRange time_range;
    DrawMode draw_mode;
    float arclength;
    VertexRange vertex_range;
    SectionRange section_range;
} Stroke;

typedef struct
{
    const char * name;
    unsigned int size;
    unsigned int offset;
} Attribute;

typedef struct
{
    const char * name;
    unsigned int size;
    const float * data;
} Uniform;

// extern char buffer[255];

/**
 * These will be filled in by the output of stroke_mesh
 */
Stroke const _strokes[];
unsigned int _stroke_count;
Attribute const _attributes[];
unsigned int _attribute_count;
Uniform const _uniforms[];
unsigned int _uniform_count;
float const _buffer[];
unsigned int _buffer_size;

unsigned int string_length(const char * s)
{
    unsigned int l = 0;
    while(s[l] != '\0')
        l++;

    return l;
}

extern unsigned int stroke_count()
{
    return _stroke_count;
}

extern BrushStyle get_stroke_brush_style(unsigned int i)
{
    return _strokes[i].brush_style;
}

extern float get_stroke_brush_size(unsigned int i)
{
    return _strokes[i].brush_size;
}

extern float get_stroke_brush_color_r(unsigned int i)
{
    return _strokes[i].brush_color.r;
}
extern float get_stroke_brush_color_g(unsigned int i)
{
    return _strokes[i].brush_color.g;
}
extern float get_stroke_brush_color_b(unsigned int i)
{
    return _strokes[i].brush_color.b;
}
extern float get_stroke_brush_color_a(unsigned int i)
{
    return _strokes[i].brush_color.a;
}

extern float get_stroke_time_start(unsigned int i)
{
    return _strokes[i].time_range.start;
}
extern float get_stroke_time_end(unsigned int i)
{
    return _strokes[i].time_range.end;
}

extern DrawMode get_stroke_draw_mode(unsigned int i)
{
    return _strokes[i].draw_mode;
}

extern float get_stroke_arclength(unsigned int i)
{
    return _strokes[i].arclength;
}

extern unsigned int get_stroke_vertex_start(unsigned int i)
{
    return _strokes[i].vertex_range.start;
}
extern unsigned int get_stroke_vertex_end(unsigned int i)
{
    return _strokes[i].vertex_range.end;
}

extern unsigned int get_stroke_section_start(unsigned int i)
{
    return _strokes[i].section_range.start;
}
extern unsigned int get_stroke_section_end(unsigned int i)
{
    return _strokes[i].section_range.end;
}


extern unsigned int attribute_count()
{
    return _attribute_count;
}
extern const char * get_attribute_name_ptr(unsigned int i)
{
    return _attributes[i].name;
}
extern unsigned int get_attribute_name_length(unsigned int i)
{
    const char * s = get_attribute_name_ptr(i);
    
    return string_length(s);
}
extern unsigned int get_attribute_size(unsigned int i)
{
    return _attributes[i].size;
}
extern unsigned int get_attribute_offset(unsigned int i)
{
    return _attributes[i].offset;
}

extern unsigned int uniform_count()
{
    return _uniform_count;
}

extern const char * get_uniform_name_ptr(unsigned int i)
{
    return _uniforms[i].name;
}
extern unsigned int get_uniform_name_length(unsigned int i)
{
    const char * s = get_uniform_name_ptr(i);
    
    return string_length(s);
}

extern unsigned int get_uniform_size(unsigned int i)
{
    return _uniforms[i].size;
}
extern const float * get_uniform_data(unsigned int i)
{
    return _uniforms[i].data;
}

extern unsigned int buffer_size()
{
    return _buffer_size;
}
extern float const * get_buffer()
{
    return _buffer;
}

#endif