#include "stdafx.h"
#include "ArtcCliLibrary.h"
#include "IArRtcChannel.h"

using namespace ArtcCliLibrary;
using namespace System;
using namespace Runtime::InteropServices;

ArtcCli::ArtcCli()
{
	rtcEngine = nullptr;
	artcMediaEngine = nullptr;

	gchs = gcnew List<GCHandle>();
	artcEventHandler = new ArtcCliEventHandler;
	artcPacketObserver = new ArtcCliPacketObserver;
	artcAudioObserver = new ArtcCliAudioFrameObserver;
	artcVideoObserver = new ArtcCliVideoFrameObserver;
	artcMetadataObserver = new ArtcCliMetadataObserver;

	VideoFormatPreference = VideoFrameType::FRAME_TYPE_YUV420;
	IsSmoothRenderingEnabled = false;
	IsVideoRotationApplied = false;
	IsVideoMirrorApplied = false;
	IsMultipleChannelVideoFrameWanted = false;
	IsMultipleChannelAudioFrameWanted = false;
	ObservedVideoFramePosition = static_cast<UINT>(EnumVideoObserverPositionType::POSITION_POST_CAPTURER);

	MaxMetadataSize = 1024;

	innerVideoSoruce = nullptr;

	initializeEventHandler();
	initializePacketObserver();
	initializeRawFrameObserver();
	initializeMetaObserver();
}

ArtcCli::~ArtcCli()
{
	this->release();
	for each (GCHandle handle in gchs) handle.Free();
}

ArtcCli::!ArtcCli()
{
}

int ArtcCli::initialize(String^ vendorkey, [Optional] Nullable<int> areaCode)
{
	auto context = gcnew CliRtcEngineContext();
	context->vendorKey = vendorkey;
	context->areaCode = areaCode.HasValue ? (EnumAreaCode)areaCode.Value : EnumAreaCode::AREA_CODE_GLOB;
	return this->initialize(context);
}

int ArtcCliLibrary::ArtcCli::initialize(CliRtcEngineContext^ context)
{
	if (rtcEngine)
		return 0;

	rtcEngine = createARRtcEngine();
	if (rtcEngine == nullptr)
		return -1;

	//auto isOK = rtcEngine->setParameters("{\"che.audio.enable.agc\":false}");

	ar::rtc::RtcEngineContext rtcContext;
	rtcContext.appId = strcopy(MarshalString(context->vendorKey));
	rtcContext.eventHandler = artcEventHandler;
	rtcContext.areaCode = (int)context->areaCode;
	rtcContext.logConfig = context->logConfig->to();
	int result = rtcEngine->initialize(rtcContext);
	if (result == 0)
	{
		// ar设计中一旦注册了packetObserver就会开启加密,
		// 所以此接口注册移入开启加密的方法中进行
		//rtcEngine->registerPacketObserver(artcPacketObserver);
		IMediaEngine* temp = nullptr;
		if (!rtcEngine->queryInterface(ar::AR_IID_MEDIA_ENGINE, reinterpret_cast<void**>(&temp)))
		{
			artcMediaEngine = temp;
			artcMediaEngine->registerAudioFrameObserver(artcAudioObserver);
			artcMediaEngine->registerVideoFrameObserver(artcVideoObserver);
		}
		rtcEngine->registerMediaMetadataObserver(artcMetadataObserver, IMetadataObserver::METADATA_TYPE::VIDEO_METADATA);
	}
	return result;
}


void ArtcCli::release()
{
	if (rtcEngine)
	{
		if (artcMediaEngine)
			artcMediaEngine->release();
		rtcEngine->release();

		delete artcEventHandler;
		delete artcPacketObserver;
		delete artcAudioObserver;
		delete artcVideoObserver;

		rtcEngine = nullptr;
	}
}

int ArtcCli::enableVideo()
{
	return rtcEngine->enableVideo();
}

int ArtcCli::disableVideo()
{
	return rtcEngine->disableVideo();
}

int ArtcCli::enableAudio()
{
	return rtcEngine->enableAudio();
}

int ArtcCli::disableAudio()
{
	return rtcEngine->disableAudio();
}

int ArtcCli::setBeautyEffectOptions(bool enabled, CliBeautyOptions options)
{
	return rtcEngine->setBeautyEffectOptions(enabled, options);
}

ArtcCliChannel^ ArtcCli::createChannel(String^ channelId)
{
	IRtcEngine2* engine2 = dynamic_cast<IRtcEngine2*>(rtcEngine);
	return gcnew ArtcCliChannel(engine2->createChannel(MarshalString(channelId).c_str()));
}

int ArtcCli::startScreenCaptureByScreenRect(CliRectangle^ screenRect, CliRectangle^ regionRect, CliScreenCaptureParameters^ params)
{
	return rtcEngine->startScreenCaptureByScreenRect(screenRect, regionRect, params);
}

int ArtcCli::startScreenCaptureByWindowId(IntPtr windowId, CliRectangle^ regionRect, CliScreenCaptureParameters^ params)
{
	return rtcEngine->startScreenCaptureByWindowId(windowId.ToPointer(), regionRect, params);
}

int ArtcCli::setScreenCaptureContentHint(EnumVideoContentHint hint)
{
	return rtcEngine->setScreenCaptureContentHint(static_cast<VideoContentHint>(hint));
}

int ArtcCli::updateScreenCaptureParameters(CliScreenCaptureParameters^ params)
{
	return rtcEngine->updateScreenCaptureParameters(params);
}

int ArtcCli::setLocalVoiceChanger(VoiceChangerPreset changer)
{
	return rtcEngine->setLocalVoiceChanger(static_cast<VOICE_CHANGER_PRESET>(changer));
}

int ArtcCli::setLocalVoiceReverbPreset(EnumAudioReverbPreset preset)
{
	return rtcEngine->setLocalVoiceReverbPreset(static_cast<AUDIO_REVERB_PRESET>(preset));
}

int ArtcCli::enableSoundPositionIndication(bool enabled)
{
	return rtcEngine->enableSoundPositionIndication(enabled);
}

int ArtcCli::setRemoteVoicePosition(uid_t uid, double pan, double gain)
{
	return rtcEngine->setRemoteVoicePosition(uid, pan, gain);
}

int ArtcCli::startChannelMediaRelay(CliChannelMediaRelayConfiguration^ config)
{
	return rtcEngine->startChannelMediaRelay(config);
}

int ArtcCli::updateChannelMediaRelay(CliChannelMediaRelayConfiguration^ config)
{
	return rtcEngine->updateChannelMediaRelay(config);
}

int ArtcCli::stopChannelMediaRelay()
{
	return rtcEngine->stopChannelMediaRelay();
}

int ArtcCli::addVideoWatermark(String^ url, CliWatermarkOptions^ options)
{
	return rtcEngine->addVideoWatermark(MarshalString(url).c_str(), options);
}

int ArtcCli::clearVideoWatermarks()
{
	return rtcEngine->clearVideoWatermarks();
}

int ArtcCli::setHightQualityAudioParameters(bool fullband, bool stereo, bool fullBitrate)
{
	return rtcEngine->setHighQualityAudioParameters(fullband, stereo, fullBitrate);
}

int ArtcCli::startPreview()
{
	return rtcEngine->startPreview();
}

int ArtcCli::stopPreview()
{
	return rtcEngine->stopPreview();
}

int ArtcCli::enableWebSdkInteroperability(bool enabled)
{
	return rtcEngine->enableWebSdkInteroperability(enabled);
}

int ArtcCli::joinChannel(String^ token, String^ channelName, String^ channelInfo, String^ uid)
{
	return rtcEngine->joinChannel(MarshalString(token).c_str(), MarshalString(channelName).c_str(), MarshalString(channelInfo).c_str(), MarshalString(uid).c_str());
}

int ArtcCliLibrary::ArtcCli::joinChannel(String^ token, String^ channelName, String^ channelInfo, String^ uid, CliChannelMediaOptions^ options)
{
	return rtcEngine->joinChannel(MarshalString(token).c_str(), MarshalString(channelName).c_str(), MarshalString(channelInfo).c_str(), MarshalString(uid).c_str());// , options);
}

int ArtcCli::switchChannel(String^ token, String^ channelId)
{
	return rtcEngine->switchChannel(MarshalString(token).c_str(), MarshalString(channelId).c_str());
}

int ArtcCliLibrary::ArtcCli::switchChannel(String^ token, String^ channelId, CliChannelMediaOptions^ options)
{
	return rtcEngine->switchChannel(MarshalString(token).c_str(), MarshalString(channelId).c_str());// , options);
}

int ArtcCli::leaveChannel()
{
	// 如果该频道是加密频道, 用户退出时回知道解除加密, 
	// 所以设置为离开频道时, 自动退出数据包观察器, 不管原来频道是否加密
	rtcEngine->registerPacketObserver(nullptr);
	return rtcEngine->leaveChannel();
}

int ArtcCli::startScreenCapture(IntPtr windowId, int captureFreq, CliRect^ rect, int bitrate)
{
	return rtcEngine->startScreenCapture(static_cast<HWND>(windowId.ToPointer()), captureFreq, rect->toRaw(), bitrate);
}

int ArtcCli::stopScreenCapture()
{
	return rtcEngine->stopScreenCapture();
}

int ArtcCli::updateScreenCaptureRegion(CliRectangle^ rect)
{
	return rtcEngine->updateScreenCaptureRegion(rect);
}

int ArtcCli::renewToken(String^ token)
{
	return rtcEngine->renewToken(MarshalString(token).c_str());
}

int ArtcCli::setEncryptionSecret(String^ key)
{
	// 设置加密密码时, ar就开启了加密模式, 同时注册数据包观察器
	rtcEngine->registerPacketObserver(artcPacketObserver);

	return rtcEngine->setEncryptionSecret(MarshalString(key).c_str());
}

int ArtcCli::setEncryptionMode(String^ mode)
{
	return rtcEngine->setEncryptionMode(MarshalString(mode).c_str());
}

int ArtcCliLibrary::ArtcCli::enableEncryption(bool enabled, CliEncryptionConfig^ config)
{
	return rtcEngine->enableEncryption(enabled, config);
}

