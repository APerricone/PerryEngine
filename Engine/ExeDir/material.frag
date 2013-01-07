#version 150

uniform samplerCube bestFitTexture;
uniform sampler2D diffuseTexture;

in vec2 TexCoord;
in vec3 FragNormal;
in vec4 FrontColor;
in vec4 SpecularColor;
out vec4 glFragData[3];

void main()
{
	glFragData[0] = texture(bestFitTexture,FragNormal.xyz);
	vec4 diffuseColor = texture(diffuseTexture,TexCoord);
	// diffuseColor = pow(diffuseColor, vec4(2.2) ); moved in CTexture2d
	glFragData[1] = FrontColor * diffuseColor;
	glFragData[2] = SpecularColor;
}
