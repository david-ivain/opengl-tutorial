#version 330 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_tex_coord;
layout(location = 3) in mat4 instance_matrix;

uniform float aspect = 1.0;

out vec4 v_colour;

void main()
{
    vec4 new_position = instance_matrix * vec4(v_position, 1.0);
    new_position.x = new_position.x / aspect;
    gl_Position = new_position;
    v_colour = vec4(1.0, 1.0, 1.0, 1.0);
}
