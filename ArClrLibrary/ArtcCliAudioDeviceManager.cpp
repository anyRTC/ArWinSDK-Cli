#include "stdafx.h"
#include "ArtcCliAudioDeviceManager.h"
#include "ArtcCliLibrary.h"

using namespace ArtcCliLibrary;
using namespace ar::rtc;

CliAudioDeviceCollection::CliAudioDeviceCollection(IAudioDeviceCollection* rawCollection):
	raw(rawCollection)
{

}

CliAudioDeviceCollection::~CliAudioDeviceCollection()
{
	this->release();
	this->!CliAudioDeviceCollection();
}

CliAudioDeviceCollection::!CliAudioDeviceCollection()
{
}

int CliAudioDeviceCollection::getCount()
{
	return raw->getCount();
}

int CliAudioDeviceCollection::getDevice(int index, String ^% deviceName, String ^% deviceId)
{
	char deviceNameBuffer[MAX_DEVICE_ID_LENGTH] = { 0 }; 
	char deviceIdBuffer[MAX_DEVICE_ID_LENGTH] = { 0 };
	int result = raw->getDevice(index, deviceNameBuffer, deviceIdBuffer);
	if (result != 0) return result;
	deviceName = gcnew String(deviceNameBuffer), deviceId = gcnew String(deviceIdBuffer);
	return result;
	//	return raw->getDevice(index, const_cast<char*>(MarshalString(deviceName).c_str()), const_cast<char*>(MarshalString(deviceId).c_str()));
}

int CliAudioDeviceCollection::setDevice(String ^ deviceId)
{
	return raw->setDevice(MarshalString(deviceId).c_str());
}

int CliAudioDeviceCollection::setApplicationVolume(int volume)
{
	return raw->setApplicationVolume(volume);
}

int CliAudioDeviceCollection::getApplicationVolume(int% volume)
{
	int vol;
	int result = raw->getApplicationVolume(vol);
	volume = vol;
	return result;
}

int CliAudioDeviceCollection::setApplicationMute(bool mute)
{
	return raw->setApplicationMute(mute);
}

int CliAudioDeviceCollection::isApplicationMute(bool% mute)
{
	bool mu;
	int result = raw->isApplicationMute(mu);
	mute = mu;
	return result;
}

void CliAudioDeviceCollection::release()
{
	raw->release();
}

ArtcCliAudioDeviceManager::ArtcCliAudioDeviceManager(IRtcEngine* engine)
{
	this->engine = engine;
}

CliAudioDeviceCollection ^ ArtcCliAudioDeviceManager::enumeratePlaybackDevices()
{
	AAudioDeviceManager manager(engine);
	return gcnew CliAudioDeviceCollection(manager->enumeratePlaybackDevices());
}

CliAudioDeviceCollection ^ ArtcCliAudioDeviceManager::enumerateRecordingDevices()
{
	AAudioDeviceManager manager(engine);
	return gcnew CliAudioDeviceCollection(manager->enumerateRecordingDevices());
}

int ArtcCliAudioDeviceManager::setPlaybackDevice(String ^ deviceId)
{
	AAudioDeviceManager manager(engine);
	return manager->setPlaybackDevice(MarshalString(deviceId).c_str());
}

int ArtcCliAudioDeviceManager::setRecordingDevice(String ^ deviceId)
{
	AAudioDeviceManager manager(engine);
	return manager->setRecordingDevice(MarshalString(deviceId).c_str());
}

int ArtcCliAudioDeviceManager::setPlaybackDeviceVolume(int volume)
{
	AAudioDeviceManager manager(engine);
	return manager->setPlaybackDeviceVolume(volume);
}

int ArtcCliAudioDeviceManager::getPlaybackDeviceVolume(int % volume)
{
	int v;
	AAudioDeviceManager manager(engine);
	int result = manager->getPlaybackDeviceVolume(&v);
	volume = v;
	return result;
}

int ArtcCliAudioDeviceManager::setRecordingDeviceVolume(int volume)
{
	AAudioDeviceManager manager(engine);
	return manager->setRecordingDeviceVolume(volume);
}

