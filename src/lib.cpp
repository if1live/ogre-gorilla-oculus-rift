// Ŭnicode please 
#include "stdafx.h"
#include "lib.h"
#include "template_lib.h"

#include "OgreOculus.h"
#include "device_plug_and_play.h"
#include "latency_util.h"
#include "console_function.h"
#include "OgreConsoleForGorilla.h"

using namespace OVR;

class CoreMessageHandler : public OVR::MessageHandler {
public:
	virtual void OnMessage(const OVR::Message &msg)
	{
		Lib::device_pnp->OnMessage(msg);
	}
};


namespace Lib {;

unsigned int init_flag = 0;

OVR::DeviceManager *device_mgr = nullptr;
OVR::HMDDevice *hmd = nullptr;
OVR::Util::Render::StereoConfig *stereo_config = nullptr;
OVR::SensorDevice *sensor = nullptr;
OVR::SensorFusion *sensor_fusion = nullptr;

static OVR::HMDInfo hmd_info_local;
OVR::HMDInfo *hmd_info = &hmd_info_local;

OVR::Profile *user_profile = nullptr;
std::unique_ptr<LatencyUtil> latency_util;
std::unique_ptr<DevicePlugAndPlay> device_pnp;

// ogre
Ogre::Root *root = nullptr;
Ogre::RenderWindow *window = nullptr;
Ogre::SceneManager *scene_mgr = nullptr;

// gorilla
Gorilla::Silverback *gorilla = nullptr;
Gorilla::Screen *screen_2d = nullptr;
Ogre::Viewport *viewport_2d = nullptr;
OgreConsole *console = nullptr;

//ois
OIS::InputManager *input_mgr = nullptr;
OIS::Keyboard *keyboard = nullptr;
OIS::Mouse *mouse = nullptr;
std::vector<OIS::JoyStick*> joystick_list;

static Oculus oculus_local;
Oculus *oculus = &oculus_local;

static CoreMessageHandler *msg_handler = nullptr;

OIS::JoyStick *main_joystick()
{
	if(joystick_list.size() == 0) {
		return nullptr;
	}
	return joystick_list[0];
}
OIS::JoyStick *getJoystick(int idx)
{
	if(idx < joystick_list.size()) {
		return joystick_list[idx];
	}
	return nullptr;
}

bool startUp(unsigned int flag)
{
	init_flag = flag;

	// fill null object
	latency_util.reset(new NullLatencyUtil());
	device_pnp.reset(new NullDevicePlugAndPlay());

	//OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));
	OVR::System::Init();

	if((flag & USE_OGRE) == USE_OGRE) {
		try {
			//bool retval = startUpOgre(false);
			bool retval = startUpOgre(true);
			//auto cam = createOgreCamera();
			//createOgreViewport(cam);

			if(retval == false) {
				return false;
			}
		} catch(Ogre::Exception &ex) {
			std::cout << "Ogre::Exception" << ex.what() << std::endl;
			return false;
		} catch(std::exception &ex) {
			std::cout << "std::exception" << ex.what() << std::endl;
			return false;
		}
	}

	if((flag & USE_OIS) == USE_OIS) {
		//ois
		startUpOIS();
	}

	if((flag & USE_OCULUS) == USE_OCULUS) {
		// libovr
		startUpLibOVR();

		// ogre oculus
		oculus->setupOculus();
		oculus->setupOgre(scene_mgr, window);
	}

	if((flag & USE_GORILLA) == USE_GORILLA) {
		// Initialise Gorilla
		gorilla = new Gorilla::Silverback();
		gorilla->loadAtlas("dejavu");
	}

	if((flag & USE_CONSOLE) == USE_CONSOLE) {
		OVR_ASSERT((flag & USE_GORILLA) == USE_GORILLA && "console need gorilla");

		//HUD must use indepentend SceneManager!
		auto hud_scene_mgr = root->createSceneManager(Ogre::ST_GENERIC);
		auto cam = hud_scene_mgr->createCamera("HMDCamera");
		const int console_z_order = 10;
		viewport_2d = window->addViewport(cam, console_z_order);
		viewport_2d->setBackgroundColour(Ogre::ColourValue(0,0,0));
		viewport_2d->setOverlaysEnabled(true);
		viewport_2d->setClearEveryFrame(false);	// you 'll need it in order not to overwrite everything.
		viewport_2d->setSkiesEnabled(false);		// you don't want skies /scene to pollute your rendering.
		screen_2d = gorilla->createScreen(viewport_2d, "dejavu");

		console = new OgreConsole();
		console->init(screen_2d);
		console->setVisible(false);

		console->addCommand("whoami", console::whoami);
		console->addCommand("version", console::version);
		console->addCommand("sudo", console::sudo);
		console->addCommand("make", console::make);
	}

	//cegui
	//CEGUI::OgreRenderer& renderer = CEGUI::OgreRenderer::bootstrapSystem();

	return true;
}
void shutDown()
{
	if((init_flag & USE_CONSOLE) == USE_CONSOLE) {
		kuuko::safeDelete(console);
		viewport_2d = nullptr;
	}

	if((init_flag & USE_GORILLA) == USE_GORILLA) {
		kuuko::safeDelete(gorilla);
	}

	if((init_flag & USE_OCULUS) == USE_OCULUS) {
		// ogre oculus
		oculus->shutDownOgre();
		oculus->shutDownOculus();
		// libovr
		shutDownLibOVR();
	}

	if((init_flag & USE_OIS) == USE_OIS) {
		// ois
		shutDownOIS();
	}

	if((init_flag & USE_OGRE) == USE_OGRE) {
		// ogre
		shutDownOgre();
	}

	//manual free object
	latency_util.reset(nullptr);
	device_pnp.reset(nullptr);

	OVR::System::Destroy();

	init_flag = 0;
}

bool startUpLibOVR()
{
	OVR_ASSERT(!device_mgr);
	OVR_ASSERT(!hmd);
	OVR_ASSERT(!stereo_config);
	OVR_ASSERT(!sensor);

	Ogre::LogManager::getSingleton().logMessage("Oculus: Initialising system");
	//System::Init(Log::ConfigureDefaultLog(LogMask_All));
	device_mgr = DeviceManager::Create();
	if(!device_mgr)	{
		Ogre::LogManager::getSingleton().logMessage("Oculus: Failed to create Device Manager");
		return false;
	}
	Ogre::LogManager::getSingleton().logMessage("Oculus: Created Device Manager");

	// if REAL HMD object is not exist, startUpLibOVR maybe occur failure
	// So, initialize LibOVR base object first.
	OVR_ASSERT(!msg_handler);
	msg_handler = new CoreMessageHandler();
	device_mgr->SetMessageHandler(msg_handler);

	device_pnp.reset(new DevicePlugAndPlay());
	latency_util.reset(new LatencyUtil());

	OVR_ASSERT(!sensor_fusion);
	sensor_fusion = new SensorFusion();

	stereo_config = new Util::Render::StereoConfig();
	if(!stereo_config) {
		Ogre::LogManager::getSingleton().logMessage("Oculus: Failed to create StereoConfig");
		return false;
	}
	Ogre::LogManager::getSingleton().logMessage("Oculus: Created StereoConfig");


	hmd = device_mgr->EnumerateDevices<HMDDevice>().CreateDevice();
	if(!hmd) {
		Ogre::LogManager::getSingleton().logMessage("Oculus: Failed to create HMD");
		return false;
	}
	Ogre::LogManager::getSingleton().logMessage("Oculus: Created HMD");


	hmd->GetDeviceInfo(hmd_info);
	stereo_config->SetHMDInfo(*hmd_info);
	sensor = hmd->GetSensor();
	if(!sensor) {
		Ogre::LogManager::getSingleton().logMessage("Oculus: Failed to create sensor");
		return false;
	}
	sensor_fusion->AttachToSensor(sensor);
	Ogre::LogManager::getSingleton().logMessage("Oculus: Created sensor");	

	// Retrieve relevant profile settings. 
	user_profile = hmd->GetProfile();
	if (user_profile) {
		//TODO
		//ThePlayer.UserEyeHeight = pUserProfile->GetEyeHeight();
		//ThePlayer.EyePos.y = ThePlayer.UserEyeHeight;
	}

	Ogre::LogManager::getSingleton().logMessage("Oculus: Oculus setup completed successfully");

	return true;
}

bool shutDownLibOVR()
{
	if(msg_handler) {
		msg_handler->RemoveHandlerFromDevices();
		kuuko::safeDelete(msg_handler);
	}

	device_pnp.reset(nullptr);
	latency_util.reset(nullptr);
	kuuko::safeDelete(stereo_config);
	kuuko::safeDelete(sensor_fusion);

	if(sensor) {
		sensor->Release();
		sensor = nullptr;
	}
	if(hmd) {
		hmd->Release();
		hmd = nullptr;
	}
	if(device_mgr) {
		device_mgr->Release();
		device_mgr = nullptr;
	}
	return true;
}

// http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Using+OIS
bool startUpOIS()
{
	OVR_ASSERT(!input_mgr);
	OVR_ASSERT(!keyboard);
	OVR_ASSERT(!mouse);
	OVR_ASSERT(joystick_list.size() == 0);

	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	window->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	input_mgr = OIS::InputManager::createInputSystem( pl );

	bool buffered_mode = true;

	if(input_mgr->getNumberOfDevices(OIS::OISKeyboard) > 0) {
		keyboard = static_cast<OIS::Keyboard*>(input_mgr->createInputObject( OIS::OISKeyboard, buffered_mode ));
	}

	if(input_mgr->getNumberOfDevices(OIS::OISMouse) > 0) {
		mouse = static_cast<OIS::Mouse*>(input_mgr->createInputObject( OIS::OISMouse, buffered_mode ));
		const OIS::MouseState &info_source = mouse->getMouseState();
		info_source.width = window->getWidth();
		info_source.height = window->getHeight();
	}

	if(input_mgr->getNumberOfDevices(OIS::OISJoyStick) > 0) {
		joystick_list.resize(input_mgr->getNumberOfDevices(OIS::OISJoyStick));

		for(size_t i = 0 ; i < joystick_list.size() ; ++i) {
			joystick_list[i] = static_cast<OIS::JoyStick*>(input_mgr->createInputObject(OIS::OISJoyStick, buffered_mode));
		}
	}
	
	return true;
}

void captureInput()
{
	keyboard->capture();
	mouse->capture();
	for(auto joystick : joystick_list) {
		joystick->capture();
	}
}

bool shutDownOIS()
{
	if(input_mgr) {
		if(mouse) {
			input_mgr->destroyInputObject(mouse);
			mouse = nullptr;
		}
		if(keyboard) {
			input_mgr->destroyInputObject(keyboard);
			keyboard = nullptr;
		}

		if(joystick_list.size() > 0) {
			for(auto joystick : joystick_list) {
				input_mgr->destroyInputObject(joystick);
			}
			joystick_list.clear();
		}
		
		OIS::InputManager::destroyInputSystem(input_mgr);
		input_mgr = nullptr;
	}

	return true;

}

bool startUpOgre(bool use_gui_configure)
{
	OVR_ASSERT(root == nullptr);
	OVR_ASSERT(window == nullptr);
	OVR_ASSERT(scene_mgr == nullptr);

	Ogre::String plugins_file_name = "";
	Ogre::String config_file_name = "config.cfg";
	Ogre::String log_file_name = "";

	bool is_debug_mode = OGRE_DEBUG_MODE;

	if(is_debug_mode) {
		log_file_name = "log_d.txt";
	} else {
		log_file_name = "log.txt";
	}

	if(use_gui_configure) {
		if(is_debug_mode) {
			plugins_file_name = "plugins_d.cfg";
		} else {
			plugins_file_name = "plugins.cfg";
		}
	}

	root = new Ogre::Root(plugins_file_name, config_file_name, log_file_name);

	bool configure_result = false;
	if(use_gui_configure) {
		configure_result = configureOgreByGUI();
	} else {
		configure_result = configureOgreByManual();
	}
	if(configure_result == false) {
		return false;
	}

	loadOgreResources();

	// Get the SceneManager, in this case a generic one
	scene_mgr = root->createSceneManager(Ogre::ST_GENERIC);
	scene_mgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	return true;
}
void shutDownOgre()
{
	Ogre::LogManager::getSingleton().logMessage("shutdown ogre");

	kuuko::safeDelete(root);
	window = nullptr;
	scene_mgr = nullptr;
}

Ogre::Camera *createOgreCamera()
{
	Ogre::SceneNode *root_scene_node = scene_mgr->getRootSceneNode();
	Ogre::Camera *cam = scene_mgr->createCamera("PlayerCam");
	Ogre::SceneNode *cam_node = root_scene_node->createChildSceneNode("PlayerCamNode");
	cam_node->attachObject(cam);

	cam->setPosition(Ogre::Vector3(20,50,80));
	cam->lookAt(Ogre::Vector3(0,0,0));

	return cam;
}

void createOgreViewport(Ogre::Camera *cam)
{
	float viewport_width = 1.0f;
	float viewport_height = 1.0f;
	float viewport_left = (1.0f - viewport_width) * 0.5f;
	float viewport_top = (1.0f - viewport_height) * 0.5f;
	int main_viewport_z_order = 100;
	Ogre::Viewport *vp = window->addViewport(cam, main_viewport_z_order, viewport_left,
		viewport_top, viewport_width, viewport_height);
	vp->setAutoUpdated(true);
	vp->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 1.0f));

	float ratio = (float)(vp->getActualWidth()) / (float)(vp->getActualHeight());
	cam->setAspectRatio(ratio);

	cam->setNearClipDistance(1.5f);
	cam->setFarClipDistance(3000.0f);
}

