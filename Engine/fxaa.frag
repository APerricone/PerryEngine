#define FXAA_PC 1
#define FXAA_GLSL_130 1
#define FXAA_DISCARD 0
#define FXAA_LINEAR 0
#define FXAA_GATHER4_ALPHA 0
#define FXAA_CONSOLE__EDGE_THRESHOLD 0.25
#define FXAA_QUALITY__EDGE_THRESHOLD (1.0/8.0)
#define FXAA_QUALITY__EDGE_THRESHOLD_MIN (1.0/16.0)
#define FXAA_QUALITY__SUBPIX_TRIM (1.0/2.0)

#include "Fxaa3_11.h"

uniform sampler2D image;
uniform vec2 rcpFrame; //see Fxaa3_8.h

in vec2 TexCoord;
out vec4 gl_FragData;

void main()
{
	const vec4 posPos = vec4(0);
	gl_FragData = FxaaPixelShader(
		TexCoord,vec4(0),
		image,image,image,
		rcpFrame,
		vec4(0),vec4(0),vec4(0),
		0.50,0.166,0.0833,
		8.0,0.125,0.05,
		vec4(1.0, -1.0, 0.25, -0.25)
	);
}
