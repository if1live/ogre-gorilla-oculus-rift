// Ŭnicode please 
#include "entrypoint.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#if defined(_DEBUG) && OGRE_PLATFORM == OGRE_PLATFORM_WIN32 && !defined(__GNUWIN32__) && !defined(_WIN32_WCE)
#define USE_MEMORY_LEAK_DETECT 1
#else
#define USE_MEMORY_LEAK_DETECT 0
#endif

#if USE_MEMORY_LEAK_DETECT
#include <crtdbg.h>
#endif


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
int main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
#if USE_MEMORY_LEAK_DETECT
	//detect memory leak
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	//_CrtSetBreakAlloc(126901);
#endif

	int exit_code = entrypoint();
	return exit_code;
}