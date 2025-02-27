#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gRender;

uniform vec3 color;

uniform sampler2D iRender;
uniform vec2 resolution;

vec3 desaturate(vec3 color)
{
    float bw = (min(color.r, min(color.g, color.b)) + max(color.r, max(color.g, color.b))) * 0.5;
    return vec3(bw, bw, bw);
}

void main()
{
    gRender.rgb = desaturate(texture(iRender, gl_FragCoord.xy / resolution).rgb) * color;
    gRender.a = 1.0;
}