int ArtcCliLibrary::ArtcCli::setCloudProxy(EnumCloudProxyType type)
{
	//return rtcEngine->setCloudProxy((ar::rtc::CLOUD_PROXY_TYPE)type);
	return 0;
}

int ArtcCliLibrary::ArtcCli::enableDeepLearningDenoise(bool enable)
{
	//return rtcEngine->enableDeepLearningDenoise(enable);
	if (!enable) {
		rtcEngine->setParameters("{\"Cmd\":\"SetAudioAiNoise\", \"Enable\": 1}");
	}
	else {
		rtcEngine->setParameters("{\"Cmd\":\"SetAudioAiNoise\", \"Enable\": 0}");
	}
	return 0;
}

int ArtcCliLibrary::ArtcCli::sendCustomReportMessage(String^ id, String^ category, String^ event, String^ label, int value)
{
	return rtcEngine->sendCustomReportMessage(MarshalString(id).c_str(), MarshalString(category).c_str(), MarshalString(event).c_str(), MarshalString(label).c_str(), value);
}

int ArtcCli::getCallId(String^% id)
{
	ar::util::AString callid;
	int result = rtcEngine->getCallId(callid);
	if (result == 0)
		id = gcnew String(callid.get()->c_str());
	else
		id = nullptr;

	return result;
}

int ArtcCli::rate(String^ callid, int rating, String^ desc)
{
	return rtcEngine->rate(MarshalString(callid).c_str(), rating, MarshalString(desc).c_str());
}

int ArtcCli::complain(String^ callid, String^ desc)
{
	return rtcEngine->complain(MarshalString(callid).c_str(), MarshalString(desc).c_str());
}

int ArtcCli::startEchoTest(int intervalInSeconds)
{
	return rtcEngine->startEchoTest(intervalInSeconds);
}

int ArtcCli::stopEchoTest()
{
	return rtcEngine->stopEchoTest();
}

int ArtcCli::enableLastmileTest()
{
	return rtcEngine->enableLastmileTest();
}

int ArtcCli::disableLastmileTest()
{
	return rtcEngine->disableLastmileTest();
}

int ArtcCli::startLastmileProbeTest(CliLastmileProbeConfig^ config)
{
	return rtcEngine->startLastmileProbeTest(config);
}

int ArtcCli::stopLastmileProbeTest()
{
	return rtcEngine->stopLastmileProbeTest();
}

int ArtcCli::setVideoProfile(VideoProfile profile, bool swapWidthAndHeight)
{
	return rtcEngine->setVideoProfile(static_cast<ar::rtc::VIDEO_PROFILE_TYPE>(profile), swapWidthAndHeight);
}

int ArtcCli::setupLocalVideo(IntPtr view, int renderMode, String^ uid)
{
	return rtcEngine->setupLocalVideo(ar::rtc::VideoCanvas(view.ToPointer(), renderMode, MarshalString(uid).c_str()));
}

int ArtcCli::setupLocalVideo(IntPtr view, int renderMode, String^ uid, EnumVideoMirrorModeType mt)
{
	return rtcEngine->setupLocalVideo(ar::rtc::VideoCanvas(view.ToPointer(), renderMode, MarshalString(uid).c_str(), static_cast<VIDEO_MIRROR_MODE_TYPE>(mt)));
}

int ArtcCli::setupRemoteVideo(IntPtr view, int renderMode, String^ uid)
{
	return rtcEngine->setupRemoteVideo(ar::rtc::VideoCanvas(view.ToPointer(), renderMode, MarshalString(uid).c_str()));
}

int ArtcCli::setupRemoteVideo(IntPtr view, int renderMode, String^ uid, EnumVideoMirrorModeType mt)
{
	return rtcEngine->setupRemoteVideo(ar::rtc::VideoCanvas(view.ToPointer(), renderMode, MarshalString(uid).c_str(), static_cast<VIDEO_MIRROR_MODE_TYPE>(mt)));
}

int ArtcCli::enableDualStreamMode(bool enabled)
{
	return rtcEngine->enableDualStreamMode(enabled);
}

int ArtcCli::setRemoteVideoStreamType(String^ uid, EnumRemoteVideoStreamType type)
{
	return rtcEngine->setRemoteVideoStreamType(MarshalString(uid).c_str(), static_cast<REMOTE_VIDEO_STREAM_TYPE>(type));
}

int ArtcCli::setVideoQualityParameters(bool preferFrameRateOverImageQuality)
{
	return rtcEngine->setVideoQualityParameters(preferFrameRateOverImageQuality);
}

int ArtcCli::setChannelProfile(ChannelProfile profile)
{
	return rtcEngine->setChannelProfile(static_cast<ar::rtc::CHANNEL_PROFILE_TYPE>(profile));
}

int ArtcCli::setClientRole(ClientRoleType role)
{
	return rtcEngine->setClientRole(static_cast<CLIENT_ROLE_TYPE>(role));
}

int ArtcCliLibrary::ArtcCli::setClientRole(ClientRoleType role, ArtcCliClientRoleOptions options)
{
	ClientRoleOptions rawOptions = options;
	return rtcEngine->setClientRole(static_cast<CLIENT_ROLE_TYPE>(role), rawOptions);
}

int ArtcCli::createDataStream(int% id, bool reliable, bool ordered)
{
	int streamId;
	int result = rtcEngine->createDataStream(&streamId, reliable, ordered);
	id = streamId;
	return result;
}

int ArtcCliLibrary::ArtcCli::createDataStream(int% id, CliDataStreamConfig config)
{
	int streamId;
	DataStreamConfig rawConfig = config;
	int result = rtcEngine->createDataStream(&streamId, rawConfig.syncWithAudio, rawConfig.ordered);
	id = streamId;
	return result;
}

int ArtcCli::sendStreamMessage(int id, String^ data)
{
	std::string dataStr = MarshalString(data);
	return rtcEngine->sendStreamMessage(id, dataStr.c_str(), dataStr.length());
}

int ArtcCli::setRecordingAudioFrameParameters(int sampleRate, int channel, RawAudioFrameOPModeType mode, int samplesPerCall)
{
	return rtcEngine->setRecordingAudioFrameParameters(sampleRate, channel, static_cast<RAW_AUDIO_FRAME_OP_MODE_TYPE>(mode), samplesPerCall);
}

int ArtcCli::setPlaybackAudioFrameParameters(int sampleRate, int channel, RawAudioFrameOPModeType mode, int samplesPerCall)
{
	return rtcEngine->setPlaybackAudioFrameParameters(sampleRate, channel, static_cast<RAW_AUDIO_FRAME_OP_MODE_TYPE>(mode), samplesPerCall);
}

int ArtcCli::setMixedAudioFrameParameters(int sampleRate, int samplesPerCall)
{
	return rtcEngine->setMixedAudioFrameParameters(sampleRate, samplesPerCall);
}

int ArtcCli::muteLocalAudioStream(bool mute)
{
	return rtcEngine->muteLocalAudioStream(mute);
}

int ArtcCli::muteAllRemoteAudioStreams(bool mute)
{
	return rtcEngine->muteAllRemoteAudioStreams(mute);
}

int ArtcCli::muteRemoteAudioStream(String^ uid, bool mute)
{
	return rtcEngine->muteRemoteAudioStream(MarshalString(uid).c_str(), mute);
}

int ArtcCli::muteLocalVideoStream(bool mute)
{
	return rtcEngine->muteLocalVideoStream(mute);
}

int ArtcCli::enableLocalVideo(bool enabled)
{
	return rtcEngine->enableLocalVideo(enabled);
}

int ArtcCli::muteAllRemoteVideoStreams(bool mute)
{
	return rtcEngine->muteAllRemoteVideoStreams(mute);
}

int ArtcCli::muteRemoteVideoStream(String^ uid, bool mute)
{
	return rtcEngine->muteRemoteVideoStream(MarshalString(uid).c_str(), mute);
}

int ArtcCli::setLocalRenderMode(EnumRenderModeType mode)
{
	return rtcEngine->setLocalRenderMode(static_cast<ar::rtc::RENDER_MODE_TYPE>(mode));
}

int ArtcCli::setLocalRenderMode(EnumRenderModeType mode, EnumVideoMirrorModeType mt)
{
	return rtcEngine->setLocalRenderMode(static_cast<ar::rtc::RENDER_MODE_TYPE>(mode),
		static_cast<VIDEO_MIRROR_MODE_TYPE>(mt));
}

int ArtcCli::setRemoteRenderMode(String^ uid, EnumRenderModeType mode)
{
	return rtcEngine->setRemoteRenderMode(MarshalString(uid).c_str(), static_cast<ar::rtc::RENDER_MODE_TYPE>(mode));
}

int ArtcCli::setRemoteRenderMode(String^ uid, EnumRenderModeType mode, EnumVideoMirrorModeType mt)
{
	return rtcEngine->setRemoteRenderMode(
		MarshalString(uid).c_str(), static_cast<ar::rtc::RENDER_MODE_TYPE>(mode),
		static_cast<VIDEO_MIRROR_MODE_TYPE>(mt)
	);
}

int ArtcCli::enableAudioVolumeIndication(int interval, int smooth, bool report_vad)
{
	return rtcEngine->enableAudioVolumeIndication(interval, smooth, report_vad);
}

int ArtcCli::startAudioRecording(String^ path, int sampleRate, AudioRecordingQualityType quality)
{
	return rtcEngine->startAudioRecording(MarshalString(path).c_str(), sampleRate, static_cast<AUDIO_RECORDING_QUALITY_TYPE>(quality));
}

int ArtcCli::stopAudioRecording()
{
	return rtcEngine->stopAudioRecording();
}

int ArtcCli::pauseAudioMixing()
{
	return rtcEngine->pauseAudioMixing();
}

int ArtcCli::resumeAudioMixing()
{
	return rtcEngine->resumeAudioMixing();
}

int ArtcCli::adjustAudioMixingVolume(int volume)
{
	return rtcEngine->adjustAudioMixingVolume(volume);
}

int ArtcCli::getAudioMixingDuration()
{
	return rtcEngine->getAudioMixingDuration();
}

