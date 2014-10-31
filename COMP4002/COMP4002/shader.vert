#version 150
 
uniform mat4 mvpMatrix;
 
in vec4 position;
in vec3 color;
 
out vec3 Color;
 
void main()
{
    Color = color;
    gl_Position = mvpMatrix * position;
}