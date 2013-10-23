// Ŭnicode please 
#pragma once

#include "sequence.h"
#include "euler.h"

class FPSCameraController;

class OculusDemoSequence : public BaseSequence {
public:
	typedef BaseSequence Parent;
		
public:
	OculusDemoSequence();
	virtual ~OculusDemoSequence();

	virtual bool frameStarted(const Ogre::FrameEvent& evt);

	virtual bool keyPressed(const OIS::KeyEvent &e);
	virtual bool keyReleased(const OIS::KeyEvent &e);

	virtual bool mouseMoved(const OIS::MouseEvent &arg);

	virtual bool axisMoved(const OIS::JoyStickEvent &arg, int axis);

private:
	Ogre::Vector3 g_bodyPosition;
	bool g_flying;
	
	// Add the level mesh.
	Ogre::SceneNode *node;
	Ogre::Entity *ent;

	std::unique_ptr<FPSCameraController> cam_controller_;
};

