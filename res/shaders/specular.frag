#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gRender;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform vec3 pointPos;
uniform vec4 pointColor;

uniform sampler2D iRender;
uniform vec2 resolution;

vec3 calcSpec(vec3 norm) {

    vec3 viewDir = normalize(viewPos - FragPos);
    float shininess = 128;
    float specularStrength = 0.3;
    vec3 lightDir = normalize(pointPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 pointSpecular = vec3(1.,1.,1.) * spec * specularStrength;

    return pointSpecular;
}

void main()
{
    vec3 norm = normalize(Normal);
    gRender.rgb = texture(iRender, gl_FragCoord.xy / resolution).rgb + calcSpec(norm);
    gRender.a = 1.0;
}