int ArtcCli::getAudioMixingCurrentPosition()
{
	return rtcEngine->getAudioMixingCurrentPosition();
}

int ArtcCli::setAudioMixingPosition(int pos)
{
	return rtcEngine->setAudioMixingPosition(pos);
}

int ArtcCli::startAudioMixing(String^ path, bool loop, bool replace, int cycle)
{
	return rtcEngine->startAudioMixing(MarshalString(path).c_str(), loop, replace, cycle);
}

int ArtcCli::stopAudioMixing()
{
	return rtcEngine->stopAudioMixing();
}

int ArtcCli::setAudioProfile(AudioProfileType profile, AudioScenarioType scenario)
{
	return rtcEngine->setAudioProfile(static_cast<ar::rtc::AUDIO_PROFILE_TYPE>(profile), static_cast<ar::rtc::AUDIO_SCENARIO_TYPE>(scenario));
}

int ArtcCli::setLogFile(String^ path)
{
	return rtcEngine->setLogFile(MarshalString(path).c_str());
}

int ArtcCli::setLogFilter(unsigned int filter)
{
	return rtcEngine->setLogFilter(filter);
}

int ArtcCli::setLogFileSize(unsigned int size)
{
	return rtcEngine->setLogFileSize(size);
}

int ArtcCli::adjustRecordingSignalVolume(int volume)
{
	return rtcEngine->adjustRecordingSignalVolume(volume);
}

int ArtcCli::adjustUserPlaybackSignalVolume(String^ uid, int volume)
{
	return rtcEngine->adjustUserPlaybackSignalVolume(MarshalString(uid).c_str(), volume);
}

int ArtcCli::adjustPlaybackSignalVolume(int volume)
{
	return rtcEngine->adjustPlaybackSignalVolume(volume);
}

int ArtcCli::enableLocalAudio(bool enabled)
{
	return rtcEngine->enableLocalAudio(enabled);
}

int ArtcCli::setLocalVoicePitch(double pitch)
{
	return rtcEngine->setLocalVoicePitch(pitch);
}

int ArtcCli::setExternalAudioSource(bool enabled, int sampleRate, int channels)
{
	return rtcEngine->setExternalAudioSource(enabled, sampleRate, channels);
}

int ArtcCli::pushAudioFrame(CliAudioFrame^ frame)
{
	return artcMediaEngine ? artcMediaEngine->pushAudioFrame(frame) : -1;
}

int ArtcCli::setLocalVoiceEqualization(AudioEqualizationBandFrequency freq, int bandGain)
{
	return rtcEngine->setLocalVoiceEqualization(static_cast<AUDIO_EQUALIZATION_BAND_FREQUENCY>(freq), bandGain);
}

int ArtcCli::setLocalVoiceReverb(AudioReverbType type, int value)
{
	return rtcEngine->setLocalVoiceReverb(static_cast<AUDIO_REVERB_TYPE>(type), value);
}

int ArtcCliLibrary::ArtcCli::setVoiceBeautifierPreset(EnumVoiceBeautifierPreset preset)
{
	return rtcEngine->setVoiceBeautifierPreset(static_cast<VOICE_BEAUTIFIER_PRESET>(preset));
}

int ArtcCliLibrary::ArtcCli::setVoiceBeautifierParameters(EnumVoiceBeautifierPreset preset, int param1, int param2)
{
	//return rtcEngine->setVoiceBeautifierParameters(static_cast<VOICE_BEAUTIFIER_PRESET>(preset), param1, param2);
	return 0;
}

int ArtcCliLibrary::ArtcCli::setAudioEffectPreset(EnumAudioEffectPreset preset)
{
	return rtcEngine->setAudioEffectPreset(static_cast<AUDIO_EFFECT_PRESET>(preset));
}

int ArtcCliLibrary::ArtcCli::setAudioEffectParameters(EnumAudioEffectPreset preset, int param1, int param2)
{
	return rtcEngine->setAudioEffectParameters(static_cast<AUDIO_EFFECT_PRESET>(preset), param1, param2);
}

int ArtcCliLibrary::ArtcCli::setVoiceConversionPreset(EnumVoiceConversionPreset preset)
{
	//return rtcEngine->setVoiceConversionPreset(static_cast<VOICE_CONVERSION_PRESET>(preset));
	return 0;
}

int ArtcCli::setLocalVideoMirrorMode(EnumVideoMirrorModeType mode)
{
	return rtcEngine->setLocalVideoMirrorMode(static_cast<VIDEO_MIRROR_MODE_TYPE>(mode));
}

int ArtcCli::setCameraCapturerConfiguration(CliCameraCaptureConfiguration^ config)
{
	return rtcEngine->setCameraCapturerConfiguration(config);
}

String^ ArtcCli::getVersion(int% build)
{
	int version;
	String^ result = gcnew String(rtcEngine->getVersion(&version));
	build = version;
	return result;
}

int ArtcCli::enableLoopbackRecording(bool enabled, String^ name)
{
	return rtcEngine->enableLoopbackRecording(enabled, MarshalString(name).c_str());
}

int ArtcCli::addPublishStreamUrl(String^ url, bool transcodingEnabled)
{
	return rtcEngine->addPublishStreamUrl(MarshalString(url).c_str(), transcodingEnabled);
}

int ArtcCli::removePublishStreamUrl(String^ url)
{
	return rtcEngine->removePublishStreamUrl(MarshalString(url).c_str());
}

int ArtcCli::setLiveTranscoding(CliLiveTranscoding^ transcoding)
{
	return rtcEngine->setLiveTranscoding(transcoding);
}

int ArtcCli::addInjectStreamUrl(String^ url, CliInjectStreamConfig^ config)
{
	return rtcEngine->addInjectStreamUrl(MarshalString(url).c_str(), config);
}

int ArtcCli::removeInjectStreamUrl(String^ url)
{
	return rtcEngine->removeInjectStreamUrl(MarshalString(url).c_str());
}

int ArtcCli::getEffectsVolume()
{
	return rtcEngine->getEffectsVolume();
}

int ArtcCli::setEffectsVolume(int volume)
{
	return rtcEngine->setEffectsVolume(volume);
}

int ArtcCli::setVolumeOfEffect(int soundId, int volume)
{
	return rtcEngine->setVolumeOfEffect(soundId, volume);
}

int ArtcCli::playEffect(int soundId, String^ file, int loopCount, double pitch, double pan, int gain, bool publish)
{
	return rtcEngine->playEffect(soundId, MarshalString(file).c_str(), loopCount, pitch, pan, gain, publish);
}

int ArtcCli::stopEffect(int soundId)
{
	return rtcEngine->stopEffect(soundId);
}

int ArtcCli::stopAllEffects()
{
	return rtcEngine->stopAllEffects();
}

int ArtcCli::preloadEffect(int soundId, String^ file)
{
	return rtcEngine->preloadEffect(soundId, MarshalString(file).c_str());
}

int ArtcCli::unloadEffect(int soundId)
{
	return rtcEngine->unloadEffect(soundId);
}

int ArtcCli::pauseEffect(int soundId)
{
	return rtcEngine->pauseEffect(soundId);
}

int ArtcCli::pauseAllEffects()
{
	return rtcEngine->pauseAllEffects();
}

int ArtcCli::resumeEffect(int soundId)
{
	return rtcEngine->resumeEffect(soundId);
}

int ArtcCli::resumeAllEffects()
{
	return rtcEngine->resumeAllEffects();
}

int ArtcCli::setAudioMixingPitch(int pitch)
{
	return rtcEngine->setAudioMixingPitch(pitch);
}

EnumConnectionStateType ArtcCli::getConnectionState()
{
	return static_cast<EnumConnectionStateType>(rtcEngine->getConnectionState());
}

int ArtcCli::setDefaultMuteAllRemoteAudioStreams(bool mute)
{
	return rtcEngine->setDefaultMuteAllRemoteAudioStreams(mute);
}

int ArtcCli::setVideoEncoderConfiguration(CliVideoEncoderConfiguration^ config)
{
	return rtcEngine->setVideoEncoderConfiguration(config);
}

int ArtcCli::setDefaultMuteAllRemoteVideoStreams(bool mute)
{
	return rtcEngine->setDefaultMuteAllRemoteVideoStreams(mute);
}

int ArtcCli::adjustAudioMixingPlayoutVolume(int volume)
{
	return rtcEngine->adjustAudioMixingPlayoutVolume(volume);
}

int ArtcCli::adjustAudioMixingPublishVolume(int volume)
{
	return rtcEngine->adjustAudioMixingPublishVolume(volume);
}

int ArtcCli::getAudioMixingPlayoutVolume()
{
	return rtcEngine->getAudioMixingPlayoutVolume();
}

int ArtcCli::getAudioMixingPublishVolume()
{
	return rtcEngine->getAudioMixingPublishVolume();
}

int ArtcCli::setExternalAudioSink(bool enabled, int sampleRate, int channels)
{
	return rtcEngine->setExternalAudioSink(enabled, sampleRate, channels);
}

int ArtcCli::pullAudioFrame([Out] CliAudioFrame^% frame)
{
	IAudioFrameObserver::AudioFrame* raw = new IAudioFrameObserver::AudioFrame();
	auto result = artcMediaEngine->pullAudioFrame(raw);
	if (result == 0) {
		frame = gcnew CliAudioFrame(*raw);
	}
	return result;
	//return artcMediaEngine ? artcMediaEngine->pullAudioFrame(frame) : -1;
}

bool ArtcCliLibrary::ArtcCli::setVideoSource(CliVideoSource^ source)
{
	auto result = rtcEngine->setVideoSource(NULL);
	if (innerVideoSoruce != nullptr) innerVideoSoruce->dispose();
	
	if (source != nullptr) {
		innerVideoSoruce = gcnew InnerVideoSource(source);
		result = rtcEngine->setVideoSource(innerVideoSoruce);
	}

	return result;
}

int ArtcCli::setExternalVideoSource(bool enabled, bool useTexture)
{
	return artcMediaEngine ? artcMediaEngine->setExternalVideoSource(enabled, useTexture) : -1;
}

