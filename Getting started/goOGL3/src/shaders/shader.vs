#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 ourColor;
uniform float time;

void main()
{
    float s = sin(time) / 2.0;
    float c = cos(time) / 2.0;
    gl_Position = vec4(position.x + s, position.y + c, position.z, 1.0);
    ourColor = vec3(position.x + s, position.y + c, position.z - (c+s) / 2.0);
}