int ArtcCliAudioDeviceManager::getRecordingDeviceVolume(int % volume)
{
	int v, result;
	AAudioDeviceManager manager(engine);
	result = manager->getRecordingDeviceVolume(&v);
	volume = v;
	return result;
}

int ArtcCliAudioDeviceManager::setPlaybackDeviceMute(bool mute)
{
	AAudioDeviceManager manager(engine);
	return manager->setPlaybackDeviceMute(mute);
}

int ArtcCliAudioDeviceManager::getPlaybackDeviceMute(bool % mute)
{
	bool m;
	AAudioDeviceManager manager(engine);
	int result = manager->getPlaybackDeviceMute(&m);
	mute = m;
	return result;
}

int ArtcCliAudioDeviceManager::setRecordingDeviceMute(bool mute)
{
	AAudioDeviceManager manager(engine);
	return manager->setRecordingDeviceMute(mute);
}

int ArtcCliAudioDeviceManager::getRecordingDeviceMute(bool & mute)
{
	bool m;
	AAudioDeviceManager manager(engine);
	int result = manager->getRecordingDeviceMute(&m);
	mute = m;
	return result;
}

int ArtcCliAudioDeviceManager::startPlaybackDeviceTest(String ^ path)
{
	AAudioDeviceManager manager(engine);
	return manager->startPlaybackDeviceTest(MarshalString(path).c_str());
}

int ArtcCliAudioDeviceManager::stopPlaybackDeviceTest()
{
	AAudioDeviceManager manager(engine);
	return manager->stopPlaybackDeviceTest();
}

int ArtcCliAudioDeviceManager::startRecordingDeviceTest(int indicationInterval)
{
	AAudioDeviceManager manager(engine);
	return manager->startRecordingDeviceTest(indicationInterval);
}

int ArtcCliAudioDeviceManager::stopRecordingDeviceTest()
{
	AAudioDeviceManager manager(engine);
	return manager->stopRecordingDeviceTest();
}

int ArtcCliAudioDeviceManager::getPlaybackDevice(String^% deviceId)
{
	AAudioDeviceManager manager(engine);
	char tmp[MAX_DEVICE_ID_LENGTH];
	int result = manager->getPlaybackDevice(tmp);
	if (result == 0) deviceId = gcnew String(tmp);
	else deviceId = nullptr;
	return result;
}

int ArtcCliAudioDeviceManager::getPlaybackDeviceInfo(String^% deviceId, String^% deviceName)
{
	AAudioDeviceManager manager(engine);
	char tmpId[MAX_DEVICE_ID_LENGTH];
	char tmpName[MAX_DEVICE_ID_LENGTH];
	int result = manager->getPlaybackDeviceInfo(tmpId, tmpName);
	if (result == 0) {
		deviceId = gcnew String(tmpId);
		deviceName = gcnew String(tmpName);
	}
	else {
		deviceId = nullptr;
		deviceName = nullptr;
	}
	return result;
}

int ArtcCliAudioDeviceManager::getRecordingDevice(String^% deviceId)
{
	AAudioDeviceManager manager(engine);
	char tmpId[MAX_DEVICE_ID_LENGTH];
	int result = manager->getRecordingDevice(tmpId);
	if (result == 0) deviceId = gcnew String(tmpId);
	else deviceId = nullptr;
	return result;
}

int ArtcCliAudioDeviceManager::getRecordingDeviceInfo(String^% deviceId, String^% deviceName)
{
	AAudioDeviceManager manager(engine);
	char tmpId[MAX_DEVICE_ID_LENGTH];
	char tmpName[MAX_DEVICE_ID_LENGTH];
	int result = manager->getRecordingDeviceInfo(tmpId, tmpName);
	if (result == 0) {
		deviceId = gcnew String(tmpId);
		deviceName = gcnew String(tmpName);
	}
	else {
		deviceId = nullptr;
		deviceName = nullptr;
	}
	return result;
}

int ArtcCliAudioDeviceManager::startAudioDeviceLoopbackTest(int interval)
{
	AAudioDeviceManager manager(engine);
	return manager->startAudioDeviceLoopbackTest(interval);
}

int ArtcCliAudioDeviceManager::stopAudioDeviceLoopbackTest()
{
	AAudioDeviceManager manager(engine);
	return manager->stopAudioDeviceLoopbackTest();
}


