#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 aTrans;

out vec2 TexCoord;
out vec3 Normal;
out vec3 Position;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * aTrans * vec4(aPos, 1.0);
    TexCoord = aTex;
    Normal = vec3(aTrans * vec4(aNormal, 0.0f));
    Position = vec3(aTrans * vec4(aPos, 1.0));
}