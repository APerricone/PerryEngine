#version 150

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D shadowTexture;

uniform vec4 lightDirPosType; 	// w:0 point 1 directional
uniform vec3 lightCol; 
uniform mat4 lightMatrix;
uniform mat4 invProjMatrix;
uniform vec3 camPos;

in vec2 TexCoord;
out vec4 gl_FragData;