int ArtcCli::pushVideoFrame(CliExternalVideoFrame^ frame)
{
	return artcMediaEngine ? artcMediaEngine->pushVideoFrame(frame) : -1;
}

int ArtcCli::setLocalPublishFallbackOption(StreamFallbackOptions option)
{
	return rtcEngine->setLocalPublishFallbackOption(static_cast<STREAM_FALLBACK_OPTIONS>(option));
}

int ArtcCli::setRemoteSubscribeFallbackOption(StreamFallbackOptions option)
{
	return rtcEngine->setRemoteSubscribeFallbackOption(static_cast<STREAM_FALLBACK_OPTIONS>(option));
}

int ArtcCli::setRemoteUserPriority(uid_t uid, PriorityType priority)
{
	return rtcEngine->setRemoteUserPriority(uid, static_cast<PRIORITY_TYPE>(priority));
}

int ArtcCli::setRemoteDefaultVideoStreamType(EnumRemoteVideoStreamType type)
{
	return rtcEngine->setRemoteDefaultVideoStreamType(static_cast<REMOTE_VIDEO_STREAM_TYPE>(type));
}

String^ ArtcCli::getErrorDescription(int code)
{
	return gcnew String(rtcEngine->getErrorDescription(code));
}

#if 0
void* ArtcCli::regEvent(Object^ obj)
{
	gchs->Add(GCHandle::Alloc(obj));
	return Marshal::GetFunctionPointerForDelegate(obj).ToPointer();
}
#endif

