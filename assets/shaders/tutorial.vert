#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec4 vColor;
out vec2 TexCoord;

uniform vec4 ourColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vColor = ourColor * vec4(aColor, 1.0);
    TexCoord = aTexCoord;
    // vColor = vec4(aColor, 1.0)
}
