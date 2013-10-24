// Ŭnicode please 
#include "stdafx.h"
#include "gorilla_3d_sequence.h"
#include "lib.h"
#include "OgreOculus.h"
#include "fps_camera_controller.h"

using namespace Ogre;

Gorilla3DSequence::Gorilla3DSequence()
	: mTimer(0),
	mTimer2(0),
	mBasePower(150),
	mNextUpdate(0),
	cam_controller_(new FPSCameraController(0, 0, 0))
{
	setUp();
}

void Gorilla3DSequence::setUp()
{
	Ogre::ColourValue BackgroundColour = Ogre::ColourValue(0.1337f, 0.1337f, 0.1337f, 1.0f);
	Ogre::ColourValue GridColour = Ogre::ColourValue(0.2000f, 0.2000f, 0.2000f, 1.0f);

	body_position_ = Ogre::Vector3(10, cam_controller_->eye_height(), 10);

	mReferenceObject = Lib::scene_mgr->createManualObject("ReferenceGrid");
	mReferenceObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);

	Ogre::Real step = 1.0f;
	unsigned int count = 200;
	unsigned int halfCount = count / 2;
	Ogre::Real full = (step * count);
	Ogre::Real half = full / 2;
	Ogre::Real y = 0;
	Ogre::ColourValue c;
	for (unsigned i=0 ; i < count+1 ; i++)
	{
		if (i == halfCount)
			c = Ogre::ColourValue(0.5f,0.3f,0.3f,1.0f);
		else
			c = GridColour;

		mReferenceObject->position(-half,y,-half+(step*i));
		mReferenceObject->colour(BackgroundColour);
		mReferenceObject->position(0,y,-half+(step*i));
		mReferenceObject->colour(c);
		mReferenceObject->position(0,y,-half+(step*i));
		mReferenceObject->colour(c);
		mReferenceObject->position(half,y,-half+(step*i));
		mReferenceObject->colour(BackgroundColour);
		
		if (i == halfCount)
			c = Ogre::ColourValue(0.3f,0.3f,0.5f,1.0f);
		else
			c = GridColour;

		mReferenceObject->position(-half+(step*i),y,-half);
		mReferenceObject->colour(BackgroundColour);
		mReferenceObject->position(-half+(step*i),y,0);
		mReferenceObject->colour(c);
		mReferenceObject->position(-half+(step*i),y,0);
		mReferenceObject->colour(c);
		mReferenceObject->position(-half+(step*i),y, half);
		mReferenceObject->colour(BackgroundColour);
	}

	mReferenceObject->end();
	Lib::scene_mgr->getRootSceneNode()->attachObject(mReferenceObject);

	createControlPanel();
}

Gorilla3DSequence::~Gorilla3DSequence()
{
	delete(mPowerPanel);
	delete(mPowerDownButton);
	delete(mPowerUpButton);
}

void Gorilla3DSequence::createControlPanel()
{
	mPowerPanel = new D3Panel(Lib::gorilla, Lib::scene_mgr, Ogre::Vector2(4,1));
	mPowerPanel->mNode->setPosition(Ogre::Vector3(0,1.5f,-10));
	Gorilla::Caption* caption = mPowerPanel->makeCaption(0,4, "Power Level");
	caption->width(400);
	caption->align(Gorilla::TextAlign_Centre);

	mPowerValueBackground = mPowerPanel->mGUILayer->createRectangle(10,35,380,10);
	mPowerValueBackground->background_colour(Gorilla::rgb(255,255,255,100));

	mPowerValue = mPowerPanel->mGUILayer->createRectangle(10,35,200,10);
	mPowerValue->background_gradient(Gorilla::Gradient_NorthSouth, Gorilla::rgb(255,255,255,200), Gorilla::rgb(64,64,64,200));
	mPowerDownButton = mPowerPanel->makeButton(10, 65, "-");
	mPowerUpButton = mPowerPanel->makeButton(84, 65, "+");


}

