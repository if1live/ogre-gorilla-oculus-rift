// Ŭnicode please 
#pragma once

#include "euler.h"


struct MoveEvent {
	MoveEvent() : forward(0.0f), strafe(0.0f) {}
	MoveEvent(float forward, float strafe) : forward(forward), strafe(strafe) { validate(); }

	float forward;
	float strafe;

	MoveEvent operator+(const MoveEvent &o) const;
	MoveEvent &operator+=(const MoveEvent &o);
	void validate();
};


/*
FPS시점을 조작하기 위한 용도의 클래스이다.
플레이어가 처다보는 방향와 이동할 방향을 통제한다.
플레이어의 실제 위치는 여기에서 통제하지 않는다
*/
class FPSCameraController : public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener {
public:
	FPSCameraController(float pitch=0.0f, float yaw=0.0f, float roll=0.0f);
	virtual ~FPSCameraController();

	virtual bool keyPressed(const OIS::KeyEvent &e);
	virtual bool keyReleased(const OIS::KeyEvent &e);

	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	virtual bool buttonPressed( const OIS::JoyStickEvent &arg, int button );
	virtual bool buttonReleased( const OIS::JoyStickEvent &arg, int button );
	virtual bool axisMoved( const OIS::JoyStickEvent &arg, int axis );
	virtual bool sliderMoved( const OIS::JoyStickEvent &, int index);
	virtual bool povMoved( const OIS::JoyStickEvent &arg, int index);
	virtual bool vector3Moved( const OIS::JoyStickEvent &arg, int index);

public:
	float eye_height() const { return eye_height_; }
	float walk_speed() const { return walk_speed_; }
	float turn_speed_mouse() const { return turn_speed_mouse_; }
	float turn_speed_joystick() const { return turn_speed_joystick_; }

	Ogre::Vector3 getTranslate() const;
	void updateOrientation();
	const Ogre::Euler &body_orientation() const { return body_orientation_; }

public:
	MoveEvent getMoveEventByKeyboard() const;
	MoveEvent getMoveEventByJoyStick() const;

private:
	OIS::KeyCode key_forward_;
	OIS::KeyCode key_backward_;
	OIS::KeyCode key_left_;
	OIS::KeyCode key_right_;

	float eye_height_;
	float walk_speed_;
	float turn_speed_mouse_;
	float turn_speed_joystick_;

	Ogre::Euler body_orientation_;
};
