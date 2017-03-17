#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform float time;

out vec4 color;

void main()
{
    color = mix(texture(Texture0, TexCoord), texture(Texture1, vec2(TexCoord.x, 1.0-TexCoord.y)), (sin(time*2.0)+1.0)/2.0) * vec4(ourColor, 1.0);
}