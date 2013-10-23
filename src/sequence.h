// Ŭnicode please 
#pragma once

typedef enum {
	kSequenceOculusDemo,
	kSequenceDemo,
	kGorillaDemo,
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
sequence에는 최소한의 인터페이스만 집어넣고
기본적인 키보드 인식(카메라 모드, 콘솔, 디버깅 정보 출력...)등의 용도를 포함한 클래스를 따로 만듬
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

class SequenceFactory {
public:
	Sequence *createRaw(SequenceType type) const;
	std::unique_ptr<Sequence> create(SequenceType type) const;
};