#pragma once

#include "IArRtcEngine.h"
#include "ArtcCliAudioDeviceManager.h"
#include "ArtcCliEventHandler.h"
#include "ArtcCliPacketObserver.h"
#include "ArtcCliRawFrameObserver.h"
#include "ArtcCliVideoDeviceManager.h"
#include "ArtcCliLibraryEnum.h"

#include <msclr\marshal.h>
#include <string>
#include <iostream>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace ar::rtc;
using namespace msclr::interop;

namespace ArtcCliLibrary {

	static char* strcopy(std::string& source) {
		char* dest = reinterpret_cast<char*>(malloc(sizeof(char) * source.length() + 1));
		if (dest == 0) return nullptr;
		else {
			strcpy_s(dest, source.length() + 1, source.c_str());
			return dest;
		}
	}

	static std::string MarshalString(String^ s)
	{
		if (s == nullptr) return std::string();

		IntPtr middleStr = Runtime::InteropServices::Marshal::StringToHGlobalAnsi(s);
		std::string result(reinterpret_cast<char*>(middleStr.ToPointer()));
		Runtime::InteropServices::Marshal::FreeHGlobal(middleStr);
		return result;
	}

	public
	ref class CliAudioVolumeInfo
	{
	public:
		String^ uid;
		unsigned int volume; // [0,255]
		unsigned int vad;
		String^ channelId;

		CliAudioVolumeInfo() {}

		CliAudioVolumeInfo(const AudioVolumeInfo& info) :
			uid(gcnew String(info.uid)),
			volume(info.volume),
			vad(info.vad),
			channelId(gcnew String(info.channelId))
		{

		}
	};

	public
	ref class CliRtcStats
	{
	public:
		unsigned int duration;
		unsigned int txBytes;
		unsigned int rxBytes;

		unsigned int txAudioBytes;
		unsigned int txVideoBytes;

		unsigned int rxAudioBytes;
		unsigned int rxVideoBytes;

		unsigned short txKBitRate;
		unsigned short rxKBitRate;

		unsigned short rxAudioKBitRate;
		unsigned short txAudioKBitRate;

		unsigned short rxVideoKBitRate;
		unsigned short txVideoKBitRate;

		unsigned short lastmileDelay;

		unsigned short txPacketLossRate;
		unsigned short rxPacketLossRate;

		unsigned int userCount;
		double cpuAppUsage;
		double cpuTotalUsage;

		int gatewayRtt;
		double memoryAppUsageRatio;
		double memoryTotalUsageRatio;
		int memoryAppUsageInKbytes;

		CliRtcStats(ar::rtc::RtcStats raw)
		{
			duration = raw.duration;
			rxBytes = raw.rxBytes;
			txBytes = raw.txBytes;

			txAudioBytes = raw.txAudioBytes;
			txVideoBytes = raw.txVideoBytes;

			rxAudioBytes = raw.rxAudioBytes;
			rxVideoBytes = raw.rxVideoBytes;

			txKBitRate = raw.txKBitRate;
			rxKBitRate = raw.rxKBitRate;

			rxAudioKBitRate = raw.rxAudioKBitRate;
			txAudioKBitRate = raw.txAudioKBitRate;

			rxVideoKBitRate = raw.rxVideoKBitRate;
			txVideoKBitRate = raw.txVideoKBitRate;

			lastmileDelay = raw.lastmileDelay;

			txPacketLossRate = raw.txPacketLossRate;
			rxPacketLossRate = raw.rxPacketLossRate;

			cpuAppUsage = raw.cpuAppUsage;
			cpuTotalUsage = raw.cpuTotalUsage;
			userCount = raw.userCount;

			gatewayRtt = raw.gatewayRtt;
			memoryAppUsageRatio = raw.memoryAppUsageRatio;
			memoryTotalUsageRatio = raw.memoryTotalUsageRatio;
			memoryAppUsageInKbytes = raw.memoryAppUsageInKbytes;
		}
	};


