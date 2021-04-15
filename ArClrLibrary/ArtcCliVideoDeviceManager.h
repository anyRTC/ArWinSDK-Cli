#pragma once
#include "IArRtcEngine.h"

using namespace ar::rtc;
using namespace System;

namespace ArtcCliLibrary {

	ref class ArtcCli;

	public ref class CliVideoDeviceCollection
	{
	public:
		CliVideoDeviceCollection(IVideoDeviceCollection* rawCollection);
		~CliVideoDeviceCollection();
		!CliVideoDeviceCollection();

		int getCount();
		int getDevice(int index, String^% deviceName, String^% deviceId);
		int setDevice(String^ deviceId);
	private:
		IVideoDeviceCollection* raw;

		void release();
	};

	public ref class ArtcCliVideoDeviceManager
	{
	public:
		ArtcCliVideoDeviceManager(IRtcEngine* engine);

		CliVideoDeviceCollection^ enumerateVideoDevices();
		int setDevice(String^ deviceId);
		int getDevice(String^ deviceId);
		int startDeviceTest(IntPtr hwnd);
		int stopDeviceTest();

	private:
		IRtcEngine* engine;
	};

}

