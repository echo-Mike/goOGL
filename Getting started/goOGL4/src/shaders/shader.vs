#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;

out vec3 ourColor;
out vec2 TexCoord;
uniform float time;

void main()
{
    float s = sin(time) / 2.0;
    float c = cos(time) / 2.0;
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    ourColor = color;//vec3(position.x + s, position.y + c, position.z - (c+s) / 2.0);
    TexCoord = texCoord;
}