	public
	ref class CliLocalVideoStats
	{
	public:
		int sentBitrate;
		int sentFrameRate;
		int encoderOutputFrameRate;
		int rendererOutputFrameRate;
		int targetBitrate;
		int targetFrameRate;
		QualityAdaptIndication qualityAdaptIndication;
		int encodedBitrate;
		int encodedFrameWidth;
		int encodedFrameHeight;
		int encodedFrameCount;
		VideoCodecType codecType;
		UINT txPacketLossRate;
		int captureFrameRate;

		CliLocalVideoStats(const LocalVideoStats& stats) :
			sentBitrate(stats.sentBitrate),
			sentFrameRate(stats.sentFrameRate),
			encoderOutputFrameRate(stats.encoderOutputFrameRate),
			rendererOutputFrameRate(stats.rendererOutputFrameRate),
			targetBitrate(stats.targetBitrate),
			targetFrameRate(stats.targetFrameRate),
			qualityAdaptIndication(static_cast<QualityAdaptIndication>(stats.qualityAdaptIndication)),
			encodedBitrate(stats.encodedBitrate),
			encodedFrameWidth(stats.encodedFrameWidth),
			encodedFrameHeight(stats.encodedFrameHeight),
			encodedFrameCount(stats.encodedFrameCount),
			codecType(static_cast<VideoCodecType>(stats.codecType)),
			txPacketLossRate(stats.txPacketLossRate),
			captureFrameRate(stats.captureFrameRate)
		{}
	};

	public
	ref class CliRemoteVideoStats
	{
	public:
		String^ uid;
		int delay;
		int width;
		int height;
		int receivedBitrate;
		int decoderOutputFrameRate;
		int rendererOutputFrameRate;
		int packedLossRate;
		EnumRemoteVideoStreamType rxStreamType;
		int totalFrozenTime;
		int frozenRate;
		int totalActiveTime;
		int publishDuration;

		CliRemoteVideoStats(ar::rtc::RemoteVideoStats stats)
		{
			uid = gcnew String(stats.uid);
			delay = stats.delay;
			width = stats.width;
			height = stats.height;
			receivedBitrate = stats.receivedBitrate;
			decoderOutputFrameRate = stats.decoderOutputFrameRate;
			rendererOutputFrameRate = stats.rendererOutputFrameRate;
			packedLossRate = stats.packetLossRate;

			rxStreamType = static_cast<EnumRemoteVideoStreamType>(stats.rxStreamType);

			totalFrozenTime = stats.totalFrozenTime;
			frozenRate = stats.frozenRate;
			totalActiveTime = stats.totalActiveTime;

			publishDuration = stats.publishDuration;
		}
	};

	public
	ref class CliPacket
	{
	public:
		array<unsigned char>^ buffer;

		CliPacket() {}
		CliPacket(IPacketObserver::Packet& packet)
		{
			this->buffer = gcnew array<unsigned char>(packet.size);
			Marshal::Copy(IntPtr(const_cast<void*>(static_cast<const void*>(packet.buffer))), this->buffer, 0, packet.size);
		}

		void writePacket(IPacketObserver::Packet& packet)
		{
			int size = sizeof(unsigned char) * buffer->Length;
			void* tmp = malloc(size);
			Marshal::Copy(buffer, 0, IntPtr(tmp), buffer->Length);
			delete packet.buffer;
			packet.buffer = static_cast<unsigned char*>(tmp);
			packet.size = buffer->Length;
		}
	};

	public
	ref class CliAudioFrame
	{
	private:
		ar::media::IAudioFrameObserver::AudioFrame* raw=nullptr;
	public:
		EnumAudioFrameType type;
		int samples;
		int bytesPerSample;
		int channels;
		int samplesPerSec;
		array<Byte>^ data;
		int64_t renderTimeMs;
		int avsync_type;

		CliAudioFrame() {}

		CliAudioFrame(ar::media::IAudioFrameObserver::AudioFrame& raw) :
			type(static_cast<EnumAudioFrameType>(raw.type)),
			samples(raw.samples),
			bytesPerSample(raw.bytesPerSample),
			channels(raw.channels),
			samplesPerSec(raw.samplesPerSec),
			renderTimeMs(raw.renderTimeMs),
			avsync_type(raw.avsync_type)
		{
			data = gcnew array<Byte>(samples * bytesPerSample);
			Marshal::Copy(IntPtr(raw.buffer), data, 0, samples * bytesPerSample);
		}

		void writeRaw(ar::media::IAudioFrameObserver::AudioFrame& raw)
		{
			bool sizeModified = (samples * bytesPerSample) != (raw.samples * raw.bytesPerSample);
			raw.type = static_cast<ar::media::IAudioFrameObserver::AUDIO_FRAME_TYPE>(type);
			raw.samples = samples;
			raw.bytesPerSample = bytesPerSample;
			raw.channels = channels;
			raw.samplesPerSec = samplesPerSec;
			raw.renderTimeMs = renderTimeMs;
			raw.avsync_type = avsync_type;
			if (sizeModified) {
				delete raw.buffer;
				raw.buffer = malloc(sizeof(Byte) * data->Length);
			}
			Marshal::Copy(data, 0, IntPtr(raw.buffer), data->Length);
		}

		operator IAudioFrameObserver::AudioFrame* () {
			if(raw)
				return raw;
			raw = new ar::media::IAudioFrameObserver::AudioFrame();
			writeRaw(*raw);
			return raw;
		}

		void release()
		{
			if(raw)
			{
				delete raw;
				raw = nullptr;
			}
		}
	};


