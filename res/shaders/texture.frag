#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gRender;

in vec2 TexCoords;

uniform sampler2D tex;

void main()
{
    gRender.rgb = texture(tex, TexCoords).rgb;
    gRender.a = 1.0;
}