void ArtcCli::initializeEventHandler()
{
	artcEventHandler->onJoinChannelSuccessEvent = PFOnJoinChannelSuccess(regEvent(gcnew NativeOnJoinChannelSuccessDelegate(this, &ArtcCli::NativeOnJoinChannelSuccess)));
	artcEventHandler->onRejoinChannelSuccessEvent = PFOnRejoinChannelSuccess(regEvent(gcnew NativeOnRejoinChannelSuccessDelegate(this, &ArtcCli::NativeOnRejoinChannelSuccess)));
	artcEventHandler->onWarningEvent = PFOnWarning(regEvent(gcnew NativeOnWarningDelegate(this, &ArtcCli::NativeOnWarning)));
	artcEventHandler->onErrorEvent = PFOnError(regEvent(gcnew NativeOnErrorDelegate(this, &ArtcCli::NativeOnError)));
	artcEventHandler->onAudioQualityEvent = PFOnAudioQuality(regEvent(gcnew NativeOnAudioQualityDelegate(this, &ArtcCli::NativeOnAudioQuality)));
	artcEventHandler->onAudioVolumeIndicationEvent = PFOnAudioVolumeIndication(regEvent(gcnew NativeOnAudioVolumeIndicationDelegate(this, &ArtcCli::NativeOnAudioVolumeIndication)));
	artcEventHandler->onLeaveChannelEvent = PFOnLeaveChannel(regEvent(gcnew NativeOnLeaveChannelDelegate(this, &ArtcCli::NativeOnLeaveChannel)));
	artcEventHandler->onRtcStatsEvent = PFOnRtcStats(regEvent(gcnew NativeOnRtcStatsDelegate(this, &ArtcCli::NativeOnRtcStats)));
	artcEventHandler->onAudioDeviceStateChangedEvent = PFOnAudioDeviceStateChanged(regEvent(gcnew NativeOnAudioDeviceStateChangedDelegate(this, &ArtcCli::NativeOnAudioDeviceStateChanged)));
	artcEventHandler->onVideoDeviceStateChangedEvent = PFOnVideoDeviceStateChanged(regEvent(gcnew NativeOnVideoDeviceStateChangedDelegate(this, &ArtcCli::NativeOnVideoDeviceStateChanged)));
	artcEventHandler->onLastmileQualityEvent = PFOnLastmileQuality(regEvent(gcnew NativeOnLastmileQualityDelegate(this, &ArtcCli::NativeOnLastmileQuality)));
	artcEventHandler->onNetworkQualityEvent = PFOnNetworkQuality(regEvent(gcnew NativeOnNetworkQualityDelegate(this, &ArtcCli::NativeOnNetworkQuality)));
	artcEventHandler->onFirstLocalVideoFrameEvent = PFOnFirstLocalVideoFrame(regEvent(gcnew NativeOnFirstLocalVideoFrameDelegate(this, &ArtcCli::NativeOnFirstLocalVideoFrame)));
	artcEventHandler->onFirstRemoteVideoDecodedEvent = PFOnFirstRemoteVideoDecoded(regEvent(gcnew NativeOnFirstRemoteVideoDecodedDelegate(this, &ArtcCli::NativeOnFirstRemoteVideoDecoded)));
	artcEventHandler->onFirstRemoteVideoFrameEvent = PFOnFirstRemoteVideoFrame(regEvent(gcnew NativeOnFirstRemoteVideoFrameDelegate(this, &ArtcCli::NativeOnFirstRemoteVideoFrame)));
	artcEventHandler->onUserJoinedEvent = PFOnUserJoined(regEvent(gcnew NativeOnUserJoinedDelegate(this, &ArtcCli::NativeOnUserJoined)));
	artcEventHandler->onUserOfflineEvent = PFOnUserOffline(regEvent(gcnew NativeOnUserOfflineDelegate(this, &ArtcCli::NativeOnUserOffline)));
	artcEventHandler->onUserMuteAudioEvent = PFOnUserMuteAudio(regEvent(gcnew NativeOnUserMuteAudioDelegate(this, &ArtcCli::NativeOnUserMuteAudio)));
	artcEventHandler->onUserMuteVideoEvent = PFOnUserMuteVideo(regEvent(gcnew NativeOnUserMuteVideoDelegate(this, &ArtcCli::NativeOnUserMuteVideo)));
	artcEventHandler->onUserEnableVideoEvent = PFOnUserEnableVideo(regEvent(gcnew NativeOnUserEnableVideoDelegate(this, &ArtcCli::NativeOnUserEnableVideo)));
	artcEventHandler->onApiCallExecutedEvent = PFOnApiCallExecuted(regEvent(gcnew NativeOnApiCallExecutedDelegate(this, &ArtcCli::NativeOnApiCallExecuted)));
	artcEventHandler->onLocalVideoStatsEvent = PFOnLocalVideoStats(regEvent(gcnew NativeOnLocalVideoStatsDelegate(this, &ArtcCli::NativeOnLocalVideoStats)));
	artcEventHandler->onRemoteVideoStatsEvent = PFOnRemoteVideoStats(regEvent(gcnew NativeOnRemoteVideoStatsDelegate(this, &ArtcCli::NativeOnRemoteVideoStats)));
	artcEventHandler->onCameraReadyEvent = PFOnCameraReady(regEvent(gcnew NativeOnCameraReadyDelegate(this, &ArtcCli::NativeOnCameraReady)));
	artcEventHandler->onVideoStoppedEvent = PFOnVideoStopped(regEvent(gcnew NativeOnVideoStoppedDelegate(this, &ArtcCli::NativeOnVideoStopped)));
	artcEventHandler->onConnectionLostEvent = PFOnConnectionLost(regEvent(gcnew NativeOnConnectionLostDelegate(this, &ArtcCli::NativeOnConnectionLost)));
	artcEventHandler->onConnectionBannedEvent = PFOnConnectionBanned(regEvent(gcnew NativeOnConnectionBannedDelegate(this, &ArtcCli::NativeOnConnectionBanned)));

	artcEventHandler->onConnectionInterruptedEvent = PFOnConnectionInterrupted(regEvent(gcnew NativeOnConnectionInterruptedDelegate(this, &ArtcCli::NativeOnConnectionInterrupted)));
	artcEventHandler->onStreamMessageEvent = PFOnStreamMessage(regEvent(gcnew NativeOnStreamMessageDelegate(this, &ArtcCli::NativeOnStreamMessage)));
	artcEventHandler->onStreamMessageErrorEvent = PFOnStreamMessageError(regEvent(gcnew NativeOnStreamMessageErrorDelegate(this, &ArtcCli::NativeOnStreamMessageError)));
	artcEventHandler->onRequestChannelKeyEvent = PFOnRequestChannelKey(regEvent(gcnew NativeOnRequestChannelKeyDelegate(this, &ArtcCli::NativeOnRequestChannelKey)));
	artcEventHandler->onRequestTokenEvent = PFOnRequestToken(regEvent(gcnew NativeOnRequestTokenDelegate(this, &ArtcCli::NativeOnRequestToken)));

	artcEventHandler->onAudioMixingFinishedEvent = PFOnAudioMixingFinished(regEvent(gcnew NativeOnAudioMixingFinishedDelegate(this, &ArtcCli::NativeOnAudioMixingFinished)));
	artcEventHandler->onActiveSpeakerEvent = PFOnActiveSpeaker(regEvent(gcnew NativeOnActiveSpeakerDelegate(this, &ArtcCli::NativeOnActiveSpeaker)));
	artcEventHandler->onClientRoleChangedEvent = PFOnClientRoleChanged(regEvent(gcnew NativeOnClientRoleChangedDelegate(this, &ArtcCli::NativeOnClientRoleChanged)));
	artcEventHandler->onAudioDeviceVolumeChangedEvent = PFOnAudioDeviceVolumeChanged(regEvent(gcnew NativeOnAudioDeviceVolumeChangedDelegate(this, &ArtcCli::NativeOnAudioDeviceVolumeChanged)));

	artcEventHandler->onStreamUnpublishedEvent = PFOnStreamUnpublished(regEvent(gcnew NativeOnStreamUnpublishedDelegate(this, &ArtcCli::NativeOnStreamUnpublished)));
	artcEventHandler->onStreamPublishedEvent = PFOnStreamPublished(regEvent(gcnew NativeOnStreamPublishedDelegate(this, &ArtcCli::NativeOnStreamPublished)));
	artcEventHandler->onTranscodingUpdatedEvent = PFOnTranscodingUpdated(regEvent(gcnew NativeOnTranscodingUpdatedDelegate(this, &ArtcCli::NativeOnTranscodingUpdated)));

	artcEventHandler->onConnectionStateChangedEvent = PFOnConnectionStateChanged(regEvent(gcnew NativeOnConnectionStateChangedDelegate(this, &ArtcCli::NativeOnConnectionStateChanged)));
	artcEventHandler->onTokenPrivilegeWillExpireEvent = PFOnTokenPrivilegeWillExpire(regEvent(gcnew NativeOnTokenPrivilegeWillExpireDelegate(this, &ArtcCli::NativeOnTokenPrivilegeWillExpire)));
	artcEventHandler->onFirstLocalAudioFrameEvent = PFOnFirstLocalAudioFrame(regEvent(gcnew NativeOnFirstLocalAudioFrameDelegate(this, &ArtcCli::NativeOnFirstLocalAudioFrame)));
	artcEventHandler->onFirstRemoteAudioFrameEvent = PFOnFirstRemoteAudioFrame(regEvent(gcnew NativeOnFirstRemoteAudioFrameDelegate(this, &ArtcCli::NativeOnFirstRemoteAudioFrame)));
	artcEventHandler->onUserEnableLocalVideoEvent = PFOnUserEnableLocalVideo(regEvent(gcnew NativeOnUserEnableLocalVideoDelegate(this, &ArtcCli::NativeOnUserEnableLocalVideo)));
	artcEventHandler->onVideoSizeChangedEvent = PFOnVideoSizeChanged(regEvent(gcnew NativeOnVideoSizeChangedDelegate(this, &ArtcCli::NativeOnVideoSizeChanged)));
	artcEventHandler->onRemoteVideoStateChangedEvent = PFOnRemoteVideoStateChanged(regEvent(gcnew NativeOnRemoteVideoStateChangedDelegate(this, &ArtcCli::NativeOnRemoteVideoStateChanged)));
	artcEventHandler->onLocalPublishFallbackToAudioOnlyEvent = PFOnLocalPublishFallbackToAudioOnly(regEvent(gcnew NativeOnLocalPublishFallbackToAudioOnlyDelegate(this, &ArtcCli::NativeOnLocalPublishFallbackToAudioOnly)));
	artcEventHandler->onRemoteSubscribeFallbackToAudioOnlyEvent = PFOnRemoteSubscribeFallbackToAudioOnly(regEvent(gcnew NativeOnRemoteSubscribeFallbackToAudioOnlyDelegate(this, &ArtcCli::NativeOnRemoteSubscribeFallbackToAudioOnly)));
	artcEventHandler->onRemoteAudioStatsEvent = PFOnRemoteAudioStats(regEvent(gcnew NativeOnRemoteAudioStatsDelegate(this, &ArtcCli::NativeOnRemoteAudioStats)));
	artcEventHandler->onRemoteAudioTransportStatsEvent = PFOnRemoteAudioTransportStats(regEvent(gcnew NativeOnRemoteAudioTransportStatsDelegate(this, &ArtcCli::NativeOnRemoteAudioTransportStats)));
	artcEventHandler->onRemoteVideoTransportStatsEvent = PFOnRemoteVideoTransportStats(regEvent(gcnew NativeOnRemoteVideoTransportStatsDelegate(this, &ArtcCli::NativeOnRemoteVideoTransportStats)));
	artcEventHandler->onAudioMixingBeginEvent = PFOnAudioMixingBegin(regEvent(gcnew NativeOnAudioMixingBeginDelegate(this, &ArtcCli::NativeOnAudioMixingBegin)));
	artcEventHandler->onAudioMixingEndEvent = PFOnAudioMixingEnd(regEvent(gcnew NativeOnAudioMixingEndDelegate(this, &ArtcCli::NativeOnAudioMixingEnd)));
	artcEventHandler->onAudioEffectFinishedEvent = PFOnAudioEffectFinished(regEvent(gcnew NativeOnAudioEffectFinishedDelegate(this, &ArtcCli::NativeOnAudioEffectFinished)));
	artcEventHandler->onStreamInjectedStatusEvent = PFOnStreamInjectedStatus(regEvent(gcnew NativeOnStreamInjectedStatusDelegate(this, &ArtcCli::NativeOnStreamInjectedStatus)));
	artcEventHandler->onMediaEngineLoadSuccessEvent = PFOnMediaEngineLoadSuccess(regEvent(gcnew NativeOnMediaEngineLoadSuccessDelegate(this, &ArtcCli::NativeOnMediaEngineLoadSuccess)));
	artcEventHandler->onMediaEngineStartCallSuccessEvent = PFOnMediaEngineStartCallSuccess(regEvent(gcnew NativeOnMediaEngineStartCallSuccessDelegate(this, &ArtcCli::NativeOnMediaEngineStartCallSuccess)));
	artcEventHandler->onNetworkTypeChangedEvent = PFOnNetworkTypeChanged(regEvent(gcnew NativeOnNetworkTypeChangedDelegate(this, &ArtcCli::NativeOnNetworkTypeChanged)));
	artcEventHandler->onLocalAudioStateChangedEvent = PFOnLocalAudioStateChanged(regEvent(gcnew NativeOnLocalAudioStateChangedDelegate(this, &ArtcCli::NativeOnLocalAudioStateChanged)));
	artcEventHandler->onLocalVideoStateChangedEvent = PFOnLocalVideoStateChanged(regEvent(gcnew NativeOnLocalVideoStateChangedDelegate(this, &ArtcCli::NativeOnLocalVideoStateChanged)));
	artcEventHandler->onRemoteAudioStateChangedEvent = PFOnRemoteAudioStateChanged(regEvent(gcnew NativeOnRemoteAudioStateChangedDelegate(this, &ArtcCli::NativeOnRemoteAudioStateChanged)));
	artcEventHandler->onFirstRemoteAudioDecodedEvent = PFOnFirstRemoteAudioDecoded(regEvent(gcnew NativeOnFirstRemoteAudioDecodedDelegate(this, &ArtcCli::NativeOnFirstRemoteAudioDecoded)));
	artcEventHandler->onLocalAudioStatsEvent = PFOnLocalAudioStats(regEvent(gcnew NativeOnLocalAudioStatsDelegate(this, &ArtcCli::NativeOnLocalAudioStats)));
	artcEventHandler->onAudioMixingStateChangedEvent = PFOnAudioMixingStateChanged(regEvent(gcnew NativeOnAudioMixingStateChangedDelegate(this, &ArtcCli::NativeOnAudioMixingStateChanged)));
	artcEventHandler->onRemoteAudioMixingBeginEvent = PFOnRemoteAudioMixingBegin(regEvent(gcnew Action(this, &ArtcCli::NativeOnRemoteAudioMixingBegin)));

	artcEventHandler->onRemoteAudioMixingEndEvent = static_cast<stdCall>(regEvent(gcnew Action(this, &ArtcCli::NativeOnRemoteAudioMixingEnd)));
	artcEventHandler->onRtmpStreamingStateChangedEvent = static_cast<PFOnRtmpStreamingStateChanged>(regEvent(gcnew NativeOnRtmpStreamingStateChangedDelegate(this, &ArtcCli::NativeOnRtmpStreamingStateChanged)));
	artcEventHandler->onChannelMediaRelayStateChangedEvent = static_cast<PFOnChannelMediaRelayStateChanged>(regEvent(gcnew NativeOnChannelMediaRelayStateChangedDelegate(this, &ArtcCli::NativeOnChannelMediaRelayStateChanged)));
	artcEventHandler->onChannelMediaRelayEventEvnet = static_cast<PFOnChannelMediaRelayEvent>(regEvent(gcnew NativeOnChannelMediaRelayEventDelegate(this, &ArtcCli::NativeOnChannelMediaRelayEvent)));
	artcEventHandler->onLastmileProbeResultEvent = static_cast<PFOnLastmileProbeResult>(regEvent(gcnew NativeOnLastmileProbeResultDelegate(this, &ArtcCli::NativeOnLastmileProbeResult)));

	artcEventHandler->onAudioPublishStateChangedEvent = static_cast<OnAudioPublishStateChanged::Pointer>(regEvent(gcnew OnAudioPublishStateChanged::Type(this, &ArtcCli::NativeOnAudioPublishStateChanged)));
	artcEventHandler->onVideoPublishStateChangedEvent = static_cast<OnVideoPublishStateChanged::Pointer>(regEvent(gcnew OnVideoPublishStateChanged::Type(this, &ArtcCli::NativeOnVideoPublishStateChanged)));
	artcEventHandler->onAudioSubscribeStateChangedEvent = static_cast<OnAudioSubscribeStateChanged::Pointer>(regEvent(gcnew OnAudioSubscribeStateChanged::Type(this, &ArtcCli::NativeOnAudioSubscribeStateChanged)));
	artcEventHandler->onVideoSubscribeStateChangedEvent = static_cast<OnVideoSubscribeStateChanged::Pointer>(regEvent(gcnew OnVideoSubscribeStateChanged::Type(this, &ArtcCli::NativeOnVideoSubscribeStateChanged)));
	artcEventHandler->onFirstLocalAudioFramePublishedEvent = static_cast<OnFirstLocalAudioFramePublished::Pointer>(regEvent(gcnew OnFirstLocalAudioFramePublished::Type(this, &ArtcCli::NativeOnFirstLocalAudioFramePublished)));
	artcEventHandler->onFirstLocalVideoFramePublishedEvent = static_cast<OnFirstLocalVideoFramePublished::Pointer>(regEvent(gcnew OnFirstLocalVideoFramePublished::Type(this, &ArtcCli::NativeOnFirstLocalVideoFramePublished)));

	artcEventHandler->onRtmpStreamingEventEvent = static_cast<OnRtmpStreamingEvent::Pointer>(regEvent(gcnew OnRtmpStreamingEvent::Type(this, &ArtcCli::NativeOnRtmpStreamingEvent)));
}

