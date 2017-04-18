#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture00;

out vec4 color;

void main()
{
    color = texture(texture00, vec2(TexCoord.x, 1.0 - TexCoord.y)) * vec4(ourColor, 1.0);
}