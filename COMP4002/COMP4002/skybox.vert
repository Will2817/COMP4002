#version 150

attribute vec4 position;		// the vertex position (in the local space) from VBO

varying vec3 rVector;
uniform mat4 mvpMatrix;

void main()
{ 

	rVector = normalize(position.xyz);
    gl_Position = mvpMatrix * position;
}
