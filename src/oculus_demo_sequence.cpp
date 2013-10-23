// Ŭnicode please 
#include "stdafx.h"
#include "oculus_demo_sequence.h"

#include "OgreOculus.h"
#include "lib.h"
#include "fps_camera_controller.h"

using namespace Ogre;

OculusDemoSequence::OculusDemoSequence() 
	: g_bodyPosition(0,0,0),
	g_flying(false),
	node(nullptr),
	ent(nullptr),
	cam_controller_(new FPSCameraController(0, 90, 0))
{
	Lib::oculus->getCameraNode()->setPosition(0.0f,1.7f,10.0f);

	// Add a light.
	Light *light = Lib::scene_mgr->createLight("light");
	light->setType(Light::LT_DIRECTIONAL);
	light->setDirection(-0.577,-0.577,-0.577);
	light->setDiffuseColour(ColourValue::White);
	Lib::scene_mgr->setAmbientLight(ColourValue(0.4f,0.4f,0.4f));

	ent = Lib::scene_mgr->createEntity("sibenik.mesh");
	node = Lib::scene_mgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);

	ent = Lib::scene_mgr->createEntity("sinbad.mesh");
	ent->getAnimationState("Dance")->setEnabled(true);
	node = Lib::scene_mgr->getRootSceneNode()->createChildSceneNode();
	node->setScale(0.25f,0.25f,0.25f);
	node->setPosition(10.7, 1.25, 0);
	node->yaw(Ogre::Degree(90));
	node->attachObject(ent);

	g_bodyPosition = Vector3(15.8f, cam_controller_->eye_height(), 0.0f);
}

OculusDemoSequence::~OculusDemoSequence() 
{
}

bool OculusDemoSequence::keyPressed(const OIS::KeyEvent &e)
{
	if(Parent::keyPressed(e) == false) {
		return false;
	}

	if(cam_controller_->keyPressed(e) == false) {
		return false;
	}

	if(e.key == OIS::KC_1) {
		g_flying = false;
	}
	if(e.key == OIS::KC_2) {
		g_flying = true;
	}

	return true;
}
bool OculusDemoSequence::keyReleased(const OIS::KeyEvent &e)
{
	if(Parent::keyReleased(e) == false) {
		return false;
	}

	if(cam_controller_->keyReleased(e) == false) {
		return false;
	}


	return true;
}

bool OculusDemoSequence::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool OculusDemoSequence::axisMoved(const OIS::JoyStickEvent &arg, int axis)
{
	return true;
}

bool OculusDemoSequence::frameStarted(const Ogre::FrameEvent& evt)
{
	if(Parent::frameStarted(evt) == false) {
		return false;
	}

	float dt = evt.timeSinceLastFrame;	

	cam_controller_->updateOrientation();
	Vector3 translate = cam_controller_->getTranslate();
	auto g_bodyOrientation = cam_controller_->body_orientation();
	
	g_bodyPosition+= g_bodyOrientation*(translate*dt);

	// Add deltaT to the ninja's animation (animations don't update automatically)
	ent->getAnimationState("Dance")->addTime(dt);
	if(!g_flying) {
		g_bodyPosition.y = cam_controller_->eye_height();
	}

	Lib::oculus->getCameraNode()->setPosition(g_bodyPosition);
	Lib::oculus->getCameraNode()->setOrientation(g_bodyOrientation.toQuaternion() * Lib::oculus->getOrientation());

	return true;
}