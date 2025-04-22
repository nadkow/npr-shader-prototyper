#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gRender;

void main()
{
    gRender.r = {{ color.r }};
    gRender.g = {{ color.g }};
    gRender.b = {{ color.b }};
    gRender.a = 1.0;
}