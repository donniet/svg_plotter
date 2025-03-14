#version 300 es
// an attribute is an input (in) to a vertex shader.
// It will receive data from a buffer
in vec2 a_position;
in vec2 a_uv;
in float a_arclength;
in vec2 a_brush;
in float a_section;

out vec2 uv;
out vec2 pos;
out float arclength;
out vec2 brush;
out float section;

uniform mat3 view;
uniform vec4 uv_range;
uniform vec2 stroke_range;

// all shaders have a main function
void main() 
{
    // gl_Position is a special variable a vertex shader
    // is responsible for setting
    vec2 p = a_position;
    pos = a_position;

    section = a_section;
    arclength = a_arclength;
    brush = a_brush;
    uv = a_uv;

    p.x = dot(view[0].xy, a_position.xy) + view[0][2];
    p.y = dot(view[1].xy, a_position.xy) + view[1][2];

    float z = clamp(a_uv[1] / uv_range[3], 0., 1.);

    gl_Position = vec4(p.xy, z, 1.);
}