	public
	ref class CliVideoFrame
	{
	public:
		EnumVideoFrameType type;
		int width;   //width of video frame
		int height;  //height of video frame
		int yStride; //stride of Y data buffer
		int uStride; //stride of U data buffer
		int vStride; //stride of V data buffer
		array<Byte>^ ybuffer;
		array<Byte>^ ubuffer;
		array<Byte>^ vbuffer;
		int rotation; // rotation of this frame (0, 90, 180, 270)
		int64_t renderTimeMs;
		int avsync_type;

		CliVideoFrame(ar::media::IVideoFrameObserver::VideoFrame& raw) :
			type(static_cast<EnumVideoFrameType>(raw.type)),
			width(raw.width),
			height(raw.height),
			yStride(raw.yStride),
			uStride(raw.uStride),
			vStride(raw.vStride),
			rotation(raw.rotation),
			renderTimeMs(raw.renderTimeMs),
			avsync_type(raw.avsync_type)
		{
			int size = width * height;
			ybuffer = gcnew array<Byte>(size);
			Marshal::Copy(IntPtr(raw.yBuffer), ybuffer, 0, size);
			ubuffer = gcnew array<Byte>(size / 4);
			Marshal::Copy(IntPtr(raw.uBuffer), ubuffer, 0, size / 4);
			vbuffer = gcnew array<Byte>(size / 4);
			Marshal::Copy(IntPtr(raw.vBuffer), vbuffer, 0, size / 4);
		}

		void writeRaw(ar::media::IVideoFrameObserver::VideoFrame& raw)
		{
			int size = width * height;
			int sizeModified = (raw.width * raw.height) != size;

			raw.type = static_cast<IVideoFrameObserver::VIDEO_FRAME_TYPE>(type);
			raw.width = width;
			raw.height = height;
			raw.yStride = yStride;
			raw.uStride = uStride;
			raw.vStride = vStride;
			raw.rotation = rotation;
			raw.renderTimeMs = renderTimeMs;
			raw.avsync_type = avsync_type;

			if (sizeModified)
			{
				raw.yBuffer = malloc(sizeof(Byte) * size);
				raw.uBuffer = malloc(sizeof(Byte) * size / 4);
				raw.vBuffer = malloc(sizeof(Byte) * size / 4);
			}
			Marshal::Copy(ybuffer, 0, IntPtr(raw.yBuffer), size);
			Marshal::Copy(ubuffer, 0, IntPtr(raw.uBuffer), size / 4);
			Marshal::Copy(vbuffer, 0, IntPtr(raw.vBuffer), size / 4);
		}
	};

	public ref class CliRectangle
	{
	public:
		int x;
		int y;
		int width;
		int height;

		CliRectangle() : x(0), y(0), width(0), height(0) {}
		CliRectangle(const ar::rtc::Rectangle& rect) : x(rect.x), y(rect.y), width(rect.width), height(rect.height) {}
		operator ar::rtc::Rectangle() {
			return ar::rtc::Rectangle(x, y, width, height);
		}
	};


