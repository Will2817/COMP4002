#version 150
 
uniform mat4 mvpMatrix;
uniform mat4 modelMatrix; 

in vec4 position;
in vec2 textCoord;
in vec4 normal;
 
varying vec2 TextCoord;
varying vec4 worldposition;
varying vec4 worldnormal;
 
void main()
{
    TextCoord = textCoord;
	
    gl_Position = mvpMatrix * position;

	worldposition = modelMatrix * position;
	worldnormal = modelMatrix * vec4(normal.x,normal.y,normal.z,0);
	worldnormal = vec4(worldnormal.x,worldnormal.y,worldnormal.z,0);
	worldnormal = normalize(normal);
}