// Ŭnicode please 
#pragma once

class BaseApplication;

class DevicePlugAndPlay : public OVR::MessageHandler {
public:
	DevicePlugAndPlay() {}
	virtual ~DevicePlugAndPlay() {}
	virtual void OnMessage(const OVR::Message& msg);
	virtual void update();
	virtual bool isNull() { return false; }

private:
	struct DeviceStatusNotificationDesc
    {
        OVR::DeviceHandle    Handle;
        OVR::MessageType     Action;

        DeviceStatusNotificationDesc():Action(OVR::Message_None) {}
        DeviceStatusNotificationDesc(OVR::MessageType mt, const OVR::DeviceHandle& dev) 
            : Handle(dev), Action(mt) {}
    };
    OVR::Array<DeviceStatusNotificationDesc> DeviceStatusNotificationsQueue; 
};

class NullDevicePlugAndPlay : public DevicePlugAndPlay {
public:
	NullDevicePlugAndPlay() {}
	virtual ~NullDevicePlugAndPlay() {}
	virtual void OnMessage(const OVR::Message& msg) {}
	virtual void update() {}
	virtual bool isNull() { return true; }
};