	public
	ref class CliRect
	{
	private:
		ar::rtc::Rect* result=nullptr;
	public:
		int top;
		int left;
		int bottom;
		int right;

		CliRect() : top(0), left(0), bottom(0), right(0) {}
		CliRect(int t, int l, int b, int r) : top(t), left(l), bottom(b), right(r) {}

		ar::rtc::Rect* toRaw()
		{
			if(result)
				return result;
			result = new ar::rtc::Rect();
			result->top = top;
			result->left = left;
			result->bottom = bottom;
			result->right = right;
			return result;
		}
		void release()
		{
			if(result)
			{
				delete result;
				result = nullptr;
			}
		}
	};

	public
	ref class CliTranscodingUser
	{
	private:
		TranscodingUser* result=nullptr;
	public:
		String^ uid;

		int x;
		int y;
		int width;
		int height;

		int zOrder;
		double alpha;
		int audioChannel;

		CliTranscodingUser()
			:x(0), y(0), width(0), height(0), zOrder(0), alpha(1.0), audioChannel(0)
		{
		}

		operator TranscodingUser* ()
		{
			if(result)
				return result;
			result = new TranscodingUser();
			result->uid = MarshalString(uid).c_str();
			result->x = x;
			result->y = y;
			result->width = width;
			result->height = height;
			result->zOrder = zOrder;
			result->alpha = alpha;
			result->audioChannel = audioChannel;
			return result;
		}

		void release()
		{
			if(result)
			{
				delete result;
				result=nullptr;
			}
		}
	};

	public
	ref class CliRtcImage
	{
	private:
		RtcImage* raw = nullptr;
	public:
		CliRtcImage() : url(nullptr),
			x(0),
			y(0),
			width(0),
			height(0)
		{
		}
		/** URL address of the image on the broadcasting video. */
		String^ url;
		/** Horizontal position of the image from the upper left of the broadcasting video. */
		int x;
		/** Vertical position of the image from the upper left of the broadcasting video. */
		int y;
		/** Width of the image on the broadcasting video. */
		int width;
		/** Height of the image on the broadcasting video. */
		int height;

		operator RtcImage* ()
		{
			if(raw)
				return raw;
			raw = new RtcImage();
			raw->x = x;
			raw->y = y;
			raw->width = width;
			raw->height = height;
			raw->url = strcopy(MarshalString(url));
			return raw;
		}

		void release()
		{
			if(raw)
			{
				delete raw;
				raw=nullptr;
			}
		}
	};


	public
	ref class CliLiveTranscoding
	{
	public:
		int width;
		int height;
		int videoBitrate;
		int videoFramerate;
		bool lowLatency;
		int videoGop;
		VideoCodecProfileType videoCodecProfile;
		unsigned int backgroundColor;
		unsigned int userCount;
		CliTranscodingUser^ transcodingUsers;
		String^ transcodingExtraInfo;
		String^ metadata;
		CliRtcImage^ watermark;
		CliRtcImage^ backgroundImage;
		AudioSampleRateType audioSampleRate;
		int audioBitrate;
		int audioChannels;
		AudioCodecProfileType audioCodecProfile;

		CliLiveTranscoding()
			: width(360), height(640), videoBitrate(400), videoFramerate(15), lowLatency(false), backgroundColor(0x000000), videoGop(30), videoCodecProfile(VideoCodecProfileType::VIDEO_CODEC_PROFILE_HIGH), userCount(0), transcodingUsers(), transcodingExtraInfo(nullptr), audioSampleRate(AudioSampleRateType::AUDIO_SAMPLE_RATE_48000), audioBitrate(48), audioChannels(1)
		{
		}

		operator LiveTranscoding() {
			LiveTranscoding raw;
			raw.width = width;
			raw.height = height;
			raw.videoBitrate = videoBitrate;
			raw.videoFramerate = videoFramerate;
			raw.lowLatency = lowLatency;
			raw.videoGop = videoGop;
			raw.videoCodecProfile = static_cast<VIDEO_CODEC_PROFILE_TYPE>(videoCodecProfile);
			raw.backgroundColor = backgroundColor;
			raw.userCount = userCount;
			raw.transcodingUsers = transcodingUsers;
			raw.transcodingExtraInfo = strcopy(MarshalString(transcodingExtraInfo));
			raw.metadata = strcopy(MarshalString(metadata));
			raw.watermark = watermark;
			raw.backgroundImage = backgroundImage;
			raw.audioSampleRate = static_cast<AUDIO_SAMPLE_RATE_TYPE>(audioSampleRate);
			raw.audioBitrate = audioBitrate;
			raw.audioChannels = audioChannels;
			raw.audioCodecProfile = static_cast<AUDIO_CODEC_PROFILE_TYPE>(audioCodecProfile);
			return raw;
		}
	};


