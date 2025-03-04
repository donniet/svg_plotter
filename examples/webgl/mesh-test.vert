#version 300 es
// an attribute is an input (in) to a vertex shader.
// It will receive data from a buffer
in vec2 a_position;
in vec2 a_uv;
in float a_arclength;
in vec2 a_brush;

out vec2 uv;
out vec2 pos;
out float arclength;
out vec2 brush;

uniform mat3 view;
uniform vec2 stroke_range;

// all shaders have a main function
void main() 
{
    // gl_Position is a special variable a vertex shader
    // is responsible for setting
    vec2 p = a_position;
    pos = a_position;

    arclength = a_arclength;
    brush = a_brush;

    p.x = dot(view[0].xy, a_position.xy) + view[0][2];
    p.y = dot(view[1].xy, a_position.xy) + view[1][2];

    gl_Position = vec4(p.xy, 0, 1);
    
    uv = a_uv;
}