// Ŭnicode please 
#include "stdafx.h"
#include "entrypoint.h"
#include "lib.h"

#include "OgreOculus.h"
#include "euler.h"

#include "sequence.h"

#include "device_plug_and_play.h"
#include "latency_util.h"

//#include "ogre_console_app.h"
#include "gorilla_3d_sequence.h"

using namespace Ogre;

int entrypoint_run_ogre();
int entrypoint_libovr();

int entrypoint()
{	
	//return entrypoint_libovr();
	return entrypoint_run_ogre();
	//return 0;
}

int entrypoint_run_ogre()
{
	bool init_result = Lib::startUp(USE_ALL);
	//bool init_result = Lib::startUp(USE_OGRE | USE_OIS | USE_GORILLA);
	if(init_result == false) {
		Lib::shutDown();
		return 0;
	}
	{
	SequenceFactory seq_factory;
	//std::unique_ptr<Sequence> sequence = seq_factory.create(kSequenceDemo);
	//std::unique_ptr<Sequence> sequence = seq_factory.create(kSequenceOculusDemo);
	//std::unique_ptr<Gorilla3DSequence> sequence((Gorilla3DSequence*)seq_factory.createRaw(kGorillaDemo));
	Gorilla3DSequence sequence;
	//sequence->setUp();
	sequence.setUp();

	//sequence->registerSequence();
	sequence.registerSequence();

	//Lib::oculus->setCameraMode(Oculus::kModeNormal);
	
	// cleaning of windows events managed by Ogre::WindowEventUtilities::...
	// I call it after a 'pause in window updating', in order to maintain smoothness.
	// Explanation : if you clicked 2000 times when the windows was being created, there are 
	// at least 2000 messages created by the OS to listen to. This is made to clean them.
	Lib::root->clearEventTimes();
	
	while(!Lib::window->isClosed()) {
		// Check if any new devices were connected.
		Lib::device_pnp->update();
		Lib::latency_util->update();

		// 입력 처리는 렌더링관련 update보다 먼저
		Lib::captureInput();

		// Check for windows messages
		Ogre::WindowEventUtilities::messagePump();

		// Render the scene
		bool next = Lib::root->renderOneFrame();
		if(next == false) {
			break;
		}

		// Drawings
		// the window update its content.
		// each viewport that is 'autoupdated' will be redrawn now,
		// in order given by its z-order.
		//Lib::window->update(true);
		
		// Perform a 1ms sleep. This stops the app from hogging the cpu, and also stops some gfx cards (like my previous one) from overheating
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		Sleep(1);
#else
		sleep(1);
#endif
	}
	
	sequence.unregisterSequence();
	//sequence->unregisterSequence();
	//sequence.reset(nullptr);
	}
	Lib::shutDown();

    return 0;
}

int entrypoint_libovr()
{
	using namespace OVR;

    //OVR::System::Init();
	OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));

    Ptr<DeviceManager> pManager = 0;
    Ptr<HMDDevice>     pHMD = 0;
    Ptr<SensorDevice>  pSensor = 0;
    SensorFusion       FusionResult;

     
    // *** Initialization - Create the first available HMD Device
    pManager = *DeviceManager::Create();
    pHMD     = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();
    if (!pHMD)
        return -1;
    pSensor  = *pHMD->GetSensor();

    // Get DisplayDeviceName, ScreenWidth/Height, etc..
    HMDInfo hmdInfo;
    pHMD->GetDeviceInfo(&hmdInfo);
    
    if (pSensor)
        FusionResult.AttachToSensor(pSensor);

    // *** Per Frame
    // Get orientation quaternion to control view
    Quatf q = FusionResult.GetOrientation();

    // Create a matrix from quaternion,
    // where elements [0][0] through [3][3] contain rotation.
    Matrix4f bodyFrameMatrix(q); 

    // Get Euler angles from quaternion, in specified axis rotation order.
    float yaw, pitch, roll;
    q.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);

    // *** Shutdown
	MessageHandler* pCurrentHandler = pSensor->GetMessageHandler();
	pCurrentHandler->RemoveHandlerFromDevices();
	FusionResult.AttachToSensor(nullptr);

    pSensor.Clear();
    pHMD.Clear();
	pManager.Clear();
	
    OVR::System::Destroy();

	return 0;
}