#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gRender;

out vec4 FragColor;

in vec4 worldPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform sampler2D iRender;
uniform vec2 resolution;

void main()
{
    FragColor = vec4(0, 0, 0, 1);

