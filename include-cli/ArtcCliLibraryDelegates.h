#pragma once
#include "../ARtcKit/IArRtcEngine.h"
#include "ArtcCliAudioDeviceManager.h"
#include "ArtcCliEventHandler.h"
#include "ArtcCliPacketObserver.h"
#include "ArtcCliRawFrameObserver.h"
#include "ArtcCliVideoDeviceManager.h"
#include "ArtcCliLibraryEnum.h"
#include "ArtcCliLibraryTypes.h"

#include <string>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace ar::rtc;

namespace ArtcCliLibrary {

	//RtcEngineEventHandler Part
	public
		delegate void onJoinChannelSuccess(String^ channel, String^ uid, int elapsed);
	public
		delegate void onRejoinChannelSuccess(String^ channel, String^ uid, int elapsed);
	public
		delegate void onWarning(int warn, String^ msg);
	public
		delegate void onError(int err, String^ msg);
	public
		delegate void onAudioQuality(String^ uid, int quality, unsigned short delay, unsigned short lost);
	public
		delegate void onAudioVolumeIndication(List<CliAudioVolumeInfo^>^ speakers, int totalVolume);
	public
		delegate void onLeaveChannel(CliRtcStats^ stat);
	public
		delegate void onUserJoined(String^ uid, int elapsed);
	public
		delegate void onUserOffline(String^ uid, UserOfflineType reason);
	public
		delegate void onRtcStats(CliRtcStats^ stat);
	public
		delegate void onLocalVideoStats(CliLocalVideoStats^ stats);
	public
		delegate void onRemoteVideoStats(CliRemoteVideoStats^ stats);
	public
		delegate void onFirstLocalVideoFrame(int width, int height, int elapsed);
	public
		delegate void onFirstRemoteVideoDecoded(String^ uid, int width, int height, int elapsed);
	public
		delegate void onFirstRemoteVideoFrame(String^ uid, int width, int height, int elapsed);

	public
		delegate void onAudioDeviceStateChanged(String^ deviceid, int deviceType, int deviceState);
	public
		delegate void onVideoDeviceStateChanged(String^ deviceid, int deviceType, int deviceState);

	public
		delegate void onLastmileQuality(int quality);
	public
		delegate void onNetworkQuality(String^ uid, int txQuality, int rxQuality);

	public
		delegate void onUserMuteAudio(String^ uid, bool muted);
	public
		delegate void onUserMuteVideo(String^ uid, bool muted);
	public
		delegate void onUserEnableVideo(String^ uid, bool enabled);
	public
		delegate void onCameraReady();
	public
		delegate void onVideoStopped();
	public
		delegate void onConnectionInterrupted();
	public
		delegate void onConnectionLost();
	public
		delegate void onConnectionBanned();

	public
		delegate void onRefreshRecordingServiceStatus(int status);
	public
		delegate void onApiCallExecuted(int err, String^ api, String^ result);
	public
		delegate void onStreamMessage(String^ uid, int streamId, String^ data);
	public
		delegate void onStreamMessageError(String^ uid, int streamId, int code, int missed, int cached);
	public
		delegate void onRequestChannelKey();
	public
		delegate void onRequestToken();
	public
		delegate void onAudioMixingFinished();

	public
		delegate void onActiveSpeaker(String^ uid);

	public
		delegate void onClientRoleChanged(ClientRoleType, ClientRoleType);
	public
		delegate void onAudioDeviceVolumeChanged(MediaDeviceType, int, bool);

	public
		delegate void onStreamUnpublished(String^ url);
	public
		delegate void onStreamPublished(String^ url, int error);
	public
		delegate void onTranscodingUpdated();

	public delegate void onRtmpStreamingEvent(String^, EnumRtmpStreamingEvent);

