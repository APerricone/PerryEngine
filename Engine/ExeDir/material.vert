#version 150

uniform mat4 gl_ModelViewProjectionMatrix;
uniform mat3 gl_NormalMatrix;
	
in vec3 gl_Vertex;
in vec3 gl_Normal;
in vec4 gl_Color;
in vec2 gl_MultiTexCoord0;
in vec4 gl_MultiTexCoord1;

out vec2 TexCoord;
out vec3 FragNormal;
out vec4 FrontColor;
out vec4 SpecularColor;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex,1);
	FragNormal = gl_NormalMatrix * gl_Normal;
	// tan = gl_NormalMatrix * v_tan;
	// bitan = gl_NormalMatrix * v_bitan;

	FrontColor = gl_Color;
	SpecularColor = gl_MultiTexCoord1;
	TexCoord = gl_MultiTexCoord0;
}
