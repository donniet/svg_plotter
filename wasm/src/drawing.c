#include "drawing.h"


uint string_length(const char * s)
{
    if(s == NULL)
        return 0;

    for(uint i = 0; i < MAX_STRING_LENGTH; ++i)
        if(s[i] == '\0')
            return i;

    return MAX_STRING_LENGTH;
}

int string_compare(const char * a, const char * b)
{
    if(a == NULL && b == NULL)
        return 0;

    if(a == NULL)
        return -1;

    if(b == NULL)
        return 1;
    
    for(uint i = 0; i < MAX_STRING_LENGTH; i++)
    {
        if(a[i] == b[i]) 
        {
            if(a[i] == '\0')
                return 0;

            continue;
        }

        if(a[i] < b[i])
            return -1;
        
        return 1;
    }

    return 0;
}


uint gl_type_size(GLenum t)
{
    switch(t) 
    {
    case GL_INT:                return 1;
    case GL_UNSIGNED_INT:       return 1;
    case GL_FLOAT:              return 1;
    case GL_DOUBLE:             return 1;
    
    case GL_INT_VEC2:     return 2;
    case GL_INT_VEC3:     return 3;
    case GL_INT_VEC4:     return 4;
    
    case GL_UNSIGNED_INT_VEC2:     return 2;
    case GL_UNSIGNED_INT_VEC3:     return 3;
    case GL_UNSIGNED_INT_VEC4:     return 4;
    
    case GL_FLOAT_VEC2:      return 2;
    case GL_FLOAT_VEC3:      return 3;
    case GL_FLOAT_VEC4:      return 4;
    
    case GL_DOUBLE_VEC2:     return 2;
    case GL_DOUBLE_VEC3:     return 3;
    case GL_DOUBLE_VEC4:     return 4;
    
    case GL_FLOAT_MAT2:      return 4;
    case GL_FLOAT_MAT2x3:    return 6;
    case GL_FLOAT_MAT2x4:    return 8;
    
    case GL_FLOAT_MAT3:      return 9;
    case GL_FLOAT_MAT3x2:    return 6;
    case GL_FLOAT_MAT3x4:    return 12;
    
    case GL_FLOAT_MAT4:      return 16;
    case GL_FLOAT_MAT4x2:    return 8;
    case GL_FLOAT_MAT4x3:    return 12;
    }

    return 0;
}

uint js_array_element_size(GLenum t)
{
    switch(t) 
    {
    case GL_INT:                return sizeof(GLint);
    case GL_UNSIGNED_INT:       return sizeof(GLuint);
    case GL_FLOAT:              return sizeof(GLfloat);
    case GL_DOUBLE:             return sizeof(GLdouble);
    
    case GL_INT_VEC2:     return sizeof(GLint);
    case GL_INT_VEC3:     return sizeof(GLint);
    case GL_INT_VEC4:     return sizeof(GLint);
    
    case GL_UNSIGNED_INT_VEC2:     return sizeof(GLuint);
    case GL_UNSIGNED_INT_VEC3:     return sizeof(GLuint);
    case GL_UNSIGNED_INT_VEC4:     return sizeof(GLuint);
    
    case GL_FLOAT_VEC2:      return sizeof(GLfloat);
    case GL_FLOAT_VEC3:      return sizeof(GLfloat);
    case GL_FLOAT_VEC4:      return sizeof(GLfloat);
    
    case GL_DOUBLE_VEC2:     return sizeof(GLdouble);
    case GL_DOUBLE_VEC3:     return sizeof(GLdouble);
    case GL_DOUBLE_VEC4:     return sizeof(GLdouble);
    
    case GL_FLOAT_MAT2:      return sizeof(GLfloat);
    case GL_FLOAT_MAT2x3:    return sizeof(GLfloat);
    case GL_FLOAT_MAT2x4:    return sizeof(GLfloat);
    
    case GL_FLOAT_MAT3:      return sizeof(GLfloat);
    case GL_FLOAT_MAT3x2:    return sizeof(GLfloat);
    case GL_FLOAT_MAT3x4:    return sizeof(GLfloat);
    
    case GL_FLOAT_MAT4:      return sizeof(GLfloat);
    case GL_FLOAT_MAT4x2:    return sizeof(GLfloat);
    case GL_FLOAT_MAT4x3:    return sizeof(GLfloat);
    }

    return 0;
}

