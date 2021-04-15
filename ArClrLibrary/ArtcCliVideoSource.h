#pragma once
#include "common.h"
#include "IArRtcEngine.h"
#include "ArtcCliLibraryEnum.h"

using namespace System;
using namespace ar::rtc;
using namespace ar::media;
using namespace Collections::Generic;
using namespace Runtime::InteropServices;

namespace ArtcCliLibrary {

	public class VideoSource : public IVideoSource
	{
	public:
		using OnInitialize = FT<bool>;
		using OnDispose = ET<void>;
		using OnStart = FT<bool>;
		using OnStop = ET<void>;
		using GetBufferType = FT<ExternalVideoFrame::VIDEO_PIXEL_FORMAT>;
		using GetVideoCaptureType = FT<VIDEO_CAPTURE_TYPE>;
		using GetVideoContentHint = FT<VideoContentHint>;

	public:
		OnInitialize::Pointer onInitializeEvent = nullptr;
		OnDispose::Pointer onDisposeEvent = nullptr;
		OnStart::Pointer onStartEvent = nullptr;
		OnStop::Pointer onStopEvent = nullptr;
		GetVideoCaptureType::Pointer getVideoCaptureTypeEvent = nullptr;
		GetBufferType::Pointer getBufferTypeEvent = nullptr;
		GetVideoContentHint::Pointer getVideoContentHintEvent = nullptr;

	public:
		// Inherited via IVideoSource
		virtual bool onInitialize(IVideoFrameConsumer* consumer) override;
		virtual void onDispose() override;
		virtual bool onStart() override;
		virtual void onStop() override;
		virtual ExternalVideoFrame::VIDEO_PIXEL_FORMAT getBufferType() override;
		virtual VIDEO_CAPTURE_TYPE getVideoCaptureType() override;
		virtual VideoContentHint getVideoContentHint() override;
	public:
		void Consume(const unsigned char* buffer, ExternalVideoFrame::VIDEO_PIXEL_FORMAT frameType, int width, int height, int rotation, long timestamp);
		IVideoFrameConsumer* consumer;
	};

	public interface class CliVideoSource {
		bool onInitialize();
		void onDispose();
		bool onStart();
		void onStop();

		EnumVideoPixelFormate getBufferType();
		EnumVideoCaptureType getVideoCaptureType();
		EnumVideoContentHint getVideoContentHint();

		delegate void ConsumeRawVideoFrameDelegate(array<Byte>^ buffer, EnumVideoPixelFormate frameType, int width, int height, int rotation, long timestamp);
		event ConsumeRawVideoFrameDelegate^ consumeRawVideoFrame;
	};

	public ref class InnerVideoSource {
	public:
		InnerVideoSource(CliVideoSource^ clr);
		~InnerVideoSource();
		!InnerVideoSource();

		operator IVideoSource*() {
			return this->raw;
		}
		void dispose();
	private:
		void linkCliToRaw();
		void* regEvent(Object^ obj);

		bool NativeOnInitialize();
		void NativeOnDispose();
		bool NativeOnStart();
		void NativeOnStop();
		ExternalVideoFrame::VIDEO_PIXEL_FORMAT NativeGetBufferType();
		VIDEO_CAPTURE_TYPE NativeGetVideoCaptureType();
		VideoContentHint NativeGetVideoContentHint();

		void Consume(array<Byte>^ buffer, EnumVideoPixelFormate frameType, int width, int height, int rotation, long timestamp);
	private:
		CliVideoSource^ clr;
		VideoSource* raw;
		List<GCHandle>^ gchs;

	};
}

