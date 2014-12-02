#version 330
 
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 textCoord;
layout (location = 2) in mat4 mvpMatrix;
 
varying vec2 TextCoord;
varying float yvalue;
 
void main()
{
    gl_Position = transpose(mvpMatrix) * position;
	yvalue = position.y;
    TextCoord = textCoord;
}