const char * glsl_variable_type_name(GLenum t)
{
    switch(t) 
    {
    case GL_INT:                return "int";
    case GL_UNSIGNED_INT:       return "uint";
    case GL_FLOAT:              return "float";
    case GL_DOUBLE:             return "double";
    
    case GL_INT_VEC2:     return "ivec2";
    case GL_INT_VEC3:     return "ivec3";
    case GL_INT_VEC4:     return "ivec4";
    
    case GL_UNSIGNED_INT_VEC2:     return "uvec2";
    case GL_UNSIGNED_INT_VEC3:     return "uvec3";
    case GL_UNSIGNED_INT_VEC4:     return "uvec4";
    
    case GL_FLOAT_VEC2:      return "vec2";
    case GL_FLOAT_VEC3:      return "vec3";
    case GL_FLOAT_VEC4:      return "vec4";
    
    case GL_DOUBLE_VEC2:     return "dvec2";
    case GL_DOUBLE_VEC3:     return "dvec3";
    case GL_DOUBLE_VEC4:     return "dvec4";
    
    case GL_FLOAT_MAT2:      return "mat2";
    case GL_FLOAT_MAT2x3:    return "mat2x3";
    case GL_FLOAT_MAT2x4:    return "mat2x4";
    
    case GL_FLOAT_MAT3:      return "mat3";
    case GL_FLOAT_MAT3x2:    return "mat3x2";
    case GL_FLOAT_MAT3x4:    return "mat3x4";
    
    case GL_FLOAT_MAT4:      return "mat4";
    case GL_FLOAT_MAT4x2:    return "mat4x2";
    case GL_FLOAT_MAT4x3:    return "mat4x3";
    }

    return NULL;
}

GLenum glsl_variable_type_from(const char * s)
{
    uint l = string_length(s);
    
    GLenum el = GL_FLOAT;
    int size1 = 1;
    int size2 = 1;

    switch(s[0]) {
    case 'b': el = GL_BOOL; break;
    case 'i': el = GL_INT; break;
    case 'u': el = GL_UNSIGNED_INT; break;
    case 'd': el = GL_DOUBLE; break;
    case 'f':
    default:
        el = GL_FLOAT; 
        break;
    }

    switch(el) {
    case GL_BOOL: 
        if(s[1] == 'o') 
            return GL_BOOL;

        switch(s[4]) {
        case '2': return GL_BOOL_VEC2;
        case '3': return GL_BOOL_VEC3;
        case '4': return GL_BOOL_VEC4;
        }
        break;
    case GL_INT:
        if(s[1] == 'n')
            return GL_INT;

        switch(s[4]) {
        case '2': return GL_INT_VEC2;
        case '3': return GL_INT_VEC3;
        case '4': return GL_INT_VEC4;
        }
        break;
    case GL_UNSIGNED_INT:
        if(s[1] == 'i')
            return GL_UNSIGNED_INT;

        switch(s[4]) {
        case '2': return GL_UNSIGNED_INT_VEC2;
        case '3': return GL_UNSIGNED_INT_VEC3;
        case '4': return GL_UNSIGNED_INT_VEC4;
        }
        break;
    case GL_DOUBLE:
        if(s[1] == 'o')
            return GL_DOUBLE;

        switch(s[4]) {
        case '2': return GL_DOUBLE_VEC2;
        case '3': return GL_DOUBLE_VEC3;
        case '4': return GL_DOUBLE_VEC4;
        }
        break;
    case GL_FLOAT:
        switch(s[0]) {
        case 'm':
            switch(s[3]) {
            case '2': 
                if(l == 4)
                    return GL_FLOAT_MAT2;
                
                switch(s[5]) {
                case '2': return GL_FLOAT_MAT2;
                case '3': return GL_FLOAT_MAT2x3;
                case '4': return GL_FLOAT_MAT2x4;
                }
                break;
            case '3':
                if(l == 4)
                    return GL_FLOAT_MAT3;
                
                switch(s[5]) {
                case '2': return GL_FLOAT_MAT3x2;
                case '3': return GL_FLOAT_MAT3;
                case '4': return GL_FLOAT_MAT3x4;
                }
                break;
            case '4':
                if(l == 4)
                    return GL_FLOAT_MAT4;
                
                switch(s[5]) {
                case '2': return GL_FLOAT_MAT4x2;
                case '3': return GL_FLOAT_MAT4x3;
                case '4': return GL_FLOAT_MAT4;
                }
                break;
            }
            break;
        case 'v':
            switch(s[3]) {
            case '2': return GL_FLOAT_VEC2;
            case '3': return GL_FLOAT_VEC3;
            case '4': return GL_FLOAT_VEC4;
            }
            break;
        } 
    }
    return GL_FLOAT;
}

