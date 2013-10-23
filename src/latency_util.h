// Ŭnicode please 
#pragma once

class LatencyUtil {
public:
	LatencyUtil() {}
	virtual ~LatencyUtil();

	virtual bool isNull() { return false; }

	virtual void addDevice(OVR::LatencyTestDevice *tester);
	virtual void removeDevice();

	virtual void update();

	OVR::LatencyTestDevice *tester() { return pLatencyTester; }
	OVR::Util::LatencyTest &util() { return util_; }

private:
	OVR::Ptr<OVR::LatencyTestDevice>  pLatencyTester;
	OVR::Util::LatencyTest util_;
};


class NullLatencyUtil : public LatencyUtil {
public:
	NullLatencyUtil() {}
	virtual ~NullLatencyUtil() {}
	virtual bool isNull() { return true; }
	virtual void addDevice(OVR::LatencyTestDevice *tester) {}
	virtual void removeDevice() {}
	virtual void update() {}
};