#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gRender;

in vec4 worldPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform sampler2D iRender;
uniform vec2 resolution;

void main()
{
    //get the dot product between the normal and the view direction
    vec3 viewDir = normalize(viewPos - vec3(worldPos));
    float fresnel = dot(Normal, viewDir);
    //invert the fresnel so the big values are on the outside
    fresnel = clamp(1 - fresnel, 0.0, 1.0);
    //hardcoded multiplication blend mode
    gRender.rgb = vec3(fresnel, fresnel, 1.0) * texture(iRender, gl_FragCoord.xy / resolution).rgb;
    //gRender.rg = gl_FragCoord.xy / resolution;
    //gRender.a = 1.0;
}