const char * uniform_setter_from_type(GLenum v)
{
    switch(v) {
    case GL_BOOL:
    case GL_INT:
        return "uniform1iv";
    case GL_UNSIGNED_INT:
        return "uniform1uv";
    case GL_HALF_FLOAT:
    case GL_FLOAT:
    case GL_DOUBLE:
        return "uniform1fv";
    case GL_BOOL_VEC2:
    case GL_INT_VEC2:
        return "uniform2iv";
    case GL_UNSIGNED_INT_VEC2:
        return "uniform2uv";
    case GL_FLOAT_VEC2:
    case GL_DOUBLE_VEC2:
        return "uniform2fv";
    case GL_BOOL_VEC3:
    case GL_INT_VEC3:
        return "uniform3iv";
    case GL_UNSIGNED_INT_VEC3:
        return "uniform3uv";
    case GL_FLOAT_VEC3:
    case GL_DOUBLE_VEC3:
        return "uniform3fv";
    case GL_BOOL_VEC4:
    case GL_INT_VEC4:
        return "uniform4iv";
    case GL_UNSIGNED_INT_VEC4:
        return "uniform4uv";
    case GL_FLOAT_VEC4:
    case GL_DOUBLE_VEC4:
        return "uniform4fv";
    case GL_FLOAT_MAT2:
        return "uniformMatrix2fv";
    case GL_FLOAT_MAT3:
        return "uniformMatrix3fv";
    case GL_FLOAT_MAT4:
        return "uniformMatrix4fv";
    case GL_FLOAT_MAT2x3:
        return "uniformMatrix2x3fv";
    case GL_FLOAT_MAT2x4:
        return "uniformMatrix2x4fv";
    case GL_FLOAT_MAT3x2:
        return "uniformMatrix3x2fv";
    case GL_FLOAT_MAT3x4:
        return "uniformMatrix3x4fv";
    case GL_FLOAT_MAT4x2:
        return "uniformMatrix4x2fv";
    case GL_FLOAT_MAT4x3:
        return "uniformMatrix4x3fv";
    }

    return "uniform1fv";
}

const char * js_typed_array_name(GLenum array_type)
{
    switch(array_type)
    {
    case GL_DOUBLE:
    case GL_DOUBLE_VEC2:
    case GL_DOUBLE_VEC3:
    case GL_DOUBLE_VEC4:
    case GL_DOUBLE_MAT2:
    case GL_DOUBLE_MAT2x3:
    case GL_DOUBLE_MAT2x4:
    case GL_DOUBLE_MAT3x2:
    case GL_DOUBLE_MAT3:
    case GL_DOUBLE_MAT3x4:
    case GL_DOUBLE_MAT4x2:
    case GL_DOUBLE_MAT4x3:
    case GL_DOUBLE_MAT4:
        return "Float64Array";
    case GL_FLOAT:
    case GL_FLOAT_VEC2:
    case GL_FLOAT_VEC3:
    case GL_FLOAT_VEC4:
    case GL_FLOAT_MAT2:
    case GL_FLOAT_MAT2x3:
    case GL_FLOAT_MAT2x4:
    case GL_FLOAT_MAT3x2:
    case GL_FLOAT_MAT3:
    case GL_FLOAT_MAT3x4:
    case GL_FLOAT_MAT4x2:
    case GL_FLOAT_MAT4x3:
    case GL_FLOAT_MAT4:
        return "Float32Array";
    #ifdef GL_HALF_FLOAT
    case GL_HALF_FLOAT:
        return "Float16Array";
    #endif
    case GL_INT:
    case GL_INT_VEC2:
    case GL_INT_VEC3:
    case GL_INT_VEC4:
        return "Int32Array";
    case GL_SHORT:
        return "Int16Array";
    case GL_BYTE:
        return "Int8Array";
    case GL_UNSIGNED_INT:
    case GL_UNSIGNED_INT_VEC2:
    case GL_UNSIGNED_INT_VEC3:
    case GL_UNSIGNED_INT_VEC4:
        return "Uint32Array";
    case GL_UNSIGNED_SHORT:
        return "Uint16Array";
    case GL_UNSIGNED_BYTE:
    default:
        return "Uint8Array";
    }
}

/**
 * Box structure
 */
float box_x0(const Box * box)
{
    return box->x0;
}
float box_y0(const Box * box)
{
    return box->y0;
}
float box_x1(const Box * box)
{
    return box->x1;
}
float box_y1(const Box * box)
{
    return box->y1;
}


/**
 * AttributeDef structure
 */
const char * attribute_def_name(const AttributeDef * d)
{
    return d->name;
}
GLint attribute_def_size(const AttributeDef * d)
{
    return d->size;
}
GLenum attribute_def_type(const AttributeDef * d)
{
    return d->type;
}

