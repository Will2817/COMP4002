#version 150

varying vec3 rVector;
uniform samplerCube texCube;

void main() 
{ 
	gl_FragColor = texture(texCube, rVector); 
 } 
