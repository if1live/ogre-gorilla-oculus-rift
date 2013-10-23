// Ŭnicode please 
#pragma once

#include <vector>
#include <memory>

class OgreInitializer;
class Oculus;
class LatencyUtil;
class DevicePlugAndPlay;
class OgreConsole;

namespace Ogre {
	class Root;
	class RenderWindow;
	class SceneManager;
	class Camera;
	class Viewport;
}	// namespace Ogre
namespace OIS {
	class InputManager;
	class Keyboard;
	class Mouse;
	class JoyStick;
}	// namespace OIS
namespace OVR {
	class DeviceManager;
	class HMDDevice;
	class SensorDevice;
	class SensorFusion;
	namespace Platform { 
		class Application; 
	}	// namespace Platform
	namespace Util {
		namespace Render {
			class StereoConfig;
		}	// namespace Render
	}	// namespace Util
}	// namespace OVR
namespace Gorilla {
	class Silverback;
	class Screen;
}

enum {
	USE_OGRE = 1 << 0,
	USE_OIS = 1 << 1,
	USE_OCULUS = 1 << 2,
	USE_GORILLA = 1 << 3,
	USE_CONSOLE = 1 << 4,
	USE_ALL = USE_OGRE | USE_OIS | USE_OCULUS | USE_GORILLA | USE_CONSOLE,
};


namespace Lib {
	bool startUp(unsigned int flag = USE_ALL);
	void shutDown();

	extern unsigned int init_flag;

	// oculus sdk
	extern OVR::DeviceManager *device_mgr;
	extern OVR::HMDDevice *hmd;
	extern OVR::Util::Render::StereoConfig *stereo_config;
	extern OVR::SensorDevice *sensor;
	extern OVR::SensorFusion *sensor_fusion;
	extern OVR::HMDInfo *hmd_info;
	extern OVR::Profile *user_profile;
	extern std::unique_ptr<LatencyUtil> latency_util;
	extern std::unique_ptr<DevicePlugAndPlay> device_pnp;

	// ogre
	extern Ogre::Root *root;
	extern Ogre::RenderWindow *window;
	extern Ogre::SceneManager *scene_mgr;

	// gorilla
	extern Gorilla::Silverback *gorilla;
	extern Gorilla::Screen *screen_2d;
	extern Ogre::Viewport *viewport_2d;
	extern OgreConsole *console;

	//ois
	extern OIS::InputManager *input_mgr;
	extern OIS::Keyboard *keyboard;
	extern OIS::Mouse *mouse;
	extern std::vector<OIS::JoyStick*> joystick_list;
	OIS::JoyStick *main_joystick();
	OIS::JoyStick *getJoystick(int idx);

	extern Oculus *oculus;

	bool startUpLibOVR();
	bool shutDownLibOVR();

	bool startUpOIS();
	bool shutDownOIS();

	void captureInput();

	// ogre
	bool startUpOgre(bool use_gui_configure);
	void shutDownOgre();

	Ogre::Camera *createOgreCamera();
	void createOgreViewport(Ogre::Camera *cam);

	bool configureOgreByManual();
	bool configureOgreByGUI();

	void loadOgreResources();
}	// namespace Lib