#version 150

flat in uvec4 color;
out uvec4 gl_FragData;

void main()
{ 	
	gl_FragData = uvec4(1,1,1,1); //color;
}
