// Ŭnicode please 
#include "stdafx.h"
#include "fps_camera_controller.h"
#include "lib.h"
#include "OgreOculus.h"

using namespace Ogre;

const float DEAD_ZONE = 0.20f;
const float AXIS_RANGE = (float)SHRT_MAX;

void MoveEvent::validate()
{
	if(forward >= 1.0f) {
		forward = 1.0f;
	}
	if(forward <= -1.0f) {
		forward = -1.0f;
	}
	if(strafe >= 1.0f) {
		strafe = 1.0f;
	}
	if(strafe <= -1.0f) {
		strafe = -1.0f;
	}
}

MoveEvent &MoveEvent::operator+=(const MoveEvent &o)
{
	this->forward += o.forward;
	this->strafe += o.strafe;
	this->validate();
	return *this;
}
MoveEvent MoveEvent::operator+(const MoveEvent &o) const
{
	MoveEvent tmp = *this;
	tmp += o;
	return tmp;
}

FPSCameraController::FPSCameraController(float pitch, float yaw, float roll)
	: key_forward_(OIS::KC_W),
	key_backward_(OIS::KC_S),
	key_left_(OIS::KC_A),
	key_right_(OIS::KC_D),
	eye_height_(1.7f),
	walk_speed_(5.0f),
	turn_speed_mouse_(0.003f),
	turn_speed_joystick_(0.02f)
{
	body_orientation_.pitch(Ogre::Degree(pitch));
	body_orientation_.yaw(Ogre::Degree(yaw));
	body_orientation_.roll(Ogre::Degree(roll));
}

FPSCameraController::~FPSCameraController()
{
}
bool FPSCameraController::keyPressed(const OIS::KeyEvent &e)
{
	if(e.key == OIS::KC_R) {
		body_orientation_.setPitch(Ogre::Radian(0)).setRoll(Ogre::Radian(0));
	}
	return true;
}
bool FPSCameraController::keyReleased(const OIS::KeyEvent &e)
{
	return true;
}

bool FPSCameraController::mouseMoved(const OIS::MouseEvent &arg) 
{
	return true;
}
bool FPSCameraController::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}
bool FPSCameraController::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool FPSCameraController::buttonPressed( const OIS::JoyStickEvent &arg, int button )
{
	return true;
}
bool FPSCameraController::buttonReleased( const OIS::JoyStickEvent &arg, int button )
{
	return true;
}
bool FPSCameraController::axisMoved( const OIS::JoyStickEvent &arg, int axis )
{
	return true;
}
bool FPSCameraController::sliderMoved( const OIS::JoyStickEvent &, int index) 
{
	return true; 
}
bool FPSCameraController::povMoved( const OIS::JoyStickEvent &arg, int index) 
{
	return true; 
}
bool FPSCameraController::vector3Moved( const OIS::JoyStickEvent &arg, int index)
{
	return true; 
}

void FPSCameraController::updateOrientation()
{
	auto mouse_state = Lib::mouse->getMouseState();
	body_orientation_.yaw(Radian(-mouse_state.X.rel * turn_speed_mouse()));
	if(!Lib::oculus->isOculusReady()) {
		body_orientation_.pitch(Radian(-mouse_state.Y.rel * turn_speed_mouse()));
		body_orientation_.limitPitch(Ogre::Degree(90));
	}

	if(Lib::main_joystick() == nullptr) {
		return;
	}

	auto joystick = Lib::main_joystick();
	auto joystick_state = joystick->getJoyStickState();
	float v_turn = joystick_state.mAxes[2].abs / AXIS_RANGE;
	float h_turn= joystick_state.mAxes[3].abs / AXIS_RANGE;
	if(fabs(h_turn) < DEAD_ZONE) {
		h_turn = 0.0f;
	}
	if(fabs(v_turn) < DEAD_ZONE) {
		v_turn = 0.0f;
	}

	body_orientation_.yaw(-Radian(h_turn * turn_speed_joystick()));
	if(!Lib::oculus->isOculusReady()) {
		body_orientation_.pitch(-Radian(v_turn * turn_speed_joystick()));
		body_orientation_.limitPitch(Ogre::Degree(90));
	}

	//OVR::LogText("%f, %f\n", h_turn, v_turn);
}


Ogre::Vector3 FPSCameraController::getTranslate() const
{
	Ogre::Vector3 translate(0, 0, 0);

	MoveEvent move_evt;
	move_evt += getMoveEventByJoyStick();
	move_evt += getMoveEventByKeyboard();
	
	translate.z += move_evt.forward * (-walk_speed_);
	translate.x += move_evt.strafe * walk_speed_;
	return translate;
}

MoveEvent FPSCameraController::getMoveEventByJoyStick() const
{
	MoveEvent evt;
	if(Lib::main_joystick() == nullptr) {
		return evt;
	}

	auto joystick = Lib::main_joystick();
	auto state = joystick->getJoyStickState();

	
	float x_move = state.mAxes[1].abs / AXIS_RANGE;
	float y_move = state.mAxes[0].abs / AXIS_RANGE;
	if(fabs(x_move) < DEAD_ZONE) {
		x_move = 0.0f;
	}
	if(fabs(y_move) < DEAD_ZONE) {
		y_move = 0.0f;
	}
	//앞으로 가는건 방향이 반대더라
	evt.forward = -y_move;
	evt.strafe = x_move;

	//OVR::LogText("%f, %f\n", evt.forward, evt.strafe);
	return evt;
}

MoveEvent FPSCameraController::getMoveEventByKeyboard() const
{
	MoveEvent evt;
	if(Lib::keyboard->isKeyDown(key_forward_)) {
		evt.forward = 1.0f;
	}
	if(Lib::keyboard->isKeyDown(key_backward_)) {
		evt.forward = -1.0f;
	}
	if(Lib::keyboard->isKeyDown(key_left_)) {
		evt.strafe = -1.0f;
	}
	if(Lib::keyboard->isKeyDown(key_right_)) {
		evt.strafe = 1.0f;
	}
	return evt;
}