	public
	ref class CliInjectStreamConfig
	{
	public:
		int width;
		int height;
		int videoGop;
		int videoFramerate;
		int videoBitrate;
		AudioSampleRateType audioSampleRate;
		int audioBitrate;
		int audioChannels;

		// width / height default set to 0 means pull the stream with its original resolution
		CliInjectStreamConfig()
			: width(0), height(0), videoGop(30), videoFramerate(15), videoBitrate(400), audioSampleRate(AudioSampleRateType::AUDIO_SAMPLE_RATE_48000), audioBitrate(48), audioChannels(1)
		{
		}

		operator InjectStreamConfig() {
			InjectStreamConfig raw;
			raw.width = width;
			raw.height = height;
			raw.videoGop = videoGop;
			raw.videoFramerate = videoFramerate;
			raw.videoBitrate = videoBitrate;
			raw.audioSampleRate = static_cast<AUDIO_SAMPLE_RATE_TYPE>(audioSampleRate);
			raw.audioBitrate = audioBitrate;
			raw.audioChannels = audioChannels;
			return raw;
		}
	};


	public
	ref class CliVideoDimensions
	{
	public:
		int width;
		int height;

		CliVideoDimensions()
			: width(1920), height(1080)
		{
		}
		CliVideoDimensions(int w, int h)
			: width(w), height(h)
		{
		}
	};

	public ref class CliScreenCaptureParameters {
	public:
		CliVideoDimensions^ dimensions;
		int frameRate;
		int bitrate;
		bool captureMouseCursor;
		bool windowFocus;
		List<IntPtr> excludeWindows;

		CliScreenCaptureParameters() : 
			dimensions(), 
			frameRate(5), 
			bitrate(0), 
			captureMouseCursor(false), 
			windowFocus(false),
			excludeWindows(gcnew List<IntPtr>())
		{}
		operator ar::rtc::ScreenCaptureParameters() { 
			auto result = ar::rtc::ScreenCaptureParameters(
				dimensions->width, 
				dimensions->height, 
				frameRate, 
				bitrate, 
				captureMouseCursor,
				windowFocus); 
			result.excludeWindowCount = this->excludeWindows.Count;
			if (result.excludeWindowCount == 0) result.excludeWindowList = NULL;
			else {
				result.excludeWindowList = new view_t[result.excludeWindowCount];
				for (int i = 0; i < result.excludeWindowCount; i++) {
					result.excludeWindowList[i] = this->excludeWindows[i].ToPointer();
				}
			}
			return result;
		}
	};


	public
	ref class CliVideoEncoderConfiguration
	{
	public:
		CliVideoDimensions^ dimensions;
		FrameRate frameRate;
		int minFrameRate;
		int bitrate;
		int minBitrate;
		OrientationMode orientationMode;
		DegradationPreference degradationPreference;
		EnumVideoMirrorModeType mirrorMode;

		CliVideoEncoderConfiguration(
			const CliVideoDimensions^ d, FrameRate f,
			int b, OrientationMode m)
			: CliVideoEncoderConfiguration(d->width, d->height, f, b, m)
		{
		}

		CliVideoEncoderConfiguration(
			int width, int height, FrameRate f,
			int b, OrientationMode m) : CliVideoEncoderConfiguration()
		{
			dimensions = gcnew CliVideoDimensions(width, height);
			frameRate = f;
			bitrate = b;
			orientationMode = m;
		}

		CliVideoEncoderConfiguration(
			const CliVideoDimensions^ d, FrameRate f,
			int b, OrientationMode m, EnumVideoMirrorModeType mt)
			: CliVideoEncoderConfiguration(d->width, d->height, f, b, m, mt)
		{
		}

		CliVideoEncoderConfiguration(
			int width, int height, FrameRate f,
			int b, OrientationMode m, EnumVideoMirrorModeType mt) : CliVideoEncoderConfiguration()
		{
			dimensions = gcnew CliVideoDimensions(width, height);
			frameRate = f;
			bitrate = b;
			orientationMode = m;
		}

		CliVideoEncoderConfiguration()
			: dimensions(),
			frameRate(FrameRate::FRAME_RATE_FPS_15),
			minFrameRate(-1),
			bitrate(STANDARD_BITRATE),
			minBitrate(DEFAULT_MIN_BITRATE),
			orientationMode(OrientationMode::ORIENTATION_MODE_ADAPTIVE),
			degradationPreference(DegradationPreference::MAINTAIN_QUALITY),
			mirrorMode(EnumVideoMirrorModeType::VIDEO_MIRROR_MODE_AUTO)

		{
		}

		operator VideoEncoderConfiguration() {
			VideoEncoderConfiguration raw;
			raw.dimensions = VideoDimensions(dimensions->width, dimensions->height);
			raw.frameRate = static_cast<FRAME_RATE>(frameRate);
			raw.minFrameRate = minFrameRate;
			raw.bitrate = bitrate;
			raw.minBitrate = minBitrate;
			raw.orientationMode = static_cast<ORIENTATION_MODE>(orientationMode);
			raw.degradationPreference = static_cast<DEGRADATION_PREFERENCE>(degradationPreference);
			raw.mirrorMode = static_cast<VIDEO_MIRROR_MODE_TYPE>(mirrorMode);
			return raw;
		}

	};


