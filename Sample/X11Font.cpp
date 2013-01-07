/*
 *		This Code Was Created By Jeff Molofee 2000
 *		Modified by Shawn T. to handle (%3.2f, num) parameters.
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing The Base Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */
/// see http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=13
#include "font.h"

#include <GL/gl.h>
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <GL/glx.h>     // Header file fot the glx libraries.
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

PFNGLPRINT glPrint;

GLuint	base;				// Base Display List For The Font Set
void StdGlPrint(float x,float y,const char *fmt, ...);// Custom GL "Print" Routine

GLuint	base;				// Base Display List For The Font Set

void BuildFont()								// Build Our Bitmap Font
{
    Display *dpy;
    XFontStruct *fontInfo;  // storage for our font.

    base = glGenLists(96);                      // storage for 96 characters.

    // load the font.  what fonts any of you have is going
    // to be system dependent, but on my system they are
    // in /usr/X11R6/lib/X11/fonts/*, with fonts.alias and
    // fonts.dir explaining what fonts the .pcf.gz files
    // are.  in any case, one of these 2 fonts should be
    // on your system...or you won't see any text.

    // get the current display.  This opens a second
    // connection to the display in the DISPLAY environment
    // value, and will be around only long enough to load
    // the font.
    dpy = XOpenDisplay(NULL); // default to DISPLAY env.

	const char* names[] =
	{
		"-*-courier-medium-r-normal-*-12-*-*-*-*-*-iso8859-1",
		"-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-iso8859-1",
		"-*-fixed-medium-r-normal-*-12*-*-*-*-*-iso8859-1",
		"fixed"
	};
	for(int i=0;i<sizeof(names)/sizeof(char*);i++)
	{
		fontInfo = XLoadQueryFont(dpy, names[i]);
		if(fontInfo != NULL) break;
	}
	if (fontInfo == NULL) {
	    printf("no X font available?\n");
	    return;
    }

    // after loading this font info, this would probably be the time
    // to rotate, scale, or otherwise twink your fonts.

    // start at character 32 (space), get 96 characters (a few characters past z), and
    // store them starting at base.
    glXUseXFont(fontInfo->fid, 32, 96, base);

    // free that font's info now that we've got the
    // display lists.
    XFreeFont(dpy, fontInfo);

    // close down the 2nd display connection.
    XCloseDisplay(dpy);

	glPrint = StdGlPrint;
}

void KillFont()									// Delete The Font List
{
	glDeleteLists(base, 96);							// Delete All 96 Characters
}

void StdGlPrint(float x,float y,const char *fmt, ...)					// Custom GL "Print" Routine
{
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	glRasterPos2f(x,y);

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32

	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
}
