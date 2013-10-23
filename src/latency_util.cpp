// Ŭnicode please 
#include "stdafx.h"
#include "latency_util.h"

LatencyUtil::~LatencyUtil()
{
	pLatencyTester.Clear();
}

void LatencyUtil::addDevice(OVR::LatencyTestDevice *tester)
{
	if(tester == nullptr) {
		return;
	}

	pLatencyTester = tester;
	if (!pLatencyTester) {
		util_.SetDevice(tester);
	}
}

void LatencyUtil::removeDevice()
{
	util_.SetDevice(nullptr);
	pLatencyTester.Clear();
}

void LatencyUtil::update()
{
	// Process latency tester results.
	const char* results = util_.GetResultsString();
	if (results != NULL)
	{
		OVR::LogText("LATENCY TESTER: %s\n", results); 
	}

	// >>> THIS MUST BE PLACED AS CLOSE AS POSSIBLE TO WHERE THE HMD ORIENTATION IS READ <<<
	util_.ProcessInputs();
}