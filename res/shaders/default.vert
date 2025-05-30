#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 worldPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    worldPos = transform * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
    Normal = vec3(transform * vec4(aNormal, 0.0f));
    TexCoords = aTexCoords;
}