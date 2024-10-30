#version 460 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

uniform vec4 ambientColor;
uniform vec3 pointPos;
uniform vec4 pointColor;
uniform vec3 directionalLight;
uniform vec4 dirColor;

vec4 calcDir(vec3 viewDir) {
    vec3 lightDir = normalize(directionalLight);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec4 diffuse  = dirColor * diff;
    vec4 specular = dirColor * spec;
    return diffuse + specular;
}

vec4 calcPoint(vec3 viewDir) {
    float constant = 1.0f;
    float linear = 0.35f;
    float quadratic = 0.44f;
    float distance = length(pointPos - Position);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    vec3 pointDir = normalize(pointPos - Position);
    float pointDiff = max(dot(Normal, pointDir), 0.0);

    float shininess = 256;
    vec3 lightDir = normalize(pointPos - Position);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), shininess);
    vec4 pointSpecular = pointColor * spec;
    return (pointDiff * pointColor + pointSpecular) * attenuation;
}

void main()
{
    vec3 viewDir = normalize(cameraPos - Position);
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    vec4 pointLightContrib = calcPoint(viewDir);
    vec4 directionalContrib = calcDir(viewDir);
    FragColor = (ambientColor + pointLightContrib + directionalContrib) * vec4(texture(skybox, R).rgb, 1.0);
}