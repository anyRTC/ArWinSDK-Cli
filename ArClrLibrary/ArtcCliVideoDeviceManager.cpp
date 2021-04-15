#include "stdafx.h"
#include "ArtcCliVideoDeviceManager.h"
#include "ArtcCliLibrary.h"
using namespace ArtcCliLibrary;

CliVideoDeviceCollection::CliVideoDeviceCollection(IVideoDeviceCollection * rawCollection):
	raw(rawCollection)
{
	
}

CliVideoDeviceCollection::~CliVideoDeviceCollection()
{
	this->release();
	this->!CliVideoDeviceCollection();
}

CliVideoDeviceCollection::!CliVideoDeviceCollection()
{
	
}

int CliVideoDeviceCollection::getCount()
{
	return raw->getCount();
}

int CliVideoDeviceCollection::getDevice(int index, String ^% deviceName, String ^% deviceId)
{
	char deviceNameBuffer[MAX_DEVICE_ID_LENGTH] = { 0 }; 
	char deviceIdBuffer[MAX_DEVICE_ID_LENGTH] = { 0 };
	int result = raw->getDevice(index, deviceNameBuffer, deviceIdBuffer);
	if (result != 0) return result;
	deviceName = gcnew String(deviceNameBuffer), deviceId = gcnew String(deviceIdBuffer);
	return result;
	// return raw->getDevice(index, const_cast<char*>(MarshalString(deviceName).c_str()), const_cast<char*>(MarshalString(deviceId).c_str()));
}

int CliVideoDeviceCollection::setDevice(String ^ deviceId)
{
	return raw->setDevice(const_cast<char*>(MarshalString(deviceId).c_str()));
}

void CliVideoDeviceCollection::release()
{
	raw->release();
}

ArtcCliVideoDeviceManager::ArtcCliVideoDeviceManager(IRtcEngine* engine)
{
	this->engine = engine;
}

CliVideoDeviceCollection ^ ArtcCliVideoDeviceManager::enumerateVideoDevices()
{
	AVideoDeviceManager manager(engine);
	return gcnew CliVideoDeviceCollection(manager->enumerateVideoDevices());
}

int ArtcCliVideoDeviceManager::setDevice(String ^ deviceId)
{
	AVideoDeviceManager manager(engine);
	return manager->setDevice(MarshalString(deviceId).c_str());
}

int ArtcCliVideoDeviceManager::getDevice(String ^ deviceId)
{
	AVideoDeviceManager manager(engine);
	return manager->getDevice(const_cast<char*>(MarshalString(deviceId).c_str()));
}

int ArtcCliVideoDeviceManager::startDeviceTest(IntPtr hwnd)
{
	AVideoDeviceManager manager(engine);
	return manager->startDeviceTest(hwnd.ToPointer());
}

int ArtcCliVideoDeviceManager::stopDeviceTest()
{
	AVideoDeviceManager manager(engine);
	return manager->stopDeviceTest();
}
