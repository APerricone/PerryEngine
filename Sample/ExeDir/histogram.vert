#version 150

in vec2 gl_Vertex;
in vec4 gl_MultiTexCoord0;
out vec4 texCoords;

void main()
{
	texCoords = gl_MultiTexCoord0;
	gl_Position = vec4(gl_Vertex,0,1);
}
