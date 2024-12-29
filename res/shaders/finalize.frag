#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gRender;

void main()
{
    FragColor = texture(gRender, TexCoords);
}