	public
	ref class CliExternalVideoFrame
	{
	private:
		ExternalVideoFrame* result = nullptr;
	public:
		VideoBufferType type;
		EnumVideoPixelFormate format;
		array<Byte>^ buffer;
		int stride;
		int height;
		int cropLeft;
		int cropTop;
		int cropRight;
		int cropBottom;
		int rotation;
		long long timestamp;

		operator ExternalVideoFrame* () {
			if(result)
				return result;
			result = new ExternalVideoFrame();
			result->type = static_cast<ExternalVideoFrame::VIDEO_BUFFER_TYPE>(type);
			result->format = static_cast<ExternalVideoFrame::VIDEO_PIXEL_FORMAT>(format);
			result->stride = stride;
			result->height = height;
			result->cropLeft = cropLeft;
			result->cropTop = cropTop;
			result->cropRight = cropRight;
			result->cropBottom = cropBottom;
			result->rotation = rotation;
			result->timestamp = timestamp;
			result->buffer = malloc(buffer->Length * sizeof(Byte));
			Marshal::Copy(buffer, 0, IntPtr(result->buffer), buffer->Length);
			return result;
		}

		void release()
		{
			if(result)
			{
				free(result->buffer);
				result->buffer=nullptr;
				delete result;
				result = nullptr;
			}
		}
	};


	public
	ref class CliRemoteAudioStats
	{
	public:
		String^ uid;
		int quality;
		int networkTransportDelay;
		int jitterBufferDelay;
		int audioLossRate;
		int numChannels;
		int receivedSampleRate;
		int receivedBitrate;
		int totalFrozenTime;
		int frozenRate;
		int totalActiveTime;
		int publishDuration;
		int qoeQuality;
		int qualityChangedReason;
		int mosValue;

		CliRemoteAudioStats() {}

		CliRemoteAudioStats(const RemoteAudioStats& raw) :
			uid(gcnew String(raw.uid)),
			quality(raw.quality),
			networkTransportDelay(raw.networkTransportDelay),
			jitterBufferDelay(raw.jitterBufferDelay),
			audioLossRate(raw.audioLossRate),
			numChannels(raw.numChannels),
			receivedSampleRate(raw.receivedSampleRate),
			receivedBitrate(raw.receivedBitrate),
			totalFrozenTime(raw.totalFrozenTime),
			frozenRate(raw.frozenRate),
			totalActiveTime(raw.totalActiveTime),
			publishDuration(raw.publishDuration),
			qoeQuality(raw.qoeQuality),
			qualityChangedReason(raw.qualityChangedReason),
			mosValue(raw.mosValue)
		{
		}

		void writeRaw(RemoteAudioStats& raw)
		{
			raw.uid = MarshalString(uid).c_str();
			raw.quality = quality;
			raw.networkTransportDelay = networkTransportDelay;
			raw.jitterBufferDelay = jitterBufferDelay;
			raw.audioLossRate = audioLossRate;
			raw.numChannels = numChannels;
			raw.receivedSampleRate = receivedSampleRate;
			raw.receivedBitrate = receivedBitrate;
			raw.totalFrozenTime = totalFrozenTime;
			raw.frozenRate = frozenRate;
			raw.totalActiveTime = totalActiveTime;
			raw.publishDuration = publishDuration;
			raw.qoeQuality = qoeQuality;
			raw.qualityChangedReason = qualityChangedReason;
			raw.mosValue = mosValue;
		}
	};

