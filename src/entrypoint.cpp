// Ŭnicode please 
#include "stdafx.h"
#include "entrypoint.h"
#include "lib.h"

#include "OgreOculus.h"
#include "euler.h"

#include "sequence.h"

#include "device_plug_and_play.h"
#include "latency_util.h"

#include "gorilla_3d_sequence.h"

using namespace Ogre;

int entrypoint_run_ogre();
int entrypoint_libovr();

int entrypoint()
{	
	return entrypoint_run_ogre();
}

int entrypoint_run_ogre()
{
	bool init_result = Lib::startUp(USE_ALL);
	if(init_result == false) {
		Lib::shutDown();
		return 0;
	}
	
	{
		Gorilla3DSequence sequence;
		sequence.registerSequence();

		//Lib::oculus->setCameraMode(Oculus::kModeNormal);

		// cleaning of windows events managed by Ogre::WindowEventUtilities::...
		// I call it after a 'pause in window updating', in order to maintain smoothness.
		// Explanation : if you clicked 2000 times when the windows was being created, there are 
		// at least 2000 messages created by the OS to listen to. This is made to clean them.
		Lib::root->clearEventTimes();

		while (!Lib::window->isClosed()) {
			// Check if any new devices were connected.
			Lib::device_pnp->update();
			Lib::latency_util->update();

			// process input before update
			Lib::captureInput();

			// Check for windows messages
			Ogre::WindowEventUtilities::messagePump();

			// Render the scene
			bool next = Lib::root->renderOneFrame();
			if (next == false) {
				break;
			}

			// Perform a 1ms sleep. This stops the app from hogging the cpu, and also stops some gfx cards (like my previous one) from overheating
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			Sleep(1);
#else
			sleep(1);
#endif
		}

		sequence.unregisterSequence();
	}
	
	Lib::shutDown();

    return 0;
}
