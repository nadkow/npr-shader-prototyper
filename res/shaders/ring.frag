#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gRender;

in vec4 worldPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform vec2 resolution;

void main()
{
    //get the dot product between the normal and the view direction
    vec3 viewDir = normalize(viewPos - vec3(worldPos));
    float dotproduct = max(dot(Normal, viewDir), .3);
    float value = sin(8/dotproduct) * .5 + .5;
    gRender.rgb = vec3(value, value, value);
    //gRender.rg = gl_FragCoord.xy / resolution;
    //gRender.a = 1.0;
}