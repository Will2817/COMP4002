#version 150
 
varying vec2 TextCoord;
out vec4 outputF;
 
uniform sampler2D texUnit;
uniform vec4 eyePosition;

varying vec4 worldposition;
varying vec4 worldnormal;

void main()
{
	vec4 rcolor = texture(texUnit, TextCoord);
	if (rcolor.w < 0.3) discard;
	outputF = rcolor;
}