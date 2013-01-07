#version 150

in vec2 gl_Vertex;
in vec2 gl_MultiTexCoord0;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(gl_Vertex,0,1);
 	TexCoord = gl_MultiTexCoord0;
}
