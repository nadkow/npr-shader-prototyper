#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gRender;

void main()
{
    gRender.r = 1.0;
    gRender.g = 0.0;
    gRender.b = 0.0;
    gRender.a = 1.0;
}