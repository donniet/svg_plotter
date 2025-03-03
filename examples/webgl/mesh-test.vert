#version 300 es
// an attribute is an input (in) to a vertex shader.
// It will receive data from a buffer
in vec2 a_position;
in vec2 a_uv;

out vec2 uv;

uniform mat3 view;

// all shaders have a main function
void main() 
{
    // gl_Position is a special variable a vertex shader
    // is responsible for setting
    vec2 p = a_position;
    p.x = dot(view[0].xy, a_position.xy) + view[0][2];
    p.y = dot(view[1].xy, a_position.xy) + view[1][2];

    gl_Position = vec4(p.xy, 0, 1);
    uv = a_uv;
}