bool configureOgreByManual()
{
	const bool is_debug_mode = OGRE_DEBUG_MODE;

	// Load Plugins
	std::vector<Ogre::String> plugin_name_list;
	plugin_name_list.push_back("RenderSystem_GL");
	//plugin_name_list.push_back("RenderSystem_Direct3D9");
	//plugin_name_list.push_back("Plugin_ParticleFX");
	plugin_name_list.push_back("Plugin_CgProgramManager");
	//plugin_name_list.push_back("Plugin_OctreeSceneManager");
	for(auto &plugin_name : plugin_name_list) {
		if(is_debug_mode) {
			plugin_name.append("_d");
		}
		root->loadPlugin(plugin_name);
	}

	// Initialize render system
	const Ogre::RenderSystemList &render_system_list = root->getAvailableRenderers();
	if(render_system_list.size() == 0) {
		Ogre::LogManager::getSingleton().logMessage("Sorry, no rendersystem was found");
		return false;
	}
	Ogre::RenderSystem *render_system = render_system_list[0];
	root->setRenderSystem(render_system);


	// When the RenderSystem is selected, we can initialise the Root. The root can be initialised only when a rendersystem has been selected.
	bool create_window_automatically = false;
	root->initialise(create_window_automatically, "", "");

	// we can ask to the RenderSystem to create a window.
	Ogre::String window_title = "Hello World";
	int size_x = 1280;
	int size_y = 800;
	bool fullscreen = false;
	//bool fullscreen = true;
	Ogre::NameValuePairList params;
	params["FSAA"] = "0";
	//params["vsync"] = "true";
	window = root->createRenderWindow(window_title, size_x, size_y, fullscreen, &params);
	
	// I want my window to be active
	window->setActive(true);

	// I want to update myself the content of the window, not automatically.
	//window->setAutoUpdated(false);

	return true;
}

bool configureOgreByGUI()
{
	if(root->showConfigDialog()) {
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		window = root->initialise(true, "TutorialApplication Render Window");
		return true;
	} else {
		return false;
	}
}

void loadOgreResources()
{
	bool is_debug_mode = OGRE_DEBUG_MODE;
	Ogre::String resource_file_name = "";
	if(is_debug_mode) {
		resource_file_name = "resources_d.cfg";
	} else {
		resource_file_name = "resources.cfg";
	}

	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(resource_file_name);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

}	// namespace Lib