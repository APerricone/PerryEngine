#version 150

uniform mat4 gl_ModelViewProjectionMatrix;
	
in vec3 gl_Vertex;
in vec4 gl_Color;

out vec4 FrontColor;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex,1);
	FrontColor = pow(gl_Color, vec4(2.2) ); // move it in material
}
