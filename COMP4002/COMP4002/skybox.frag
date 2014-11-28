#version 150

varying vec3 rVector;
uniform samplerCube texCube;

void main() 
{ 
	gl_FragColor = vec4(rVector,1);//texture(texCube, rVector); 
 } 