void Gorilla3DSequence::createHUD()
{
	/*
	mHUD = Lib::gorilla->createScreen(mViewport, "dejavu");
	mHUDLayer = mHUD->createLayer();

	Gorilla::Caption* fakeHealth = mHUDLayer->createCaption(24, 0, 0, "+ 100");
	fakeHealth->width(mViewport->getActualWidth()-16);
	fakeHealth->height(mViewport->getActualHeight()-4);
	fakeHealth->align(Gorilla::TextAlign_Right);
	fakeHealth->vertical_align(Gorilla::VerticalAlign_Bottom);

	Gorilla::Caption* fakeAmmo = mHUDLayer->createCaption(24, 16, 0, ": 60");
	fakeAmmo->width(mViewport->getActualWidth());
	fakeAmmo->height(mViewport->getActualHeight()-4);
	fakeAmmo->vertical_align(Gorilla::VerticalAlign_Bottom);

	mCrosshairLayer = mHUD->createLayer();
	mCrosshair = mCrosshairLayer->createRectangle((mViewport->getActualWidth() * 0.5f) - 11, (mViewport->getActualHeight() * 0.5f) - 11, 22, 22);
	mCrosshair->background_image("crosshair");
	*/
} 

bool Gorilla3DSequence::frameStarted(const Ogre::FrameEvent& evt)
{
	if(!Parent::frameStarted(evt)) {
		return false;
	}

	auto mCamera = Lib::oculus->getCameraNode();
	auto cam = Lib::oculus->getCamera(Oculus::kCameraFull);

	cam_controller_->updateOrientation();
	Vector3 translate = cam_controller_->getTranslate();
	auto body_orientation = cam_controller_->body_orientation();
	body_position_ += body_orientation*(translate*evt.timeSinceLastFrame);
	body_position_.y = cam_controller_->eye_height();	
	Lib::oculus->getCameraNode()->setPosition(body_position_);
	Lib::oculus->getCameraNode()->setOrientation(body_orientation.toQuaternion() * Lib::oculus->getOrientation());

	
	mTimer += evt.timeSinceLastFrame;

	if (mTimer > 1.0f / 60.0f) {
		mPowerPanel->mNode->yaw(Ogre::Radian(0.0005));
		mTimer2 += Ogre::Math::RangeRandom(0, mTimer * 10);
		mTimer = 0;
		Ogre::Math::Clamp<Ogre::Real>(mTimer2, 0, 25);
		Ogre::Real power = mBasePower + (Ogre::Math::Cos(mTimer2) * 5);
		power = Ogre::Math::Clamp<Ogre::Real>(power, 0, 380);
		mPowerValue->width(power);
	}
	bool isOver = false;

	Button* button = mPowerPanel->check( cam->getCameraToViewportRay(0.5f,0.5f), isOver );

	bool btn_press = Lib::mouse->getMouseState().buttonDown(OIS::MB_Left);
	if(Lib::main_joystick() != nullptr) {
		auto joystick_state = Lib::main_joystick()->getJoyStickState();
		btn_press = btn_press || joystick_state.mButtons[0];
	}
	
	if (button != 0 && btn_press) {
		if (button == mPowerDownButton) {
			mBasePower -= 1.0f;
		}
		if (button == mPowerUpButton) {
			mBasePower += 1.0f;
		}
	}

	return true;
}


bool Gorilla3DSequence::mouseMoved( const OIS::MouseEvent &arg )
{
	return true;
}

bool Gorilla3DSequence::keyPressed(const OIS::KeyEvent &e)
{
	if(Parent::keyPressed(e) == false) {
		return false;
	}
	if(cam_controller_->keyPressed(e) == false) {
		return false;
	}
	return true;
}
bool Gorilla3DSequence::keyReleased(const OIS::KeyEvent &e)
{
	if(Parent::keyReleased(e) == false) {
		return false;
	}
	if(cam_controller_->keyReleased(e) == false) {
		return false;
	}
	return true;
}