void ArtcCli::initializePacketObserver()
{
	artcPacketObserver->onSendAudioPacketEvent = PFOnSendAudioPacket(regEvent(gcnew NativeOnSendAudioPacketDelegate(this, &ArtcCli::NativeOnSendAudioPacket)));
	artcPacketObserver->onSendVideoPacketEvent = PFOnSendVideoPacket(regEvent(gcnew NativeOnSendVideoPacketDelegate(this, &ArtcCli::NativeOnSendVideoPacket)));
	artcPacketObserver->onReceiveVideoPacketEvent = PFOnReceiveVideoPacket(regEvent(gcnew NativeOnSendVideoPacketDelegate(this, &ArtcCli::NativeOnReceiveVideoPacket)));
	artcPacketObserver->onReceiveAudioPacketEvent = PFOnReceiveAudioPacket(regEvent(gcnew NativeOnReceiveAudioPacketDelegate(this, &ArtcCli::NativeOnReceiveAudioPacket)));
}

void ArtcCli::initializeRawFrameObserver()
{
	artcAudioObserver->onRecordAudioFrameEvent = PFOnRecordAudioFrame(regEvent(gcnew NativeOnRecordAudioFrameDelegate(this, &ArtcCli::NativeOnRecordAudioFrame)));
	artcAudioObserver->onPlaybackAudioFrameEvent = PFOnPlaybackAudioFrame(regEvent(gcnew NativeOnPlaybackAudioFrameDelegate(this, &ArtcCli::NativeOnPlaybackAudioFrame)));
	artcAudioObserver->onPlaybackAudioFrameBeforeMixingEvent = PFOnPlaybackAudioFrameBeforeMixing(regEvent(gcnew NativeOnPlaybackAudioFrameBeforeMixingDelegate(this, &ArtcCli::NativeOnPlaybackAudioFrameBeforeMixing)));
	artcAudioObserver->onMixedAudioFrameEvent = PFOnMixedAudioFrame(regEvent(gcnew NativeOnMixedAudioFrameDelegate(this, &ArtcCli::NativeOnMixedAudioFrame)));
	artcAudioObserver->isMultipleChannelFrameWantedEvent = static_cast<PFIsMultipleChannelFrameWanted>(regEvent(gcnew NativeIsMultipleChannelFrameWantedDelegate(this, &ArtcCli::NativeIsMultipleChannelAudioFrameWanted)));

	artcVideoObserver->onCaptureVideoFrameEvent = PFOnCaptureVideoFrame(regEvent(gcnew NativeOnCaptureVideoFrameDelegate(this, &ArtcCli::NativeOnCaptureVideoFrame)));
	artcVideoObserver->onPreEncodeVideoFrameEvent = PFOnPreEncodeVideoFrame(regEvent(gcnew NativeOnPreEncodeVideoFrameDelegate(this, &ArtcCli::NativeOnPreEncodeVideoFrame)));
	artcVideoObserver->getSmoothRenderingEnabledEvent = PFGetSmoothRenderingEnabled(regEvent(gcnew NativeGetSmoothRenderingEnabledDelegate(this, &ArtcCli::NativeOnGetSmoothRenderingEnabled)));
	artcVideoObserver->onRenderVideoFrameEvent = PFOnRenderVideoFrame(regEvent(gcnew NativeOnRenderVideoFrameDelegate(this, &ArtcCli::NativeOnRenderVideoFrame)));
	artcVideoObserver->onGetVideoFormatPreferenceEvent = static_cast<PFOnGetVideoFormatPreference>(regEvent(gcnew NativeOnGetVideoFormatePreferenceDelegate(this, &ArtcCli::NativeOnGetVideoFormatPreference)));
	artcVideoObserver->onGetRotationAppliedEvent = static_cast<PFOnGetRotationApplied>(regEvent(gcnew NativeOnGetRotationAppliedDelegate(this, &ArtcCli::NativeOnGetRotationApplied)));
	artcVideoObserver->onGetMirrorAppliedEvent = static_cast<PFOnGetMirrorApplied>(regEvent(gcnew NativeOnGetMirrorAppliedDelegate(this, &ArtcCli::NativeOnGetMirrorApplied)));
	artcVideoObserver->isMultipleChannelFrameWantedEvent = static_cast<PFIsMultipleChannelFrameWanted>(regEvent(gcnew NativeIsMultipleChannelFrameWantedDelegate(this, &ArtcCli::NativeIsMultipleChannelVideoFrameWanted)));
	artcVideoObserver->getObservedFramePositionEvent = static_cast<PFGetObservedFramePosition>(regEvent(gcnew NativeGetObservedFramePositionDelegate(this, &ArtcCli::NativeObservedVideoFramePosition)));
	artcVideoObserver->onRenderVideoFrameExEvent = PFOnRenderVideoFrameEx(regEvent(gcnew NativeOnRenderVideoFrameExDelegate(this, &ArtcCli::NativeOnRenderVideoFrameEx)));
}

void ArtcCli::initializeMetaObserver()
{
	artcMetadataObserver->onGetMaxMetadataSizeEvent = static_cast<PFOnGetMaxMetadataSize>(regEvent(gcnew NativeOnGetMaxMetadataSizeDelegate(this, &ArtcCli::NativeGetMaxMetadataSize)));
	artcMetadataObserver->onReadyToSendMetadataEvent = static_cast<PFOnReadyToSendMetadata>(regEvent(gcnew NativeOnReadyToSendMetadataDelegate(this, &ArtcCli::NativeOnReadyToSendMetadata)));
	artcMetadataObserver->onMetadataReceivedEvent = static_cast<PFOnMetadataReceived>(regEvent(gcnew NativeOnMetadataReceivedDelegate(this, &ArtcCli::NativeOnMetadataReceived)));
}

ArtcCliAudioDeviceManager^ ArtcCli::getAudioDeviceManager()
{
	return gcnew ArtcCliAudioDeviceManager(this->rtcEngine);
}

ArtcCliVideoDeviceManager^ ArtcCli::getVideoDeviceManager()
{
	return gcnew ArtcCliVideoDeviceManager(this->rtcEngine);
}

void ArtcCli::NativeOnJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
	if (onJoinChannelSuccess)
	{
		onJoinChannelSuccess(gcnew String(channel), gcnew String(uid), elapsed);
	}
}

void ArtcCli::NativeOnRejoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
	if (onRejoinChannelSuccess)
	{
		onRejoinChannelSuccess(gcnew String(channel), gcnew String(uid), elapsed);
	}
}

void ArtcCli::NativeOnWarning(int warn, const char* msg)
{
	if (onWarning)
	{
		onWarning(warn, gcnew String(msg));
	}
}

void ArtcCli::NativeOnError(int err, const char* msg)
{
	if (onError)
		onError(err, gcnew String(msg));
}

void ArtcCli::NativeOnAudioQuality(uid_t uid, int quality, unsigned short delay, unsigned short lost)
{
	if (onAudioQuality)
		onAudioQuality(gcnew String(uid), quality, delay, lost);
}

void ArtcCli::NativeOnAudioVolumeIndication(const ar::rtc::AudioVolumeInfo* speakers, unsigned int speakerNumber, int totalVolume)
{
	if (onAudioVolumeIndication)
	{
		List<CliAudioVolumeInfo^>^ list = gcnew List<CliAudioVolumeInfo^>();
		for (unsigned int i = 0; i < speakerNumber; i++)
			list->Add(gcnew CliAudioVolumeInfo(speakers[i]));

		onAudioVolumeIndication(list, totalVolume);
	}
}

void ArtcCli::NativeOnLeaveChannel(const ar::rtc::RtcStats& stats)
{
	if (onLeaveChannel)
		onLeaveChannel(gcnew CliRtcStats(stats));
}

void ArtcCli::NativeOnRtcStats(const ar::rtc::RtcStats& stats)
{
	if (onRtcStats)
	{
		CliRtcStats^ rtc = gcnew CliRtcStats(stats);
		onRtcStats(rtc);
	}
}

void ArtcCli::NativeOnAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState)
{
	if (onAudioDeviceStateChanged)
		onAudioDeviceStateChanged(gcnew String(deviceId), deviceType, deviceState);
}

void ArtcCli::NativeOnVideoDeviceStateChanged(const char* deviceId, int deviceType, int deviceState)
{
	if (onVideoDeviceStateChanged)
		onVideoDeviceStateChanged(gcnew String(deviceId), deviceType, deviceState);
}

void ArtcCli::NativeOnAudioMixingFinished()
{
	if (onAudioMixingFinished)
		onAudioMixingFinished();
}

void ArtcCli::NativeOnActiveSpeaker(uid_t uid)
{
	if (onActiveSpeaker)
		onActiveSpeaker(gcnew String(uid));
}

void ArtcCli::NativeOnClientRoleChanged(CLIENT_ROLE_TYPE oldRole, CLIENT_ROLE_TYPE newRole)
{
	if (onClientRoleChanged)
		onClientRoleChanged(static_cast<ClientRoleType>(oldRole), static_cast<ClientRoleType>(newRole));
}

void ArtcCli::NativeOnAudioDeviceVolumeChanged(MEDIA_DEVICE_TYPE deviceType, int volume, bool muted)
{
	if (onAudioDeviceVolumeChanged)
		onAudioDeviceVolumeChanged(static_cast<MediaDeviceType>(deviceType), volume, muted);
}

void ArtcCli::NativeOnStreamUnpublished(const char* url)
{
	if (onStreamUnpublished)
		onStreamUnpublished(gcnew String(url));
}

void ArtcCli::NativeOnStreamPublished(const char* url, int error)
{
	if (onStreamPublished)
		onStreamPublished(gcnew String(url), error);
}

void ArtcCli::NativeOnTranscodingUpdated()
{
	if (onTranscodingUpdated)
		onTranscodingUpdated();
}

void ArtcCli::NativeOnConnectionStateChanged(CONNECTION_STATE_TYPE state, CONNECTION_CHANGED_REASON_TYPE reason)
{
	if (onConnectionStateChanged)
		onConnectionStateChanged(static_cast<EnumConnectionStateType>(state), static_cast<EnumConnectionChangedReasonType>(reason));
}

void ArtcCli::NativeOnTokenPrivilegeWillExpire(const char* token)
{
	if (onTokenPrivilegeWillExpire)
		onTokenPrivilegeWillExpire(gcnew String(token));
}

void ArtcCli::NativeOnFirstLocalAudioFrame(int elapsed)
{
	if (onFirstLocalAudioFrame)
		onFirstLocalAudioFrame(elapsed);
}

