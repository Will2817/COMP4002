#version 150
 
varying vec2 TextCoord;
out vec4 outputF;
 
uniform sampler2D texUnit;
uniform vec4 eyePosition;

varying vec4 worldposition;
varying vec4 worldnormal;
flat in int InstanceID;


void main()
{
	vec4 temp = texture(texUnit, TextCoord);
	if (temp.w < 0.3) discard;
	
	vec4 lightPos = vec4(5000,5000,-5000,1);
	vec4 L = lightPos - worldposition;
	L = normalize(L);
	vec4 norm = normalize(worldnormal);
	
	vec4 rcolor = temp * vec4(0.3,0.3,0.3,1);
	rcolor = rcolor + temp * vec4(1,1,1,1) * max(dot(norm,L),0.0f);
	rcolor = clamp(rcolor,0,1);
	outputF = rcolor;
}