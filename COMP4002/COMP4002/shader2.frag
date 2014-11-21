#version 150
 
varying vec2 TextCoord;
out vec4 outputF;
 
uniform sampler2D texUnit;

void main()
{
	vec4 temp = texture(texUnit, TextCoord);
	if (temp.w < 0.9) discard;
    outputF =  texture(texUnit, TextCoord);
}