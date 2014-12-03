#version 330
 
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 textCoord;
//layout (location = 2) in vec4 normal;
layout (location = 3) in mat4 mvpMatrix;

uniform mat4 modelMatrix;

varying vec2 TextCoord;
//varying vec4 worldposition;
//varying vec4 worldnormal;
flat out int InstanceID; 
 
void main()
{
    gl_Position = transpose(mvpMatrix) * position;
    TextCoord = textCoord;

	//worldposition = vec4(0,0,0,1);
	//worldnormal = vec4(0,1,0,1);
	InstanceID = gl_InstanceID;
}