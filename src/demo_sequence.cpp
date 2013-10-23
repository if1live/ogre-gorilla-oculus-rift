// Ŭnicode please 
#include "stdafx.h"
#include "demo_sequence.h"
#include "lib.h"

using namespace Ogre;

DemoSequence::DemoSequence()
{
	// create your scene here :)
	//Ogre::Entity *ent = Lib::scene_mgr->createEntity("MyEntity", "ogrehead.mesh");
	//Lib::scene_mgr->getRootSceneNode()->attachObject(ent);

	Ogre::Plane plane(Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		1500, 1500, 20, 20, true, 1, 5, 5, Vector3::UNIT_Z);
	Ogre::Entity *ent1 = Lib::scene_mgr->createEntity("light plane entity", "plane");
	Lib::scene_mgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent1);
	ent1->setMaterialName("Examples/BeachStones");

	Ogre::SceneNode *node = Lib::scene_mgr->createSceneNode("Node1");
	Lib::scene_mgr->getRootSceneNode()->addChild(node);

	Ogre::Light *light1 = Lib::scene_mgr->createLight("light1");
	light1->setType(Ogre::Light::LT_DIRECTIONAL);
	//light1->setPosition(0, 20, 0);
	light1->setDirection(Vector3(1, -1, 0));
	light1->setDiffuseColour(1.0f, 1.0f, 1.0f);

	Ogre::Entity *lightEnt = Lib::scene_mgr->createEntity("MyfdsEntity", "sphere.mesh");
	Ogre::SceneNode *node3 = node->createChildSceneNode("node3");
	node3->setScale(0.1f, 0.1f, 0.1f);
	node3->setPosition(30, 30, 0);
	node3->attachObject(lightEnt);
	lightEnt->setMaterialName("Examples/BeachStones");

	///////////////
	Ogre::SceneNode *root_scene_node = Lib::scene_mgr->getRootSceneNode();
	Ogre::Camera *cam = Lib::scene_mgr->createCamera("PlayerCam");
	Ogre::SceneNode *cam_node = root_scene_node->createChildSceneNode("PlayerCamNode");
	cam_node->attachObject(cam);

	cam->setPosition(Ogre::Vector3(20,50,80));
	cam->lookAt(Ogre::Vector3(0,0,0));


	float viewport_width = 1.0f;
	float viewport_height = 1.0f;
	float viewport_left = (1.0f - viewport_width) * 0.5f;
	float viewport_top = (1.0f - viewport_height) * 0.5f;
	int main_viewport_z_order = 100;
	Ogre::Viewport *vp = Lib::window->addViewport(cam, main_viewport_z_order, viewport_left,
		viewport_top, viewport_width, viewport_height);
	vp->setAutoUpdated(true);
	vp->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 1.0f));

	float ratio = (float)(vp->getActualWidth()) / (float)(vp->getActualHeight());
	cam->setAspectRatio(ratio);

	cam->setNearClipDistance(1.5f);
	cam->setFarClipDistance(3000.0f);

}

DemoSequence::~DemoSequence()
{
}
