// Ŭnicode please 
#include "stdafx.h"
#include "device_plug_and_play.h"

#include "latency_util.h"
#include "lib.h"

using namespace OVR;

void DevicePlugAndPlay::OnMessage(const OVR::Message& msg)
{
	if (msg.Type == Message_DeviceAdded || msg.Type == Message_DeviceRemoved)
	{
		if (msg.pDevice == Lib::device_mgr)
		{
			const MessageDeviceStatus& statusMsg =
				static_cast<const MessageDeviceStatus&>(msg);

			{ // limit the scope of the lock
				Lock::Locker lock(Lib::device_mgr->GetHandlerLock());
				DeviceStatusNotificationsQueue.PushBack(
					DeviceStatusNotificationDesc(statusMsg.Type, statusMsg.Handle));
			}

			switch (statusMsg.Type)
			{
			case OVR::Message_DeviceAdded:
				LogText("DeviceManager reported device added.\n");
				break;

			case OVR::Message_DeviceRemoved:
				LogText("DeviceManager reported device removed.\n");
				break;

			default: OVR_ASSERT(0); // unexpected type
			}
		}
	}
}

// Check if any new devices were connected.
void DevicePlugAndPlay::update()
{
	bool queueIsEmpty = false;
	while (!queueIsEmpty)
	{
		DeviceStatusNotificationDesc desc;

		{
			Lock::Locker lock(Lib::device_mgr->GetHandlerLock());
			if (DeviceStatusNotificationsQueue.GetSize() == 0)
				break;
			desc = DeviceStatusNotificationsQueue.Front();

			// We can't call Clear under the lock since this may introduce a dead lock:
			// this thread is locked by HandlerLock and the Clear might cause 
			// call of Device->Release, which will use Manager->DeviceLock. The bkg
			// thread is most likely locked by opposite way: 
			// Manager->DeviceLock ==> HandlerLock, therefore - a dead lock.
			// So, just grab the first element, save a copy of it and remove
			// the element (Device->Release won't be called since we made a copy).

			DeviceStatusNotificationsQueue.RemoveAt(0);
			queueIsEmpty = (DeviceStatusNotificationsQueue.GetSize() == 0);
		}

		bool wasAlreadyCreated = desc.Handle.IsCreated();

		if (desc.Action == Message_DeviceAdded)
		{
			switch(desc.Handle.GetType())
			{
			case Device_Sensor:
				if (desc.Handle.IsAvailable() && !desc.Handle.IsCreated())
				{
					if (!Lib::sensor)
					{
						Lib::sensor = desc.Handle.CreateDeviceTyped<SensorDevice>();
						Lib::sensor_fusion->AttachToSensor(Lib::sensor);

						 LogText("SENSOR connected");
					}
					else if (!wasAlreadyCreated)
					{
						LogText("A new SENSOR has been detected, but it is not currently used.");
					}
				}
				break;
			case Device_LatencyTester:
				if (desc.Handle.IsAvailable() && !desc.Handle.IsCreated())
				{
					if(!Lib::latency_util->tester()) {
						Lib::latency_util->addDevice(desc.Handle.CreateDeviceTyped<LatencyTestDevice>());
						if (!wasAlreadyCreated)
							LogText("LATENCY TESTER connected");
					}
				}
				break;
			case Device_HMD:
				{
					OVR::HMDInfo info;
					desc.Handle.GetDeviceInfo(&info);
					// if strlen(info.DisplayDeviceName) == 0 then
					// this HMD is 'fake' (created using sensor).
					if (strlen(info.DisplayDeviceName) > 0 && (!Lib::hmd || !info.IsSameDisplay(*Lib::hmd_info)))
					{
						LogText("HMD connected");
						if (!Lib::hmd || !desc.Handle.IsDevice(Lib::hmd))
							Lib::hmd = desc.Handle.CreateDeviceTyped<HMDDevice>();
						// update stereo config with new HMDInfo
						if (Lib::hmd && Lib::hmd->GetDeviceInfo(Lib::hmd_info))
						{
							//RenderParams.MonitorName = hmd.DisplayDeviceName;
							Lib::stereo_config->SetHMDInfo(*Lib::hmd_info);
						}
						LogText("HMD device added.\n");
					}
					break;
				}
			default:;
			}
		}
		else if (desc.Action == Message_DeviceRemoved)
		{
			if (desc.Handle.IsDevice(Lib::sensor))
			{
				LogText("Sensor reported device removed.\n");
				Lib::sensor_fusion->AttachToSensor(NULL);

				Lib::sensor->Release();
				Lib::sensor = nullptr;
			}
			else if (desc.Handle.IsDevice(Lib::latency_util->tester()))
			{
				LogText("Latency Tester reported device removed.\n");
				Lib::latency_util->removeDevice();
			}
			else if (desc.Handle.IsDevice(Lib::hmd))
			{
				if (Lib::hmd && !Lib::hmd->IsDisconnected())
				{
					LogText("HMD disconnected");
					// Disconnect HMD. pSensor is used to restore 'fake' HMD device
					// (can be NULL).
					Lib::hmd = Lib::hmd->Disconnect(Lib::sensor);

					// This will initialize TheHMDInfo with information about configured IPD,
					// screen size and other variables needed for correct projection.
					// We pass HMD DisplayDeviceName into the renderer to select the
					// correct monitor in full-screen mode.
					if (Lib::hmd && Lib::hmd->GetDeviceInfo(Lib::hmd_info))
					{
						//RenderParams.MonitorName = hmd.DisplayDeviceName;
						Lib::stereo_config->SetHMDInfo(*Lib::hmd_info);
					}
					LogText("HMD device removed.\n");
				}
			}
		}
		else 
			OVR_ASSERT(0); // unexpected action
	}
}