	public ref class CliLocalAudioStats {
	public:
		int numChannels;
		int sentSampleRate;
		int sentBitrate;
		UINT txPacketLossRate;

		CliLocalAudioStats() : numChannels(0), sentSampleRate(0), sentBitrate(0), txPacketLossRate(0) {}
		CliLocalAudioStats(const LocalAudioStats& stats) : 
			numChannels(stats.numChannels), 
			sentSampleRate(stats.sentSampleRate), 
			sentBitrate(stats.sentBitrate),
			txPacketLossRate(stats.txPacketLossRate)
		{}
	};

	public ref class CliChannelMediaInfo {
	private:
		ChannelMediaInfo* info = nullptr;
	public:
		String^ channel;
		String^ token;
		uid_t uid;

		operator ChannelMediaInfo* () {
			if(info)
				return  info;
			info = new ChannelMediaInfo();
			info->channelName = MarshalString(channel).c_str();
			info->token = MarshalString(token).c_str();
			info->uid = uid;
			return info;
		}

		void release()
		{
			if(info)
			{
				delete info;
				info=nullptr;
			}
		}
	};

	public ref class CliChannelMediaOptions {
	public:
		bool autoSubscribeAudio;
		bool autoSubscribeVideo;

		CliChannelMediaOptions() :
			autoSubscribeAudio(true),
			autoSubscribeVideo(true)
		{}

		operator ChannelMediaOptions () {
			ChannelMediaOptions result;
			result.autoSubscribeAudio = autoSubscribeAudio;
			result.autoSubscribeVideo = autoSubscribeVideo;
			return result;
		}
	};

	public ref class CliChannelMediaRelayConfiguration {
	public:
		CliChannelMediaInfo^ src;
		CliChannelMediaInfo^ dest;
		int destCount;

		CliChannelMediaRelayConfiguration() :
			src(nullptr),
			dest(nullptr),
			destCount(0) {}
		operator ChannelMediaRelayConfiguration() {
			ChannelMediaRelayConfiguration config;
			config.srcInfo = src;
			config.destInfos = dest;
			config.destCount = destCount;
			return config;
		}
	};

	public ref class CliLastmileProbeConfig {
	public:
		bool probeUplink;
		bool probeDownlink;
		unsigned int expectedUplinkBitrate;
		unsigned int expectedDownlinkBitrate;

		operator LastmileProbeConfig() {
			LastmileProbeConfig config;
			config.probeUplink = probeUplink;
			config.probeDownlink = probeDownlink;
			config.expectedUplinkBitrate = expectedUplinkBitrate;
			config.expectedDownlinkBitrate = expectedDownlinkBitrate;
			return config;
		}
	};

	public ref class CliLastmileProbeOneWayResult {
	public:
		unsigned int packetLossRate;
		unsigned int jitter;
		unsigned int availableBandwidth;

		CliLastmileProbeOneWayResult() {}

		CliLastmileProbeOneWayResult(const LastmileProbeOneWayResult& val) :
			packetLossRate(val.packetLossRate),
			jitter(val.jitter),
			availableBandwidth(val.availableBandwidth)
		{}

		operator LastmileProbeOneWayResult() {
			LastmileProbeOneWayResult result;
			result.packetLossRate = packetLossRate;
			result.jitter = jitter;
			result.availableBandwidth = availableBandwidth;
			return result;
		}
	};

	public ref class CliLastmileProbeResult {
	public:
		LastmileProbeResultState state;
		CliLastmileProbeOneWayResult^ uplinkReport;
		CliLastmileProbeOneWayResult^ downlinkReport;
		unsigned int rtt;

		CliLastmileProbeResult() {}

		CliLastmileProbeResult(const LastmileProbeResult& val) :
			state(static_cast<LastmileProbeResultState>(val.state)),
			uplinkReport(gcnew CliLastmileProbeOneWayResult(val.uplinkReport)),
			downlinkReport(gcnew CliLastmileProbeOneWayResult(val.downlinkReport)),
			rtt(val.rtt)
		{

		}

		operator LastmileProbeResult() {
			LastmileProbeResult result;
			result.state = static_cast<LASTMILE_PROBE_RESULT_STATE>(state);
			result.uplinkReport = uplinkReport->operator LastmileProbeOneWayResult();
			result.downlinkReport = downlinkReport->operator ar::rtc::LastmileProbeOneWayResult();
			result.rtt = rtt;
			return result;
		}
	};