void ArtcCli::NativeOnFirstRemoteAudioFrame(uid_t uid, int elapsed)
{
	if (onFirstRemoteAudioFrame)
		onFirstRemoteAudioFrame(gcnew String(uid), elapsed);
}

void ArtcCli::NativeOnUserEnableLocalVideo(uid_t uid, bool enabled)
{
	if (onUserEnableLocalVideo)
		onUserEnableLocalVideo(gcnew String(uid), enabled);
}

void ArtcCli::NativeOnVideoSizeChanged(uid_t uid, int width, int height, int rotation)
{
	if (onVideoSizeChanged)
		onVideoSizeChanged(gcnew String(uid), width, height, rotation);
}

void ArtcCli::NativeOnRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (onRemoteVideoStateChanged)
		onRemoteVideoStateChanged(gcnew String(uid), static_cast<EnumRemoteVideoState>(state), static_cast<EnumRemoteVideoStateReason>(reason), elapsed);
}

void ArtcCli::NativeOnLastmileQuality(int quality)
{
	if (onLastmileQuality)
		onLastmileQuality(quality);
}

void ArtcCli::NativeOnNetworkQuality(uid_t uid, int txQuality, int rxQuality)
{
	if (onNetworkQuality)
		onNetworkQuality(gcnew String(uid), txQuality, rxQuality);
}

void ArtcCli::NativeOnFirstLocalVideoFrame(int width, int height, int elapsed)
{
	if (onFirstLocalVideoFrame)
		onFirstLocalVideoFrame(width, height, elapsed);
}

void ArtcCli::NativeOnFirstRemoteVideoDecoded(uid_t uid, int width, int height, int elapsed)
{
	if (onFirstRemoteVideoDecoded)
		onFirstRemoteVideoDecoded(gcnew String(uid), width, height, elapsed);
}

void ArtcCli::NativeOnFirstRemoteVideoFrame(uid_t uid, int width, int height, int elapsed)
{
	if (onFirstRemoteVideoFrame)
		onFirstRemoteVideoFrame(gcnew String(uid), width, height, elapsed);
}

void ArtcCli::NativeOnUserJoined(uid_t uid, int elapsed)
{
	if (onUserJoined)
		onUserJoined(gcnew String(uid), elapsed);
}

void ArtcCli::NativeOnUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	if (onUserOffline)
		onUserOffline(gcnew String(uid), static_cast<UserOfflineType>(reason));
}

void ArtcCli::NativeOnNetworkTypeChanged(NETWORK_TYPE type)
{
	if (onNetworkTypeChanged)
		onNetworkTypeChanged(static_cast<NetworkType>(type));
}

void ArtcCli::NativeOnLocalAudioStateChanged(ar::rtc::LOCAL_AUDIO_STREAM_STATE state, ar::rtc::LOCAL_AUDIO_STREAM_ERROR error)
{
	if (onLocalAudioStateChanged)
		onLocalAudioStateChanged(static_cast<LocalAudioStreamState>(state), static_cast<LocalAudioStreamError>(error));
}

void ArtcCli::NativeOnLocalVideoStateChanged(LOCAL_VIDEO_STREAM_STATE state, LOCAL_VIDEO_STREAM_ERROR error)
{
	if (onLocalVideoStateChanged)
		onLocalVideoStateChanged(static_cast<LocalVideoStreamState>(state), static_cast<LocalVideoStreamError>(error));
}

void ArtcCli::NativeOnRemoteAudioStateChanged(uid_t uid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed)
{
	if (onRemoteAudioStateChanged)
		onRemoteAudioStateChanged(uid, static_cast<EnumRemoteAudioState>(state), static_cast<EnumRemoteAudioStateReason>(reason), elapsed);
}

void ArtcCli::NativeOnFirstRemoteAudioDecoded(uid_t uid, int elapsed)
{
	if (onFirstRemoteAudioDecoded)
		onFirstRemoteAudioDecoded(uid, elapsed);
}

void ArtcCli::NativeOnLocalAudioStats(const LocalAudioStats& stats)
{
	if (onLocalAudioStats)
		onLocalAudioStats(gcnew CliLocalAudioStats(stats));
}

void ArtcCli::NativeOnAudioMixingStateChanged(AUDIO_MIXING_STATE_TYPE state, AUDIO_MIXING_ERROR_TYPE error)
{
	if (onAudioMixingStateChanged)
		onAudioMixingStateChanged(static_cast<AudioMixingStateType>(state), static_cast<AudioMixingErrorType>(error));
}

void ArtcCli::NativeOnRemoteAudioMixingBegin()
{
	if (onRemoteAudioMixingBegin)
		onRemoteAudioMixingBegin();
}

void ArtcCli::NativeOnRemoteAudioMixingEnd()
{
	if (onRemoteAudioMixingEnd)
		onRemoteAudioMixingEnd();
}

void ArtcCli::NativeOnRtmpStreamingStateChanged(const char* url, RTMP_STREAM_PUBLISH_STATE state, RTMP_STREAM_PUBLISH_ERROR error)
{
	if (onRtmpStreamingStateChanged)
		onRtmpStreamingStateChanged(gcnew String(url), static_cast<EnumRtmpStreamPublishState>(state), static_cast<EnumRtmpStreamPublishError>(error));
}

void ArtcCli::NativeOnChannelMediaRelayStateChanged(CHANNEL_MEDIA_RELAY_STATE state, CHANNEL_MEDIA_RELAY_ERROR error)
{
	if (onChannelMediaRelayStateChanged)
		onChannelMediaRelayStateChanged(static_cast<EnumChannelMediaRelayState>(state), static_cast<EnumChannelMediaRelayError>(error));
}

void ArtcCli::NativeOnChannelMediaRelayEvent(CHANNEL_MEDIA_RELAY_EVENT event)
{
	if (onChannelMediaRelayEvent)
		onChannelMediaRelayEvent(static_cast<EnumChannelMediaRelayEvent>(event));
}

void ArtcCli::NativeOnLastmileProbeResult(const LastmileProbeResult& result)
{
	if (onLastmileProbeResult)
		onLastmileProbeResult(gcnew CliLastmileProbeResult(result));
}

int ArtcCli::NativeGetMaxMetadataSize()
{
	return MaxMetadataSize;
}

bool ArtcCli::NativeOnReadyToSendMetadata(IMetadataObserver::Metadata& metadata)
{
	return onReadyToSendMetadata ? onReadyToSendMetadata(gcnew CliMetadata(metadata)) : false;
}

void ArtcCli::NativeOnMetadataReceived(const IMetadataObserver::Metadata& metadata)
{
	if (onMetadataReceived)
		onMetadataReceived(gcnew CliMetadata(metadata));
}

void ArtcCliLibrary::ArtcCli::NativeOnAudioPublishStateChanged(const char* channel, STREAM_PUBLISH_STATE oldState, STREAM_PUBLISH_STATE newState, int elapseSinceLastState)
{
	if (onVideoPublishStateChanged)
		onVideoPublishStateChanged(
			gcnew String(channel),
			(EnumStreamPublishState)oldState,
			(EnumStreamPublishState)newState,
			elapseSinceLastState
		);
}

void ArtcCliLibrary::ArtcCli::NativeOnVideoPublishStateChanged(const char* channel, STREAM_PUBLISH_STATE oldState, STREAM_PUBLISH_STATE newState, int elapseSinceLastState)
{
	if (this->onVideoPublishStateChanged)
		this->onVideoPublishStateChanged(
			gcnew String(channel),
			(EnumStreamPublishState)oldState,
			(EnumStreamPublishState)newState,
			elapseSinceLastState
		);
}

void ArtcCliLibrary::ArtcCli::NativeOnAudioSubscribeStateChanged(const char* channel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState)
{
	if (this->onAudioSubscribeStateChanged)
		this->onAudioSubscribeStateChanged(
			gcnew String(channel),
			gcnew String(uid),
			(EnumStreamSubscribeState)oldState,
			(EnumStreamSubscribeState)newState,
			elapseSinceLastState
		);
}

void ArtcCliLibrary::ArtcCli::NativeOnVideoSubscribeStateChanged(const char* channel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState)
{
	if (this->onVideoSubscribeStateChanged)
		this->onVideoSubscribeStateChanged(
			gcnew String(channel),
			gcnew String(uid),
			(EnumStreamSubscribeState)oldState,
			(EnumStreamSubscribeState)newState,
			elapseSinceLastState
		);
}

void ArtcCliLibrary::ArtcCli::NativeOnFirstLocalAudioFramePublished(int elapse)
{
	if (this->onFirstLocalAudioFramePublished)
		this->onFirstLocalAudioFramePublished(elapse);
}

void ArtcCliLibrary::ArtcCli::NativeOnFirstLocalVideoFramePublished(int elapse)
{
	if (this->onFirstLocalVideoFramePublished)
		this->onFirstLocalVideoFramePublished(elapse);
}

void ArtcCliLibrary::ArtcCli::NativeOnRtmpStreamingEvent(const char* url, RTMP_STREAMING_EVENT code)
{
	if (this->onRtmpStreamingEvent)
		this->onRtmpStreamingEvent(gcnew String(url), (EnumRtmpStreamingEvent)code);
}

void ArtcCli::NativeOnUserMuteAudio(uid_t uid, bool muted)
{
	if (onUserMuteAudio)
		onUserMuteAudio(gcnew String(uid), muted);
}

void ArtcCli::NativeOnUserMuteVideo(uid_t uid, bool muted)
{
	if (onUserMuteVideo)
		onUserMuteVideo(gcnew String(uid), muted);
}

void ArtcCli::NativeOnUserEnableVideo(uid_t uid, bool enabled)
{
	if (onUserEnableVideo)
		onUserEnableVideo(gcnew String(uid), enabled);
}

void ArtcCli::NativeOnApiCallExecuted(int err, const char* api, const char* result)
{
	if (onApiCallExecuted)
		onApiCallExecuted(err, gcnew String(api), gcnew String(result));
}

void ArtcCli::NativeOnLocalVideoStats(const ar::rtc::LocalVideoStats& stats)
{
	if (onLocalVideoStats)
		onLocalVideoStats(gcnew CliLocalVideoStats(stats));
}

