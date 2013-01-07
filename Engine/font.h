#pragma once

#include "opengl.h"

typedef void (GLAPIENTRY * PFNGLPRINT) (GLfloat x, GLfloat y, const GLchar* fmt, ... );
extern PFNGLPRINT glPrint;