/**
 * Attribute structure
 */

const char * attribute_name(const Attribute * a)
{
    return a->name;
}
GLboolean attribute_normalized(const Attribute * a)
{
    return a->normalized;
}
GLsizei attribute_stride(const Attribute * a)
{
    return a->stride;
}
const void * attribute_offset(const Attribute * a)
{
    return a->offset;
}

/**
 * Uniform structure
 */

const char * uniform_name(const Uniform * u)
{
    return u->name;
}
const char * uniform_glsl_type(const Uniform * u)
{
    return glsl_variable_type_name(u->type);
}
const char * uniform_js_typed_array_name(const Uniform * u)
{
    return js_typed_array_name(u->type);
}
GLenum uniform_gl_type(const Uniform * u)
{
    return u->type;
}
const char * uniform_gl_setter(const Uniform * u)
{
    return uniform_setter_from_type(u->type);
}
const void * uniform_data(const Uniform * u)
{
    return u->data;
}
uint uniform_data_size(const Uniform * u)
{
    return u->data_size;
}

/**
 * Define
 */
const char * define_name(const Define * d)
{
    return d->name;
}
const char * define_value(const Define * d)
{
    return d->value;
}

/**
 * Layer structure
 */
GLuint layer_draw_mode(const Layer * l)
{
    return l->draw_mode;
}
const char * layer_shader_name(const Layer * l)
{
    return l->shader_name;
}
uint layer_attribute_count(const Layer * l)
{
    for(uint i = 0; i < MAX_ATTRIBUTES; i++)
        if(l->attributes[i].name == NULL)
            return i;

    return MAX_ATTRIBUTES;
}
const Attribute * layer_attribute(const Layer * l, uint i)
{
    return &l->attributes[i];
}
uint layer_uniform_count(const Layer * l)
{
    for(uint i = 0; i < MAX_UNIFORMS; i++)
        if(l->uniforms[i].name == NULL)
            return i;

    return MAX_UNIFORMS;
}
const Uniform * layer_uniform(const Layer * l, uint i)
{
    return &l->uniforms[i];
}
uint layer_draw_offset(const Layer * l)
{
    return l->range[0];
}
uint layer_draw_count(const Layer * l)
{
    return l->range[1] - l->range[0];
}

const char * layer_vertex_array_name(const Layer * l)
{
    return l->vertex_array_name;
}
const char * layer_element_array_name(const Layer * l)
{
    return l->element_array_name;
}

/**
 * ShaderFile
 */
const char * shader_file_path(const ShaderFile * f)
{
    return f->path;
}
const char * shader_file_text(const ShaderFile * f)
{
    return f->text;
}
GLenum shader_file_type(const ShaderFile * f)
{
    return f->shader_type;
}


/**
 * Shader
 */
extern const Shader * shader_by_index(uint index)
{
    return &shaders[index];
}
uint shader_count()
{
    for(uint i = 0; i < MAX_SHADERS; i++)
        if(shaders[i].name == NULL)
            return i;
    
    return MAX_SHADERS;
}
extern const Shader * shader_by_name(const char * shader_name)
{
    uint N = shader_count();
    for(uint i = 0; i < N; i++)
        if(string_compare(shader_name, shaders[i].name) == 0)
            return &shaders[i];
    
    return NULL;
}
const char * shader_name(const Shader * s)
{
    return s->name;
}
uint shader_file_count(const Shader * s)
{
    for(uint i = 0; i < MAX_SHADER_FILES; i++)
        if(s->shader_files[i].path == NULL)
            return i;

    return MAX_SHADER_FILES;
}
const ShaderFile * shader_file(const Shader * s, uint i)
{
    return &s->shader_files[i];
}
uint shader_define_size(const Shader * s)
{
    for(uint i = 0; i < MAX_DEFINES; i++)
        if(s->defines[i].name == NULL)
            return i;

    return MAX_DEFINES;
}
const Define * shader_define(const Shader * s, uint i)
{
    return &s->defines[i];
}
const Uniform * shader_uniform(const Shader * s, uint i)
{
    return &s->uniforms[i];
}
uint shader_uniform_size(const Shader * s)
{
    for(uint i = 0; i < MAX_UNIFORMS; i++)
        if(s->uniforms[i].name == NULL)
            return i;

    return MAX_UNIFORMS;
}
const AttributeDef * shader_attribute_def(const Shader * s, uint i)
{
    return &s->attributes[i];
}
uint shader_attribute_size(const Shader * s)
{
    for(uint i = 0; i < MAX_ATTRIBUTES; i++)
        if(s->attributes[i].name == NULL)
            return i;
    
    return MAX_ATTRIBUTES;
}