void ArtcCli::NativeOnRemoteVideoStats(const ar::rtc::RemoteVideoStats& stats)
{
	if (onRemoteVideoStats)
	{
		CliRemoteVideoStats^ s = gcnew CliRemoteVideoStats(stats);

		onRemoteVideoStats(s);
	}
}

void ArtcCli::NativeOnCameraReady()
{
	if (onCameraReady)
		onCameraReady();
}

void ArtcCli::NativeOnVideoStopped()
{
	if (onVideoStopped)
		onVideoStopped();
}

void ArtcCli::NativeOnConnectionLost()
{
	if (onConnectionLost)
		onConnectionLost();
}

void ArtcCli::NativeOnConnectionBanned()
{
	if (onConnectionBanned)
		onConnectionBanned();
}

void ArtcCli::NativeOnConnectionInterrupted()
{
	if (onConnectionInterrupted)
		onConnectionInterrupted();
}

void ArtcCli::NativeOnStreamMessage(uid_t uid, int streamId, const char* data, size_t length)
{
	if (onStreamMessage)
		onStreamMessage(gcnew String(uid), streamId, gcnew String(data));
}

void ArtcCli::NativeOnStreamMessageError(uid_t uid, int streamId, int code, int missed, int cached)
{
	if (onStreamMessageError)
		onStreamMessageError(gcnew String(uid), streamId, code, missed, cached);
}

void ArtcCli::NativeOnRequestChannelKey()
{
	if (onRequestChannelKey)
		onRequestChannelKey();
}

void ArtcCli::NativeOnRequestToken()
{
	if (onRequestToken)
		onRequestToken();
}

void ArtcCli::NativeOnLocalPublishFallbackToAudioOnly(bool FallbackOrRecover)
{
	if (onLocalPublishFallbackToAudioOnly)
		onLocalPublishFallbackToAudioOnly(FallbackOrRecover);
}

void ArtcCli::NativeOnRemoteSubscribeFallbackToAudioOnly(uid_t uid, bool isFallbackOrRecover)
{
	if (onRemoteSubscribeFallbackToAudioOnly)
		onRemoteSubscribeFallbackToAudioOnly(gcnew String(uid), isFallbackOrRecover);
}

void ArtcCli::NativeOnRemoteAudioStats(const RemoteAudioStats& stats)
{
	if (onRemoteAudioStats)
		onRemoteAudioStats(gcnew CliRemoteAudioStats(stats));
}

void ArtcCli::NativeOnRemoteAudioTransportStats(uid_t uid, unsigned short delay, unsigned short lost, unsigned short rxKBitRate)
{
	if (onRemoteAudioTransportStats)
		onRemoteAudioTransportStats(gcnew String(uid), delay, lost, rxKBitRate);
}

void ArtcCli::NativeOnRemoteVideoTransportStats(uid_t uid, unsigned short delay, unsigned short lost, unsigned short rxKBitRate)
{
	if (onRemoteVideoTransportStats)
		onRemoteVideoTransportStats(gcnew String(uid), delay, lost, rxKBitRate);
}

void ArtcCli::NativeOnAudioMixingBegin()
{
	if (onAudioMixingBegin)
		onAudioMixingBegin();
}

void ArtcCli::NativeOnAudioMixingEnd()
{
	if (onAudioMixingEnd)
		onAudioMixingEnd();
}

void ArtcCli::NativeOnAudioEffectFinished(int soundId)
{
	if (onAudioEffectFinished)
		onAudioEffectFinished(soundId);
}

void ArtcCli::NativeOnStreamInjectedStatus(const char* url, uid_t uid, int status)
{
	if (onStreamInjectedStatus)
		onStreamInjectedStatus(gcnew String(url), gcnew String(uid), status);
}

void ArtcCli::NativeOnMediaEngineLoadSuccess()
{
	if (onMediaEngineLoadSuccess)
		onMediaEngineLoadSuccess();
}

void ArtcCli::NativeOnMediaEngineStartCallSuccess()
{
	if (onMediaEngineStartCallSuccess)
		onMediaEngineStartCallSuccess();
}

bool ArtcCli::NativeOnSendAudioPacket(ar::rtc::IPacketObserver::Packet& packet)
{
	bool result = true;
	if (onSendAudioPacket)
	{
		CliPacket^ managedPacket = gcnew CliPacket(packet);
		result = onSendAudioPacket(managedPacket);
		if (result)
			managedPacket->writePacket(packet);
	}
	return result;
}

bool ArtcCli::NativeOnSendVideoPacket(ar::rtc::IPacketObserver::Packet& packet)
{
	bool result = true;
	if (onSendVideoPacket)
	{
		CliPacket^ managedPacket = gcnew CliPacket(packet);
		result = onSendVideoPacket(managedPacket);
		if (result)
			managedPacket->writePacket(packet);
	}
	return result;
}

bool ArtcCli::NativeOnReceiveAudioPacket(ar::rtc::IPacketObserver::Packet& packet)
{
	bool result = true;
	if (onSendVideoPacket)
	{
		CliPacket^ managedPacket = gcnew CliPacket(packet);
		result = onReceiveAudioPacket(managedPacket);
		if (result)
			managedPacket->writePacket(packet);
	}
	return result;
}

bool ArtcCli::NativeOnReceiveVideoPacket(ar::rtc::IPacketObserver::Packet& packet)
{
	bool result = true;
	if (onSendVideoPacket)
	{
		CliPacket^ managedPacket = gcnew CliPacket(packet);
		result = onReceiveVideoPacket(managedPacket);
		if (result)
			managedPacket->writePacket(packet);
	}
	return result;
}

bool ArtcCli::NativeOnRecordAudioFrame(ar::media::IAudioFrameObserver::AudioFrame& frame)
{
	bool result = true;
	if (onRecordAudioFrame)
	{
		CliAudioFrame^ clrFrame = gcnew CliAudioFrame(frame);
		result = onRecordAudioFrame(clrFrame);
		if (result)
			clrFrame->writeRaw(frame);
	}
	return result;
}

bool ArtcCli::NativeOnPlaybackAudioFrame(ar::media::IAudioFrameObserver::AudioFrame& frame)
{
	bool result = true;
	if (onPlaybackAudioFrame)
	{
		CliAudioFrame^ clrFrame = gcnew CliAudioFrame(frame);
		result = onPlaybackAudioFrame(clrFrame);
		if (result)
			clrFrame->writeRaw(frame);
	}
	return result;
}

bool ArtcCli::NativeOnPlaybackAudioFrameBeforeMixing(uid_t uid, ar::media::IAudioFrameObserver::AudioFrame& frame)
{
	bool result = true;
	if (onPlaybackAudioFrameBeforeMixing)
	{
		CliAudioFrame^ clrFrame = gcnew CliAudioFrame(frame);
		result = onPlaybackAudioFrameBeforeMixing(gcnew String(uid), clrFrame);
		if (result)
			clrFrame->writeRaw(frame);
	}
	return result;
}

bool ArtcCli::NativeOnMixedAudioFrame(ar::media::IAudioFrameObserver::AudioFrame& frame)
{
	bool result = true;
	if (onMixedAudioFrame)
	{
		CliAudioFrame^ clrFrame = gcnew CliAudioFrame(frame);
		result = onMixedAudioFrame(clrFrame);
		if (result)
			clrFrame->writeRaw(frame);
	}
	return result;
}

bool ArtcCli::NativeOnCaptureVideoFrame(ar::media::IVideoFrameObserver::VideoFrame& frame)
{
	bool result = true;
	if (onCaptureVideoFrame)
	{
		CliVideoFrame^ clrFrame = gcnew CliVideoFrame(frame);
		result = onCaptureVideoFrame(clrFrame);
		if (result)
			clrFrame->writeRaw(frame);
	}
	return result;
}

bool ArtcCli::NativeOnPreEncodeVideoFrame(ar::media::IVideoFrameObserver::VideoFrame& frame)
{
	bool result = true;
	if (onPreEncodeVideoFrame)
	{
		CliVideoFrame^ clrFrame = gcnew CliVideoFrame(frame);
		result = onPreEncodeVideoFrame(clrFrame);
		if (result)
			clrFrame->writeRaw(frame);
	}
	return result;
}

bool ArtcCli::NativeOnGetSmoothRenderingEnabled()
{
	return IsSmoothRenderingEnabled;
}

bool ArtcCli::NativeOnRenderVideoFrame(uid_t uid, ar::media::IVideoFrameObserver::VideoFrame& frame)
{
	bool result = true;
	if (onRenderVideoFrame)
	{
		CliVideoFrame^ clrFrame = gcnew CliVideoFrame(frame);
		result = onRenderVideoFrame(gcnew String(uid), clrFrame);
		if (result)
			clrFrame->writeRaw(frame);
	}
	return result;
}
bool ArtcCli::NativeOnRenderVideoFrameEx(const char* channelId, uid_t uid, ar::media::IVideoFrameObserver::VideoFrame& frame)
{
	bool result = true;
	if (onRenderVideoFrameEx)
	{
		CliVideoFrame^ clrFrame = gcnew CliVideoFrame(frame);
		result = onRenderVideoFrameEx(gcnew String(channelId), gcnew String(uid), clrFrame);
		if (result)
			clrFrame->writeRaw(frame);
	}
	return result;
}

IVideoFrameObserver::VIDEO_FRAME_TYPE ArtcCli::NativeOnGetVideoFormatPreference()
{
	return static_cast<IVideoFrameObserver::VIDEO_FRAME_TYPE>(VideoFormatPreference);
}

bool ArtcCli::NativeOnGetRotationApplied()
{
	return IsVideoRotationApplied;
}

bool ArtcCli::NativeOnGetMirrorApplied()
{
	return IsVideoMirrorApplied;
}

bool ArtcCli::NativeIsMultipleChannelVideoFrameWanted()
{
	return IsMultipleChannelVideoFrameWanted;
}

bool ArtcCli::NativeIsMultipleChannelAudioFrameWanted()
{
	return IsMultipleChannelAudioFrameWanted;
}

UINT ArtcCli::NativeObservedVideoFramePosition()
{
	return ObservedVideoFramePosition;
}
