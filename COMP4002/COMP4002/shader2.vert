#version 150
 
uniform mat4 mvpMatrix;
 
in vec4 position;
in vec2 textCoord;
 
varying vec2 TextCoord;
 
void main()
{
    TextCoord = textCoord;
	
    gl_Position = mvpMatrix * position;
}