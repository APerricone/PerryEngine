#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "config.h"
#include "opengl.h"
#include "game.h"

#include "scene.h"

#include "mymacros.h"

#include <GL/glfw.h>

#ifdef _WIN32
#include <windows.h>
#endif //_WIN32

#include "log.h"

class Log : public ILog
{
protected:
	virtual void InternalMessage(const char* i_pMessage, va_list args)
	{
		vprintf(i_pMessage, args);
	}

	virtual void InternalError(const char* i_pMessage, va_list args)
	{
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		printf("ERROR:");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
#else
		printf("\033[22;31m ERROR: \033[01;37m");
#endif //
		vprintf(i_pMessage, args);
	}
} myLog;

void GLFWCALL resizeCB( int width, int height )
{
	CGame::GetInstance().Resize(width,height);
}

static bool externalRunning = true;
int GLFWCALL closeGame(  )
{
	externalRunning = false;
	return GL_TRUE;
}

void closeSignal(int s)
{
	externalRunning = false;
}

int main (int argc, char *argv[])
{
	ILog::SetLog( &myLog );
	if(glfwInit() == GL_FALSE )
		return -1;
	int maj,min,rev;
	glfwGetVersion(&maj,&min,&rev );
	ILog::Message("Initialized GLFW v.%i.%i.%i\n", maj,min,rev);
	int w=800,h=600;
#ifdef WANT_VIDEO
	w=VIDEO_WIDTH;
	h=VIDEO_HEIGHT;
#endif
	if(glfwOpenWindow(w,h,0,0,0,0,24,0,GLFW_WINDOW) == GL_FALSE )
	{
		glfwTerminate();
	}

	if( !CScene::InitStatic() )
	{
		glfwTerminate();
		return 1;
	}
#ifdef _WIN32
	signal(SIGBREAK,closeSignal);	//CTRL+Break (or close windows on MSWindows)
#endif //_WIN32
	signal(SIGINT,closeSignal);		//CTRL+C

	CGame& game = CGame::GetInstance();
	game.Init();
	glfwSetWindowSizeCallback( resizeCB );
	//glfwSetWindowCloseCallback( closeGame );
	glfwSetWindowTitle(PROJ_NAME_STR);

	bool running = true;
	double lastTime = glfwGetTime();
	double thisTime=lastTime;

	// Main loop
	while( running )
	{
		lastTime = thisTime;
		thisTime = glfwGetTime();
		game.SetLastFrameTime( thisTime-lastTime );

		running = game.Update();

		if(!running) break;
		if(!externalRunning) break;
		game.Draw();
		// Check if ESC key was pressed or window was closed
		running = !glfwGetKey( GLFW_KEY_ESC ) &&
				glfwGetWindowParam( GLFW_OPENED );

	}
	game.Deinit();
	glfwTerminate();

	return 0;
}
