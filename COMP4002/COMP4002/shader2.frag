#version 150
 
varying vec2 TextCoord;
out vec4 outputF;
 
uniform sampler2D texUnit;

void main()
{
    outputF =  texture(texUnit, TextCoord);
}