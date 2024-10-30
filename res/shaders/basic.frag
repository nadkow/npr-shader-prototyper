#version 460 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 Position;

uniform sampler2D ourTexture;
uniform vec4 ambientColor;
uniform vec3 viewPos;
uniform vec3 pointPos;
uniform vec4 pointColor;
uniform vec3 directionalLight;
uniform vec4 dirColor;

vec4 calcPoint(vec3 norm, vec3 viewDir) {
    float constant = 1.0f;
    float linear = 0.35f;
    float quadratic = 0.44f;
    float distance = length(pointPos - Position);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    vec3 pointDir = normalize(pointPos - Position);
    float pointDiff = max(dot(norm, pointDir), 0.0);

    float shininess = 2;
    float specularStrength = 0.3;
    vec3 lightDir = normalize(pointPos - Position);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec4 pointSpecular = pointColor * spec * specularStrength;
    return (pointDiff * pointColor + pointSpecular) * attenuation;
}

vec4 calcDir(vec3 norm, vec3 viewDir) {
    vec3 lightDir = normalize(directionalLight);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    vec4 diffuse  = dirColor  * diff;
    vec4 specular = dirColor * spec * 0.3f;
    return diffuse + specular;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - Position);

    vec4 pointLightContrib = calcPoint(norm, viewDir);
    vec4 directionalContrib = calcDir(norm, viewDir);
    vec4 spotContrib1 = calcSpot(norm, viewDir, spotPos1, spotDir1, cutOff1, spotColor1);
    vec4 spotContrib2 = calcSpot(norm, viewDir, spotPos2, spotDir2, cutOff2, spotColor2);

    FragColor = (ambientColor + pointLightContrib + directionalContrib) * texture(ourTexture, TexCoord);
}