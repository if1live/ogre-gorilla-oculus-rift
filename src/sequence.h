// Ŭnicode please 
#pragma once

typedef enum {
	kSequenceGorillaDemo,
} SequenceType;

class Sequence : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener {
public:
	Sequence() {}
	virtual ~Sequence() {}

	virtual bool frameStarted(const Ogre::FrameEvent& evt) { return true; }
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) { return true; }
	virtual bool frameEnded(const Ogre::FrameEvent& evt) { return true; }

	virtual bool keyPressed(const OIS::KeyEvent &e) { return true; }
	virtual bool keyReleased(const OIS::KeyEvent &e) { return true; }

	virtual bool mouseMoved(const OIS::MouseEvent &arg) { return true; }
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) { return true; }
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) { return true; }

	virtual bool buttonPressed( const OIS::JoyStickEvent &arg, int button ) { return true; }
	virtual bool buttonReleased( const OIS::JoyStickEvent &arg, int button ) { return true; }
	virtual bool axisMoved( const OIS::JoyStickEvent &arg, int axis ) { return true; }
	virtual bool sliderMoved( const OIS::JoyStickEvent &, int index) { return true; }
	virtual bool povMoved( const OIS::JoyStickEvent &arg, int index) { return true; }
	virtual bool vector3Moved( const OIS::JoyStickEvent &arg, int index) { return true; }

	void registerSequence();
	void unregisterSequence();
};

/*
Sequence is minimal interface
BaseSequence is REAL Base Sequence, support camera, console, draw fps,...
*/
class BaseSequence : public Sequence {
public:
	BaseSequence();
	virtual ~BaseSequence();

	virtual bool keyPressed(const OIS::KeyEvent &e);
	virtual bool keyReleased(const OIS::KeyEvent &e);

	virtual bool frameStarted(const Ogre::FrameEvent& evt);

private:
	Gorilla::Layer *fps_layer_;
	Gorilla::Caption *fps_;
	Ogre::Real timer_;
	bool running_;
};