	public ref class CliMetadata {
	public:
		uid_t uid;
		array<Byte>^ buffer;
		long long timeStampMs;

		CliMetadata(const IMetadataObserver::Metadata& data) :
			uid(data.uid),
			timeStampMs(data.timeStampMs)
		{
			buffer = gcnew array<Byte>(data.size);
			Marshal::Copy(IntPtr(data.buffer), buffer, 0, data.size);
		}
	};

	public ref class CliWatermarkOptions {
	public:
		bool visibleInPreview;
		CliRectangle^ positionInLandscapeMode;
		CliRectangle^ positionInPortraitMode;

		CliWatermarkOptions() :
			visibleInPreview(false),
			positionInLandscapeMode(gcnew CliRectangle()),
			positionInPortraitMode(gcnew CliRectangle())
		{}

		operator WatermarkOptions() {
			WatermarkOptions options;
			options.visibleInPreview = visibleInPreview;
			options.positionInLandscapeMode = positionInLandscapeMode->operator ar::rtc::Rectangle();
			options.positionInPortraitMode = positionInPortraitMode->operator ar::rtc::Rectangle();
			return options;
		}

	};

	public ref class CliCameraCaptureConfiguration {
	public:
		EnumCapturerOutputPreference preference;

		operator CameraCapturerConfiguration() {
			CameraCapturerConfiguration config;
			config.preference = static_cast<CAPTURER_OUTPUT_PREFERENCE>(preference);
			return config;
		}
	};

	public ref class CliBeautyOptions {
	public:
		EnumLighteningContrastLevel lighteningContrastLevel;
		float lighteningLevel;
		float smoothnessLevel;
		float rednessLevel;
		float sharpnessLevel;

		CliBeautyOptions() :
			lighteningLevel(0),
			smoothnessLevel(0),
			rednessLevel(0),
			sharpnessLevel(0),
			lighteningContrastLevel(EnumLighteningContrastLevel::LIGHTENING_CONTRAST_NORMAL) {}

		operator BeautyOptions () {
			return BeautyOptions(
				static_cast<BeautyOptions::LIGHTENING_CONTRAST_LEVEL>(lighteningContrastLevel),
				lighteningLevel,
				smoothnessLevel,
				rednessLevel,
				sharpnessLevel
				);
		}

	};

	public ref class CliEncryptionConfig {
	public:
		String^ key;
		EnumEncryptionMode mode;

		CliEncryptionConfig() : mode(EnumEncryptionMode::AES_128_XTS), key(nullptr) {}

		operator EncryptionConfig() {
			auto result = EncryptionConfig();
			result.encryptionKey = key == nullptr ? NULL : MarshalString(key).c_str();
			result.encryptionMode = (ENCRYPTION_MODE)mode;
			return result;
		}
	};

	public ref class CliLogConfig {
	public:
		String^ filePath = nullptr;
		int fileSize = -1;
		EnumLogLevel level = EnumLogLevel::LOG_LEVEL_INFO;

		LogConfig to() {
			auto result = LogConfig();
			result.filePath = filePath == nullptr ? NULL : MarshalString(filePath).c_str();
			result.fileSize = fileSize;
			result.level = (ar::LOG_LEVEL)level;
			return result;
		}
	};

	public ref class CliRtcEngineContext {
	public:
		String^ vendorKey = nullptr;
		EnumAreaCode areaCode = EnumAreaCode::AREA_CODE_GLOB;
		CliLogConfig^ logConfig = gcnew CliLogConfig();
	};

	public ref class CliDataStreamConfig {
	public:
		bool syncWithAudio;
		bool ordered;

		operator DataStreamConfig() {
			auto result = DataStreamConfig();
			result.ordered = ordered;
			result.syncWithAudio = syncWithAudio;
			return result;
		}
	};
}