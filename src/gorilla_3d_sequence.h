// Ŭnicode please 
#pragma once

#include "sequence.h"

struct Button
{

	Button(Ogre::Real x, Ogre::Real y, const Ogre::String& text, Gorilla::Layer* layer)
		: hovered(false)
	{
		caption = layer->createCaption(14, x,y, text);
		caption->size(64,25);
		caption->align(Gorilla::TextAlign_Centre);
		caption->vertical_align(Gorilla::VerticalAlign_Middle);
		caption->background(Gorilla::rgb(255,255,255,32));
	}

	bool isOver(const Ogre::Vector2& pos)
	{
		bool result = caption->intersects(pos);
		if (result && !hovered)
			caption->background(Gorilla::rgb(255,255,255,128));
		else if (!result && hovered)
			caption->background(Gorilla::rgb(255,255,255,32));
		hovered = result;
		return result;
	}

	bool                 hovered;
	Gorilla::Caption*    caption;

};

struct D3Panel
{

	D3Panel(Gorilla::Silverback* silverback, Ogre::SceneManager* sceneMgr, const Ogre::Vector2& size)
		: mSize(size)
	{

		mScreen = silverback->createScreenRenderable(Ogre::Vector2(mSize.x,mSize.y), "dejavu");
		mNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
		mNode->attachObject(mScreen);

		mGUILayer = mScreen->createLayer(0);
		mBackground = mGUILayer->createRectangle(0,0, mSize.x * 100, mSize.y * 100);
		mBackground->background_gradient(Gorilla::Gradient_NorthSouth, Gorilla::rgb(94,97,255,5), Gorilla::rgb(94,97,255,50));
		mBackground->border(2, Gorilla::rgb(255,255,255,150));

		mMousePointerLayer = mScreen->createLayer(15);
		mMousePointer = mMousePointerLayer->createRectangle(0,0,10,18);
		mMousePointer->background_image("mousepointer");

	}

	~D3Panel()
	{
		mScreen->destroy(mGUILayer);
		mScreen->destroy(mMousePointerLayer);
	}

	Button* check(const Ogre::Ray& ray, bool& isOver)
	{

		isOver = false;

		Ogre::Matrix4 transform;
		transform.makeTransform(mNode->getPosition(), mNode->getScale(), mNode->getOrientation());

		Ogre::AxisAlignedBox aabb = mScreen->getBoundingBox();
		aabb.transform(transform);
		std::pair<bool, Ogre::Real> result = Ogre::Math::intersects(ray, aabb);

		if (result.first == false)
			return 0;

		Ogre::Vector3 a,b,c,d;
		Ogre::Vector2 halfSize = mSize * 0.5f;
		a = transform * Ogre::Vector3(-halfSize.x,-halfSize.y,0);
		b = transform * Ogre::Vector3( halfSize.x,-halfSize.y,0);
		c = transform * Ogre::Vector3(-halfSize.x, halfSize.y,0);
		d = transform * Ogre::Vector3( halfSize.x, halfSize.y,0);

		result = Ogre::Math::intersects(ray, c, b, a);
		if (result.first == false)
			result = Ogre::Math::intersects(ray, c, d, b);
		if (result.first == false)
			return 0;

		if (result.second > 6.0f)
			return 0;

		isOver = true;

		Ogre::Vector3 hitPos = ( ray.getOrigin() + (ray.getDirection() * result.second) );
		Ogre::Vector3 localPos = transform.inverse() * hitPos;
		localPos.x += halfSize.x;
		localPos.y -= halfSize.y;
		localPos.x *= 100;
		localPos.y *= 100;

		// Cursor clip
		localPos.x = Ogre::Math::Clamp<Ogre::Real>(localPos.x, 0, (mSize.x * 100) - 10);
		localPos.y = Ogre::Math::Clamp<Ogre::Real>(-localPos.y, 0, (mSize.y * 100) - 18);

		mMousePointer->position(localPos.x, localPos.y);

		for (size_t i=0;i < mButtons.size();i++)
		{
			if (mButtons[i]->isOver(mMousePointer->position()))
				return mButtons[i];
		}

		return 0;
	}

	Gorilla::Caption* makeCaption(Ogre::Real x, Ogre::Real y, const Ogre::String& text)
	{
		return mGUILayer->createCaption(14, x, y, text);
	}

	Button* makeButton(Ogre::Real x, Ogre::Real y, const Ogre::String& text)
	{
		Button* button = new Button(x,y, text, mGUILayer);
		mButtons.push_back(button);
		return button;
	}

	Gorilla::ScreenRenderable*     mScreen;
	Ogre::SceneNode*     mNode;
	Gorilla::Layer*      mGUILayer, *mMousePointerLayer;
	Gorilla::Rectangle*  mBackground, *mMousePointer;
	Ogre::Vector2        mSize;

	std::vector<Button*> mButtons;

};

class FPSCameraController;

class Gorilla3DSequence : public BaseSequence {
public:
	typedef BaseSequence Parent;

public:
	Gorilla3DSequence();
	virtual ~Gorilla3DSequence();

	void setUp();

	void createHUD();
	void createControlPanel();

	virtual bool frameStarted(const Ogre::FrameEvent& evt);

	virtual bool mouseMoved( const OIS::MouseEvent &arg );

	virtual bool keyPressed(const OIS::KeyEvent &e);
	virtual bool keyReleased(const OIS::KeyEvent &e);

private:
	Ogre::Real              mTimer, mTimer2;
	D3Panel*                mPowerPanel;
	Gorilla::Rectangle*     mPowerValue, *mPowerValueBackground;
	Button*                 mPowerUpButton;
	Button*                 mPowerDownButton;
	Ogre::Real              mBasePower;
	Ogre::SceneNode*        mNode;

	Ogre::Real              mNextUpdate;
	Ogre::ManualObject*     mReferenceObject;

	Ogre::Vector3 body_position_;
	std::unique_ptr<FPSCameraController> cam_controller_;
};