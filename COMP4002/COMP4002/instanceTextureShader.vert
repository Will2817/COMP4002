#version 330
 
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 textCoord;
layout (location = 2) in vec4 normal;
layout (location = 3) in mat4 instanceMatrix;

uniform mat4 modelMatrix;
uniform mat4 mvpMatrix;

varying vec2 TextCoord;
varying vec4 worldposition;
varying vec4 worldnormal;
flat out int InstanceID; 
 
void main()
{
    gl_Position = mvpMatrix * transpose(instanceMatrix) * modelMatrix * position;
    TextCoord = textCoord;

	worldposition = transpose(instanceMatrix) * modelMatrix * position;
	worldnormal = transpose(instanceMatrix) * modelMatrix * vec4(normal.x,normal.y,normal.z,0);
	worldnormal = vec4(worldnormal.x,worldnormal.y,worldnormal.z,0);
	worldnormal = normalize(worldnormal);
	InstanceID = gl_InstanceID;
}