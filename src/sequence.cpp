// Ŭnicode please 
#include "stdafx.h"
#include "sequence.h"
#include "lib.h"
#include "OgreOculus.h"
#include "oculus_demo_sequence.h"
#include "demo_sequence.h"
#include "OgreConsoleForGorilla.h"
#include "gorilla_3d_sequence.h"

static const int console_key = OIS::KC_GRAVE;

Sequence *SequenceFactory::createRaw(SequenceType type) const
{
	switch(type) {
	case kSequenceOculusDemo:
		return new OculusDemoSequence();
	case kSequenceDemo:
		return new DemoSequence();
	case kGorillaDemo:
		return new Gorilla3DSequence();
	default:
		OVR_ASSERT(!"not valid sequence type");
		return nullptr;
	}
}
std::unique_ptr<Sequence> SequenceFactory::create(SequenceType type) const
{
	return std::unique_ptr<Sequence>(createRaw(type));
}

/////////////////////////////////////////////////////////////////////
void Sequence::registerSequence()
{
	Lib::root->addFrameListener(this);
	Lib::keyboard->setEventCallback(this);
	Lib::mouse->setEventCallback(this);
	for(auto joystick : Lib::joystick_list) {
		joystick->setEventCallback(this);
	}
}

void Sequence::unregisterSequence()
{
	for(auto joystick : Lib::joystick_list) {
		joystick->setEventCallback(nullptr);
	}
	Lib::mouse->setEventCallback(nullptr);
	Lib::keyboard->setEventCallback(nullptr);
	Lib::root->removeFrameListener(this);
}

/////////////////////////////////////////////////////////////////////

BaseSequence::BaseSequence()
	: timer_(0),
	fps_layer_(nullptr),
	fps_(nullptr),
	running_(true)
{
	fps_layer_ = Lib::screen_2d->createLayer(14);
	fps_ = fps_layer_->createCaption(14, 10,10, Ogre::StringUtil::BLANK);
}

BaseSequence::~BaseSequence()
{
	Lib::screen_2d->destroy(fps_layer_);
	fps_layer_ = nullptr;
}

bool BaseSequence::keyPressed(const OIS::KeyEvent &e)
{
	if(e.key == OIS::KC_F5) {
		Lib::oculus->setDistortion(false);
		Lib::oculus->setCameraMode(Oculus::kModeNormal);
		return false;
	}
	if(e.key == OIS::KC_F6) {
		Lib::oculus->setDistortion(false);
		Lib::oculus->setCameraMode(Oculus::kModeOculus);
		return false;
	}
	if(e.key == OIS::KC_F7) {
		Lib::oculus->setDistortion(true);
		Lib::oculus->setCameraMode(Oculus::kModeOculus);
		return false;
	}

	if(Lib::console->isVisible() == true) {
		Lib::console->onKeyPressed(e);
		return false;
	}

	if(e.key == OIS::KC_ESCAPE) {
		running_ = false;
	}

	return true;
}
bool BaseSequence::keyReleased(const OIS::KeyEvent &e)
{
	if (e.key == console_key) {
		Lib::console->setVisible(!Lib::console->isVisible());
		return true;
	}
	return true;
}

bool BaseSequence::frameStarted(const Ogre::FrameEvent& evt)
{
	if(running_ == false) {
		return false;
	}

	timer_ += evt.timeSinceLastFrame;
	if (timer_ > 1.0f / 60.0f) {
		timer_ = 0;
		std::stringstream s;
		s << "FPS: " << Lib::window->getLastFPS() << ", Batches: " << Lib::root->getRenderSystem()->_getBatchCount() << "\n";
		fps_->top(Lib::viewport_2d->getActualHeight() - 25);
		fps_->text(s.str());
	}
	return true;
}