/**
 * RGBA
 */

float rgba_red(const RGBA * c)
{
    return c->red;
}
float rgba_green(const RGBA * c)
{
    return c->green;
}
float rgba_blue(const RGBA * c)
{
    return c->blue;
}
float rgba_alpha(const RGBA * c)
{
    return c->alpha;
}

/**
 * Blend structure
 */

const RGBA * blend_color(const Blend *b)
{
    return &b->color;
}
GLenum blend_equation_rgb(const Blend *b)
{
    return b->equation_rgb;
}
GLenum blend_equation_alpha(const Blend *b)
{
    return b->equation_alpha;
}
GLenum blend_source_rgb(const Blend *b)
{
    return b->source_rgb;
}
GLenum blend_destination_rgb(const Blend *b)
{
    return b->destination_rgb;
}
GLenum blend_source_alpha(const Blend *b)
{
    return b->source_alpha;
}
GLenum blend_destination_alpha(const Blend *b)
{
    return b->destination_alpha;
}

uint array_count()
{
    for(uint i = 0; i < MAX_ARRAYS; i++)
        if(arrays[i].data == NULL)
            return i;

    return MAX_ARRAYS;
}
const Array * array_by_index(uint i)
{
    return &arrays[i];
}
const Array * array_by_name(const char * s)
{
    uint N = array_count();
    for(uint i = 0; i < N; i++)
        if(string_compare(s, arrays[i].name) == 0)
            return &arrays[i];
    
    return NULL;
}
const char * array_name(const Array * a)
{
    return a->name;
}
const void * array_data(const Array * a)
{
    return a->data;
}
uint array_size(const Array * a)
{
    return a->size;
}
GLenum array_type(const Array * a)
{
    return a->type;
}
const char * array_js_typed_array_name(const Array * a)
{
    return js_typed_array_name(a->type);
}
uint array_js_typed_array_size(const Array * a)
{
    return a->size / js_array_element_size(a->type);
}
GLenum array_target(const Array * a)
{
    return a->target;
}

/**
 * Drawing structure
 */

const char * drawing_name(const Drawing * d) 
{ 
    return d->name;
}
float drawing_dimension_width(const Drawing * d) 
{ 
    return d->dimensions[0];
}
float drawing_dimension_height(const Drawing * d) 
{ 
    return d->dimensions[1];
}
uint drawing_layers_size(const Drawing * d) 
{ 
    for(int i = 0; i < MAX_LAYERS; i++)
        if(d->layers[i].shader_name == NULL)
            return i;

    return MAX_LAYERS;
}
const Layer * drawing_layer(const Drawing * d, uint i) 
{ 
    return &d->layers[i];
}
const Blend * drawing_blend(const Drawing * d) 
{ 
    return d->blend;
}
const RGBA * drawing_clear_color(const Drawing * d) 
{ 
    return &d->clear_color;
}
extern GLbitfield drawing_clear_bits(const Drawing * d)
{
    return d->clear_bits;
}

/**
 * Drink structure
 */


uint drink_count()
{
    for(uint i = 0; i < MAX_DRINKS; i++)
        if(drinks[i].name == NULL)
            return i;

    return MAX_DRINKS;
}
const Drink * drink_by_index(uint i)
{
    return &drinks[i];
}

const char * drink_name(const Drink * d)
{
    return d->name;
}
const Drawing * drink_drawing(const Drink * d)
{
    return &d->drawing;
}

/**
 * Location
 */

const Location * locations[] = {};

uint location_count()
{
    return sizeof(locations) / sizeof(const Location *);
}
const Location * location(uint i)
{
    return locations[i];
}

const char * location_name(const Location * l) 
{
    return l->name;
}
uint location_utc_offset_minutes(const Location * l) 
{
    return l->utc_offset_minutes;
}
float location_latitude(const Location * l) 
{
    return l->lat_long[0];
}
float location_longitude(const Location * l) 
{
    return l->lat_long[1];
}
float location_drink_position_x(const Location * l) 
{
    return l->drink_position[0];
}
float location_drink_position_y(const Location * l) 
{
    return l->drink_position[1];
}
const Box * location_prev_drink_hitbox(const Location * l) 
{
    return &l->prev_drink_hitbox;
}
const Box * location_next_drink_hitbox(const Location * l) 
{ 
    return &l->next_drink_hitbox;
}

const Drawing * location_drawing(const Location * l)
{
    return &l->drawing;
}

/**
 * Canvas
 */

uint canvas_width()
{
    return canvas_size[0];
}

uint canvas_height()
{
    return canvas_size[1];
}