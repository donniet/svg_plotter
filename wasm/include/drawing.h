#ifndef __DRAWING_H__
#define __DRAWING_H__

#define GL_GLEXT_LEGACY
#include <GL/gl.h>

// TODO: determine feasibility of including glext.h

// this include does not work due to the lack of libc
// #include <GL/glext.h>
/* glext defines */

#include "glext.h"

#define NULL (0x0)

#define MAX_SHADERS 64
#define MAX_SHADER_FILES 16
#define MAX_LAYERS 32
#define MAX_STRING_LENGTH 0xFFFFFF
#define MAX_DEFINES 16
#define MAX_UNIFORMS 32
#define MAX_ATTRIBUTES 32
#define MAX_ARRAYS 128
#define MAX_DRINKS 64
#define MAX_LOCATIONS 256

// mean to supercede mesh.h and solid.h
// simple exported functions that can pull in all necessary data
// - Float32Array's for all the data
// - Int32Array's for indices
// - Helper functions for strings
// - search functions?

typedef unsigned int uint;
typedef unsigned int uint32;
typedef unsigned short uint16;

extern uint             string_length(const char *);
extern int              string_compare(const char *, const char *);

extern const char *     variable_type_name(GLenum);
extern GLenum           variable_type_from(const char *);

extern const char *     array_type_name(GLenum);

/**
 * Box structure
 */
typedef struct {
    float x0, y0, x1, y1;
} Box;

extern float            box_x0(const Box *);
extern float            box_y0(const Box *);
extern float            box_x1(const Box *);
extern float            box_y1(const Box *);

/**
 * AttributeDef
 */
typedef struct {
    const char * name;
    GLint size;
    GLenum type;
} AttributeDef;

extern const char *     attribute_def_name(const AttributeDef *);
extern GLint            attribute_def_size(const AttributeDef *);
extern GLenum           attribute_def_type(const AttributeDef *);

/**
 * Attribute structure
 */
typedef struct {
    const char * name;
    GLboolean normalized;
    GLsizei stride;
    const void * offset;
} Attribute;

extern const char *     attribute_name(const Attribute *);
extern GLboolean        attribute_normalized(const Attribute *);
extern GLsizei          attribute_stride(const Attribute *);
extern const void *     attribute_offset(const Attribute *);

/**
 * Uniform structure
 */
typedef struct {
    const char * name;
    GLenum type;
    const void * data;
    const uint data_size;
} Uniform;

extern const char *     uniform_name(const Uniform *);
extern const char *     uniform_type(const Uniform *);
extern GLenum           uniform_gl_type(const Uniform *);
extern const char *     uniform_gl_setter(const Uniform *);
extern const void *     uniform_data(const Uniform *);
extern uint             uniform_data_size(const Uniform *);
extern const char *     uniform_setter_from_type(GLenum);

/**
 * Define structure
 */
typedef struct {
    const char * name;
    const char * value;
} Define;

extern const char *     define_name(const Define *);
extern const char *     define_value(const Define *);


/**
 * Layer structure
 */
typedef struct {
    const char * shader_name;

    Attribute attributes[MAX_ATTRIBUTES];
    Uniform uniforms[MAX_UNIFORMS];

    const char * vertex_array_name;
    const char * element_array_name;

    GLuint draw_mode;
    uint range[2];
} Layer;

extern GLuint           layer_draw_mode(const Layer *);
extern const char *     layer_shader_name(const Layer *);
extern uint             layer_attribute_count(const Layer *);
extern const Attribute * layer_attribute(const Layer *, uint);
extern uint             layer_uniform_count(const Layer *);
extern const Uniform *  layer_uniform(const Layer *, uint);
extern uint             layer_element_byte_size(const Layer *);
extern uint             layer_draw_offset(const Layer *);
extern uint             layer_draw_count(const Layer *);
extern const char *     layer_vertex_array_name(const Layer *);
extern const char *     layer_element_array_name(const Layer *);

/**
 * ShaderFile
 */

typedef struct {
    const char * path;
    const char * text;
    GLenum shader_type;
} ShaderFile;

extern const char *     shader_file_path(const ShaderFile *);
extern const char *     shader_file_text(const ShaderFile *);
extern GLenum           shader_file_type(const ShaderFile *);

