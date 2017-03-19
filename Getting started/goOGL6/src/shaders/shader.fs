#version 330 core

//in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture00;
uniform sampler2D texture01;
//uniform float time;

out vec4 color;

void main()
{
    color = mix(texture(texture00, TexCoord), texture(texture01, vec2(TexCoord.x, 1.0-TexCoord.y)), 0.2);
}