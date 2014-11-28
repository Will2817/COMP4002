#version 150
 
varying vec2 TextCoord;
varying float yvalue;
out vec4 outputF;
 
uniform sampler2D texUnit;

void main()
{
	vec4 temp = texture(texUnit, TextCoord);
	if (temp.w < 0.9) discard;
	vec4 temp2= texture(texUnit, TextCoord) * yvalue;
    temp2.w = 1;
	//outputF = temp2;
	outputF = vec4(yvalue,yvalue,yvalue,1);
}