typedef struct {
    const char * name;

    ShaderFile shader_files[MAX_SHADER_FILES];
    
    const Define defines[MAX_DEFINES];
    const Uniform uniforms[MAX_UNIFORMS];
    const AttributeDef attributes[MAX_ATTRIBUTES];
} Shader;

extern const Shader shaders[MAX_SHADERS];

extern const Shader *   shader_by_index(uint index);
extern uint             shader_count();
extern const Shader *   shader_by_name(const char * shader_name);
extern const char *     shader_name(const Shader *);
extern uint             shader_file_count(const Shader *);
extern const ShaderFile * shader_file(const Shader *, uint);
extern uint             shader_define_size(const Shader *);
extern const Define *   shader_define(const Shader *, uint);
extern uint             shader_uniform_size(const Shader *);
extern const Uniform *  shader_uniform(const Shader *, uint);
extern uint             shader_attribute_size(const Shader *);
extern const AttributeDef * shader_attribute_def(const Shader *, uint);

/**
 * RGBA
 */
typedef struct {
    float red, green, blue, alpha;
} RGBA;

extern float            rgba_red(const RGBA *);
extern float            rgba_green(const RGBA *);
extern float            rgba_blue(const RGBA *);
extern float            rgba_alpha(const RGBA *);

/**
 * Blend structure
 */
typedef struct {
    RGBA color;
    GLenum equation_rgb;
    GLenum equation_alpha;
    GLenum source_rgb;
    GLenum destination_rgb;
    GLenum source_alpha;
    GLenum destination_alpha;
} Blend;

extern const RGBA *     blend_color(const Blend *);
extern GLenum           blend_equation_rgb(const Blend *);
extern GLenum           blend_equation_alpha(const Blend *);
extern GLenum           blend_source_rgb(const Blend *);
extern GLenum           blend_destination_rgb(const Blend *);
extern GLenum           blend_source_alpha(const Blend *);
extern GLenum           blend_destination_alpha(const Blend *);

/**
 * Arrays
 */
typedef struct {
    const char * name;
    GLenum type;
    const void * data;
    uint size;
    GLenum target;
} Array;

extern const Array arrays[MAX_ARRAYS];

extern uint             array_count();
extern const Array *    array_by_index(uint);
extern const Array *    array_by_name(const char *);
extern const char *     array_name(const Array *);
extern const void *     array_data(const Array *);
extern uint             array_size(const Array *);
extern GLenum           array_type(const Array *);
extern GLenum           array_target(const Array *);

/**
 * Drawing structure
 */
typedef struct {
    const char * name;
    float dimensions[2];
    const Layer layers[MAX_LAYERS];
    const Blend * blend;
    RGBA clear_color;
    GLenum clear_bits;
} Drawing;

extern const char *     drawing_name(const Drawing *);
extern float            drawing_dimension_width(const Drawing *);
extern float            drawing_dimension_height(const Drawing *);
extern uint             drawing_layers_size(const Drawing *);
extern const Layer *    drawing_layer(const Drawing *, uint i);
extern const Blend *    drawing_blend(const Drawing *);
extern const RGBA *     drawing_clear_color(const Drawing *);
extern GLbitfield       drawing_clear_bits(const Drawing *);

/**
 * Drink structure
 */
typedef struct {
    const char * name;
    Drawing drawing;
} Drink;


extern const Drink drinks[MAX_DRINKS];

extern uint             drink_count();
extern const Drink *    drink_by_index(uint);
extern const char *     drink_name(const Drink *);
extern const Drawing *  drink_drawing(const Drink *);

/**
 * Location
 */
typedef struct {
    const char * name;
    uint utc_offset_minutes;
    float lat_long[2];
    float drink_position[2];
    Box prev_drink_hitbox;
    Box next_drink_hitbox;
    Drawing drawing;
} Location;


extern uint             location_count();
extern const Location * location(uint);
extern const char *     location_name(const Location *);
extern uint             location_utc_offset_minutes(const Location *);
extern float            location_latitude(const Location *);
extern float            location_longitude(const Location *);
extern float            location_drink_position_x(const Location *);
extern float            location_drink_position_y(const Location *);
extern const Box *      location_prev_drink_hitbox(const Location *);
extern const Box *      location_next_drink_hitbox(const Location *);
extern const Drawing *  location_drawing(const Location *);

extern uint canvas_size[2];

extern uint canvas_width();
extern uint canvas_height();

#endif