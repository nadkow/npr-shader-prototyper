#version 460 core

in vec3 TexCoords; // direction vector representing a 3D texture coordinate
uniform samplerCube cubemap;
uniform vec4 skycolor;

out vec4 FragColor;

void main()
{
    FragColor =  skycolor * texture(cubemap, TexCoords);
}