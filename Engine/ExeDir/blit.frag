#version 150

uniform sampler2D image;

in vec2 TexCoord;
out vec4 gl_FragData;

void main()
{ 	
	gl_FragData = texture(image, TexCoord.xy);
}
