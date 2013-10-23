/// Copyright (C) 2013 Kojack
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
/// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
/// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
/// DEALINGS IN THE SOFTWARE.

#include "stdafx.h"

#include "OgreOculus.h"
#include "OVR.h"
#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"
#include "OgreCompositorManager.h"
#include "OgreCompositorInstance.h"
#include "OgreCompositionTargetPass.h"
#include "OgreCompositionPass.h"
#include "lib.h"

using namespace OVR;

namespace
{
	const float g_defaultNearClip = 0.01f;
	const float g_defaultFarClip = 10000.0f;
	const float g_defaultIPD = 0.064f;
	const Ogre::ColourValue g_defaultViewportColour(0.1337f, 0.1337f, 0.1337f, 1.0f);
	const float g_defaultProjectionCentreOffset = 0.14529906f;
	const float g_defaultDistortion[4] = {1.0f, 0.22f, 0.24f, 0};
}


Oculus::Oculus(void):m_ogreReady(false),
					 m_centreOffset(g_defaultProjectionCentreOffset),
					 m_window(0),
					 m_sceneManager(0),
					 m_cameraNode(0)
{
	for(int i=0;i<kCameraCount;++i)
	{
		m_cameras[i] = 0;
		m_viewports[i] = 0;
		m_compositors[i] = 0;
	}
}

Oculus::~Oculus(void)
{
	shutDownOgre();
	shutDownOculus();
}

void Oculus::shutDownOculus()
{
	//System::Destroy();
}

void Oculus::shutDownOgre()
{
	m_ogreReady = false;
	for(int i=0;i<2;++i)
	{
		if(m_compositors[i])
		{
			Ogre::CompositorManager::getSingleton().removeCompositor(m_viewports[i], "Oculus");
			m_compositors[i] = 0;
		}
		if(m_viewports[i])
		{
			m_window->removeViewport(i);
			m_viewports[i] = 0;
		}
		if(m_cameras[i])
		{
			m_cameras[i]->getParentSceneNode()->detachObject(m_cameras[i]);
			m_sceneManager->destroyCamera(m_cameras[i]);
			m_cameras[i] = 0;
		}
	}
	if(m_cameraNode)
	{
		m_cameraNode->getParentSceneNode()->removeChild(m_cameraNode);
		m_sceneManager->destroySceneNode(m_cameraNode);
		m_cameraNode = 0;
	}
	m_window = 0;
	m_sceneManager = 0;
}

bool Oculus::isOculusReady() const
{
	return (Lib::sensor != nullptr);
}

bool Oculus::isOgreReady() const
{
	return m_ogreReady;
}

bool Oculus::setupOculus()
{
	m_centreOffset = Lib::stereo_config->GetProjectionCenterOffset();
	return true;
}

