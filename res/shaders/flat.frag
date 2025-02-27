#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gRender;

void main()
{
    gRender.rgb = vec3(0.7, 0.2, 0.7);
    gRender.a = 1.0;
}