	public
		delegate void onConnectionStateChanged(EnumConnectionStateType state, EnumConnectionChangedReasonType reason);
	public
		delegate void onTokenPrivilegeWillExpire(String^ token);
	public
		delegate void onFirstLocalAudioFrame(int elapsed);
	public
		delegate void onFirstRemoteAudioFrame(String^ uid, int elapsed);
	public
		delegate void onUserEnableLocalVideo(String^ uid, bool enabled);
	public
		delegate void onVideoSizeChanged(String^ uid, int width, int height, int rotation);
	public
		delegate void onRemoteVideoStateChanged(String^ uid, EnumRemoteVideoState state, EnumRemoteVideoStateReason reason, int elapsed);
	public
		delegate void onLocalPublishFallbackToAudioOnly(bool);
	public
		delegate void onRemoteSubscribeFallbackToAudioOnly(String^ uid, bool isFallbackOrRecover);
	public
		delegate void onCameraFocusAreaChanged(int x, int y, int width, int height);
	public
		delegate void onRemoteAudioStats(CliRemoteAudioStats^ stats);
	public
		delegate void onRemoteAudioTransportStats(String^ uid, short delay, short lost, short rxKBitRate);
	public
		delegate void onRemoteVideoTransportStats(String^ uid, short delay, short lost, short rxKBitRate);
	public
		delegate void onAudioMixingBegin();
	public
		delegate void onAudioMixingEnd();
	public
		delegate void onAudioEffectFinished(int soundId);
	public
		delegate void onStreamInjectedStatus(String^ url, String^ uid, int status);
	public
		delegate void onMediaEngineLoadSuccess();
	public
		delegate void onMediaEngineStartCallSuccess();

	//PacketObserver Part
	public
		delegate bool onSendAudioPacket(CliPacket^ packet);
	public
		delegate bool onSendVideoPacket(CliPacket^ packet);
	public
		delegate bool onReceiveAudioPacket(CliPacket^ packet);
	public
		delegate bool onReceiveVideoPacket(CliPacket^ packet);

	//Raw Data Part
	public
		delegate bool onRecordAudioFrame(CliAudioFrame^ frame);
	public
		delegate bool onPlaybackAudioFrame(CliAudioFrame^ frame);
	public
		delegate bool onPlaybackAudioFrameBeforeMixing(String^ uid, CliAudioFrame^ frame);
	public
		delegate bool onMixedAudioFrame(CliAudioFrame^ frame);

	public
		delegate bool onCaptureVideoFrame(CliVideoFrame^ frame);
	public
		delegate bool onPreEncodeVideoFrame(CliVideoFrame^ frame);
	public
		delegate bool onGetSmoothRenderingEnabled();
	public
		delegate bool onRenderVideoFrame(String^ uid, CliVideoFrame^ frame);
	public delegate bool onRenderVideoFrameEx(String^channelId,String^ uid, CliVideoFrame^ frame);

	public
		delegate void onNetworkTypeChanged(NetworkType type);

	public
		delegate void onLocalAudioStateChanged(LocalAudioStreamState state, LocalAudioStreamError error);

	public
		delegate void onLocalVideoStateChanged(LocalVideoStreamState state, LocalVideoStreamError error);

	public delegate void onRemoteAudioStateChanged(uid_t, EnumRemoteAudioState, EnumRemoteAudioStateReason, int);

	public delegate void onFirstRemoteAudioDecoded(uid_t, int);

	public delegate void onLocalAudioStats(CliLocalAudioStats^ stats);

	public delegate void onAudioMixingStateChanged(AudioMixingStateType state, AudioMixingErrorType error);

	public delegate void onRtmpStreamingStateChanged(String^, EnumRtmpStreamPublishState, EnumRtmpStreamPublishError);

	public delegate void onChannelMediaRelayStateChanged(EnumChannelMediaRelayState, EnumChannelMediaRelayError);

	public delegate void onAudioPublishStateChanged(String^, EnumStreamPublishState, EnumStreamPublishState, int);
	public delegate void onVideoPublishStateChanged(String^, EnumStreamPublishState, EnumStreamPublishState, int);
	public delegate void onAudioSubscribeStateChanged(String^, String^, EnumStreamSubscribeState, EnumStreamSubscribeState, int);
	public delegate void onVideoSubscribeStateChanged(String^, String^, EnumStreamSubscribeState, EnumStreamSubscribeState, int);
	public delegate void onFirstLocalAudioFramePublished(int);
	public delegate void onFirstLocalVideoFramePublished(int);

}