bool Oculus::setupOgre(Ogre::SceneManager *sm, Ogre::RenderWindow *win, Ogre::SceneNode *parent)
{
	m_window = win;
	m_sceneManager = sm;
	Ogre::LogManager::getSingleton().logMessage("Oculus: Setting up Ogre");
	if(parent)
		m_cameraNode = parent->createChildSceneNode("StereoCameraNode");
	else
		m_cameraNode = sm->getRootSceneNode()->createChildSceneNode("StereoCameraNode");

	static_assert(kCameraLeftOculus == 0, "left camera is index=0");
	static_assert(kCameraRightOculus == 1, "right camera is index=1");
	static_assert(kCameraFull == 2, "full camera is index=2");

	m_cameras[0] = sm->createCamera("CameraLeft");
	m_cameras[1] = sm->createCamera("CameraRight");
	m_cameras[2] = sm->createCamera("CameraFull");
	
	Ogre::MaterialPtr matLeft = Ogre::MaterialManager::getSingleton().getByName("Ogre/Compositor/Oculus");
	Ogre::MaterialPtr matRight = matLeft->clone("Ogre/Compositor/Oculus/Right");
	Ogre::GpuProgramParametersSharedPtr pParamsLeft = matLeft->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	Ogre::GpuProgramParametersSharedPtr pParamsRight = matRight->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	Ogre::Vector4 hmdwarp;
	if(Lib::stereo_config) {
		hmdwarp = Ogre::Vector4(Lib::stereo_config->GetDistortionK(0),
								Lib::stereo_config->GetDistortionK(1),
								Lib::stereo_config->GetDistortionK(2),
								Lib::stereo_config->GetDistortionK(3));
	} else {
		hmdwarp = Ogre::Vector4(g_defaultDistortion[0],
								g_defaultDistortion[1],
								g_defaultDistortion[2],
								g_defaultDistortion[3]);
	}
	pParamsLeft->setNamedConstant("HmdWarpParam", hmdwarp);
	pParamsRight->setNamedConstant("HmdWarpParam", hmdwarp);
	pParamsLeft->setNamedConstant("LensCentre", 0.5f+(Lib::stereo_config->GetProjectionCenterOffset()/2.0f));
	pParamsRight->setNamedConstant("LensCentre", 0.5f-(Lib::stereo_config->GetProjectionCenterOffset()/2.0f));

	Ogre::CompositorPtr comp = Ogre::CompositorManager::getSingleton().getByName("OculusRight");
	comp->getTechnique(0)->getOutputTargetPass()->getPass(0)->setMaterialName("Ogre/Compositor/Oculus/Right");

	for(int i=0;i<2;++i)
	{
		m_cameraNode->attachObject(m_cameras[i]);
		if(Lib::stereo_config)
		{
			// Setup cameras.
			m_cameras[i]->setNearClipDistance(Lib::stereo_config->GetEyeToScreenDistance());
			m_cameras[i]->setFarClipDistance(g_defaultFarClip);
			m_cameras[i]->setPosition((i * 2 - 1) * Lib::stereo_config->GetIPD() * 0.5f, 0, 0);
			m_cameras[i]->setAspectRatio(Lib::stereo_config->GetAspect());
			m_cameras[i]->setFOVy(Ogre::Radian(Lib::stereo_config->GetYFOVRadians()));
			
			// Oculus requires offset projection, create a custom projection matrix
			Ogre::Matrix4 proj = Ogre::Matrix4::IDENTITY;
			float temp = Lib::stereo_config->GetProjectionCenterOffset();
			proj.setTrans(Ogre::Vector3(-Lib::stereo_config->GetProjectionCenterOffset() * (2 * i - 1), 0, 0));
			m_cameras[i]->setCustomProjectionMatrix(true, proj * m_cameras[i]->getProjectionMatrix());
		}
		else
		{
			m_cameras[i]->setNearClipDistance(g_defaultNearClip);
			m_cameras[i]->setFarClipDistance(g_defaultFarClip);
			m_cameras[i]->setPosition((i*2-1) * g_defaultIPD * 0.5f, 0, 0);
		}
		m_viewports[i] = win->addViewport(m_cameras[i], i, 0.5f*i, 0, 0.5f, 1.0f);
		m_viewports[i]->setBackgroundColour(g_defaultViewportColour);
		m_compositors[i] = Ogre::CompositorManager::getSingleton().addCompositor(m_viewports[i],i==0?"OculusLeft":"OculusRight");
		m_compositors[i]->setEnabled(true);
	}

	//fullscreen camera
	m_cameraNode->attachObject(m_cameras[kCameraFull]);
	m_cameras[kCameraFull]->setNearClipDistance(Lib::stereo_config->GetEyeToScreenDistance());
	m_cameras[kCameraFull]->setFarClipDistance(g_defaultFarClip);

	auto vp = win->addViewport(m_cameras[kCameraFull], kCameraFull, 0, 0, 1.0f, 1.0f);
	m_viewports[kCameraFull] = vp;
	vp->setBackgroundColour(g_defaultViewportColour);

	float ratio = (float)(vp->getActualWidth()) / (float)(vp->getActualHeight());
	m_cameras[kCameraFull]->setAspectRatio(ratio);

	//default mode : oculus mode
	//setCameraMode(kModeOculus);
	setCameraMode(kModeNormal);

	m_ogreReady = true;
	Ogre::LogManager::getSingleton().logMessage("Oculus: Oculus setup completed successfully");
	return true;
}

void Oculus::update()
{
	if(m_ogreReady)	{
		m_cameraNode->setOrientation(getOrientation());
	}
}

Ogre::SceneNode *Oculus::getCameraNode()
{
	return m_cameraNode;
}

Ogre::Quaternion Oculus::getOrientation() const
{
	if(isOculusReady()) {
		Quatf q = Lib::sensor_fusion->GetOrientation();
		return Ogre::Quaternion(q.w,q.x,q.y,q.z);
	} else {
		return Ogre::Quaternion::IDENTITY;
	}
}

Ogre::Camera *Oculus::getCamera(unsigned int i)
{
	return m_cameras[i];
}

Ogre::CompositorInstance *Oculus::getCompositor(unsigned int i)
{
	return m_compositors[i];
}

float Oculus::getCentreOffset() const
{
	return m_centreOffset;
}

void Oculus::resetOrientation()
{
	if(Lib::sensor_fusion) {
		Lib::sensor_fusion->Reset();
	}
}

void Oculus::setDistortion(bool val)
{
	for(auto compositor : m_compositors) {
		if(compositor != nullptr) {
			compositor->setEnabled(val);
		}
	}
}
bool Oculus::isDistortionEnabled() const
{
	return m_compositors[0]->getEnabled();
}

void Oculus::setCameraMode(int mode)
{
	switch(mode)
	{
	case kModeNormal:
		m_viewports[kCameraLeftOculus]->setCamera(nullptr);
		m_viewports[kCameraRightOculus]->setCamera(nullptr);
		m_viewports[kCameraFull]->setCamera(m_cameras[kCameraFull]);
		break;
	case kModeOculus:
		m_viewports[kCameraFull]->setCamera(nullptr);
		m_viewports[kCameraLeftOculus]->setCamera(m_cameras[kCameraLeftOculus]);
		m_viewports[kCameraRightOculus]->setCamera(m_cameras[kCameraRightOculus]);
		break;
	}
}

Ogre::Viewport *Oculus::getViewport(unsigned int i)
{
	return m_viewports[i];
}