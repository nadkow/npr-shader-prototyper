#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gRender;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 pointPos;
uniform vec4 pointColor;

vec3 calcPoint(vec3 norm) {
    float constant = 1.0f;
    float linear = 0.35f;
    float quadratic = 0.44f;
    float distance = length(pointPos - FragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    vec3 pointDir = normalize(pointPos - FragPos);
    float pointDiff = max(dot(norm, pointDir), 0.0);

    return (pointDiff * pointColor.xyz) * attenuation;
}

void main()
{
    vec3 norm = normalize(Normal);
    gRender.rgb = calcPoint(norm);
    gRender.a = 1.0;
}