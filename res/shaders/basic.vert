#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 trans;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Normal = vec3(trans * vec4(aNormal, 0.0f));
    Position = vec3(trans * vec4(aPos, 1.f));
    gl_Position = projection * view * trans * vec4(aPos, 1.0);
}