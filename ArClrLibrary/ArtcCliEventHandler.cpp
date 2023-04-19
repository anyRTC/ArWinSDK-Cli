#include "stdafx.h"
#include "ArtcCliEventHandler.h"
using namespace ArtcCliLibrary;

ArtcCliEventHandler::ArtcCliEventHandler()
{
}

ArtcCliEventHandler::~ArtcCliEventHandler()
{
}

void ArtcCliEventHandler::onJoinChannelSuccess(const char * channel, uid_t uid, int elapsed)
{
	if (onJoinChannelSuccessEvent) onJoinChannelSuccessEvent(channel, uid, elapsed);
}

void ArtcCliEventHandler::onRejoinChannelSuccess(const char * channel, uid_t uid, int elapsed)
{
	if (onRejoinChannelSuccessEvent) onRejoinChannelSuccessEvent(channel, uid, elapsed);
}

void ArtcCliEventHandler::onWarning(int warn, const char * msg)
{
	if (onWarningEvent) onWarningEvent(warn, msg);
}

void ArtcCliEventHandler::onError(int err, const char* msg) {
	if (onErrorEvent) onErrorEvent(err, msg);
}
void ArtcCliEventHandler::onAudioQuality(uid_t uid, int quality, unsigned short delay, unsigned short lost) {
	if (onAudioQualityEvent) onAudioQualityEvent(uid, quality, delay, lost);
}
void ArtcCliEventHandler::onAudioVolumeIndication(const AudioVolumeInfo* speakers, unsigned int speakerNumber, int totalVolume) {
	if (onAudioVolumeIndicationEvent) onAudioVolumeIndicationEvent(speakers, speakerNumber, totalVolume);
}
void ArtcCliEventHandler::onLeaveChannel(const RtcStats& stats) {
	if (onLeaveChannelEvent) onLeaveChannelEvent(stats);
}
void ArtcCliEventHandler::onRtcStats(const RtcStats& stats) {
	if (onRtcStatsEvent) onRtcStatsEvent(stats);
}
void ArtcCliEventHandler::onAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState) {
	if (onAudioDeviceStateChangedEvent) onAudioDeviceStateChangedEvent(deviceId, deviceType, deviceState);
}
void ArtcCliEventHandler::onVideoDeviceStateChanged(const char* deviceId, int deviceType, int deviceState) {
	if (onVideoDeviceStateChangedEvent) onVideoDeviceStateChangedEvent(deviceId, deviceType, deviceState);
}
void ArtcCliEventHandler::onLastmileQuality(int quality) {
	if (onLastmileQualityEvent) onLastmileQualityEvent(quality);
}
void ArtcCliEventHandler::onNetworkQuality(uid_t uid, int txQuality, int rxQuality)
{
	if (onNetworkQualityEvent) onNetworkQualityEvent(uid, txQuality, rxQuality);
}
void ArtcCliEventHandler::onFirstLocalVideoFrame(int width, int height, int elapsed) {
	if (onFirstLocalVideoFrameEvent) onFirstLocalVideoFrameEvent(width, height, elapsed);
}
void ArtcCliEventHandler::onFirstRemoteVideoDecoded(uid_t uid, int width, int height, int elapsed) {
	if (onFirstRemoteVideoDecodedEvent) onFirstRemoteVideoDecodedEvent(uid, width, height, elapsed);
}
void ArtcCliEventHandler::onFirstRemoteVideoFrame(uid_t uid, int width, int height, int elapsed) {
	if (onFirstRemoteVideoFrameEvent) onFirstRemoteVideoFrameEvent(uid, width, height, elapsed);
}
void ArtcCliEventHandler::onUserJoined(uid_t uid, int elapsed) {
	if (onUserJoinedEvent) onUserJoinedEvent(uid, elapsed);
}
void ArtcCliEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) {
	if (onUserOfflineEvent) onUserOfflineEvent(uid, reason);
}
void ArtcCliEventHandler::onUserMuteAudio(uid_t uid, bool muted) {
	if (onUserMuteAudioEvent) onUserMuteAudioEvent(uid, muted);
}
void ArtcCliEventHandler::onUserMuteVideo(uid_t uid, bool muted) {
	if (onUserMuteVideoEvent) onUserMuteVideoEvent(uid, muted);
}
void ArtcCliEventHandler::onUserEnableVideo(uid_t uid, bool enabled) {
	if (onUserEnableVideoEvent) onUserEnableVideoEvent(uid, enabled);
}
void ArtcCliEventHandler::onApiCallExecuted(int err, const char* api, const char* result) {
	if (onApiCallExecutedEvent) onApiCallExecutedEvent(err, api, result);
}
void ArtcCliEventHandler::onLocalVideoStats(const LocalVideoStats& stats) {
	if (onLocalVideoStatsEvent) onLocalVideoStatsEvent(stats);
}
void ArtcCliEventHandler::onRemoteVideoStats(const RemoteVideoStats& stats) {
	if (onRemoteVideoStatsEvent) onRemoteVideoStatsEvent(stats);
}
void ArtcCliEventHandler::onCameraReady() {
	if (onCameraReadyEvent) onCameraReadyEvent();
}
void ArtcCliEventHandler::onVideoStopped() {
	if (onVideoStoppedEvent) onVideoStoppedEvent();
}
void ArtcCliEventHandler::onConnectionLost() {
	if (onConnectionLostEvent) onConnectionLostEvent();
}
void ArtcCliEventHandler::onConnectionInterrupted() {
	if (onConnectionInterruptedEvent) onConnectionLostEvent();
}
void ArtcCliEventHandler::onConnectionBanned()
{
	if (onConnectionBannedEvent) onConnectionBannedEvent();
}
void ArtcCliEventHandler::onStreamMessage(uid_t uid, int streamId, const char* data, size_t length) {
	if (onStreamMessageEvent) onStreamMessageEvent(uid, streamId, data, length);
}
void ArtcCliEventHandler::onStreamMessageError(uid_t uid, int streamId, int code, int missed, int cached) {
	if (onStreamMessageErrorEvent) onStreamMessageErrorEvent(uid, streamId, code, missed, cached);
}

void ArtcCliEventHandler::onRequestToken()
{
	if (onRequestTokenEvent) onRequestTokenEvent();
}

void ArtcCliEventHandler::onAudioMixingFinished()
{
	if (onAudioMixingFinishedEvent) onAudioMixingFinishedEvent();
}

void ArtcCliEventHandler::onActiveSpeaker(uid_t uid)
{
	if (onActiveSpeakerEvent) onActiveSpeakerEvent(uid);
}

void ArtcCliEventHandler::onClientRoleChanged(CLIENT_ROLE_TYPE oldRole, CLIENT_ROLE_TYPE newRole)
{
	if (onClientRoleChangedEvent) onClientRoleChangedEvent(oldRole, newRole);
}

void ArtcCliEventHandler::onAudioDeviceVolumeChanged(MEDIA_DEVICE_TYPE deviceType, int volume, bool muted)
{
	if (onAudioDeviceVolumeChangedEvent) onAudioDeviceVolumeChangedEvent(deviceType, volume, muted);
}

void ArtcCliEventHandler::onStreamUnpublished(const char * url)
{
	if (onStreamUnpublishedEvent) onStreamUnpublishedEvent(url);
}

void ArtcCliEventHandler::onStreamPublished(const char * url, int error)
{
	if (onStreamPublishedEvent) onStreamPublishedEvent(url, error);
}

void ArtcCliEventHandler::onTranscodingUpdated()
{
	if (onTranscodingUpdatedEvent) onTranscodingUpdatedEvent();
}

void ArtcCliEventHandler::onConnectionStateChanged(CONNECTION_STATE_TYPE state, CONNECTION_CHANGED_REASON_TYPE reason)
{
	if (onConnectionStateChangedEvent) onConnectionStateChangedEvent(state, reason);
}

void ArtcCliEventHandler::onTokenPrivilegeWillExpire(const char * token)
{
	if (onTokenPrivilegeWillExpireEvent) onTokenPrivilegeWillExpireEvent(token);
}

void ArtcCliEventHandler::onFirstLocalAudioFrame(int elapsed)
{
	if (onFirstLocalAudioFrameEvent) onFirstLocalAudioFrameEvent(elapsed);
}

void ArtcCliEventHandler::onFirstRemoteAudioFrame(uid_t uid, int elapsed)
{
	if (onFirstRemoteAudioFrameEvent) onFirstRemoteAudioFrameEvent(uid, elapsed);
}

void ArtcCliEventHandler::onUserEnableLocalVideo(uid_t uid, bool enabled)
{
	if (onUserEnableLocalVideoEvent) onUserEnableLocalVideoEvent(uid, enabled);
}

void ArtcCliEventHandler::onVideoSizeChanged(uid_t uid, int width, int height, int rotation)
{
	if (onVideoSizeChangedEvent) onVideoSizeChangedEvent(uid, width, height, rotation);
}

void ArtcCliEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (onRemoteVideoStateChangedEvent) onRemoteVideoStateChangedEvent(uid, state, reason, elapsed);
}

void ArtcCliEventHandler::onLocalPublishFallbackToAudioOnly(bool fallbackOrRecover)
{
	if (onLocalPublishFallbackToAudioOnlyEvent) onLocalPublishFallbackToAudioOnlyEvent(fallbackOrRecover);
}

void ArtcCliEventHandler::onRemoteSubscribeFallbackToAudioOnly(uid_t uid, bool isFallbackOrRecover)
{
	if (onRemoteSubscribeFallbackToAudioOnlyEvent) onRemoteSubscribeFallbackToAudioOnlyEvent(uid, isFallbackOrRecover);
}

void ArtcCliEventHandler::onRemoteAudioStats(const RemoteAudioStats & stats)
{
	if (onRemoteAudioStatsEvent) onRemoteAudioStatsEvent(stats);
}

void ArtcCliEventHandler::onRemoteAudioTransportStats(uid_t uid, unsigned short delay, unsigned short lost, unsigned short rxKBitRate)
{
	if (onRemoteAudioTransportStatsEvent) onRemoteAudioTransportStatsEvent(uid, delay, lost, rxKBitRate);
}

void ArtcCliEventHandler::onRemoteVideoTransportStats(uid_t uid, unsigned short delay, unsigned short lost, unsigned short rxKBitRate)
{
	if (onRemoteVideoTransportStatsEvent) onRemoteVideoTransportStatsEvent(uid, delay, lost, rxKBitRate);
}

void ArtcCliEventHandler::onAudioEffectFinished(int soundId)
{
	if (onAudioEffectFinishedEvent) onAudioEffectFinishedEvent(soundId);
}

void ArtcCliEventHandler::onStreamInjectedStatus(const char * url, uid_t uid, int status)
{
	if (onStreamInjectedStatusEvent) onStreamInjectedStatusEvent(url, uid, status);
}

void ArtcCliEventHandler::onMediaEngineLoadSuccess()
{
	if (onMediaEngineLoadSuccessEvent) onMediaEngineLoadSuccessEvent();
}

void ArtcCliEventHandler::onMediaEngineStartCallSuccess()
{
	if (onMediaEngineStartCallSuccessEvent) onMediaEngineStartCallSuccessEvent();
}

void ArtcCliEventHandler::onNetworkTypeChanged(NETWORK_TYPE type)
{
	if (onNetworkTypeChangedEvent) onNetworkTypeChangedEvent(type);
}

void ArtcCliEventHandler::onLocalAudioStateChanged(LOCAL_AUDIO_STREAM_STATE state, LOCAL_AUDIO_STREAM_ERROR error)
{
	if (onLocalAudioStateChangedEvent) onLocalAudioStateChangedEvent(state, error);
}

void ArtcCliEventHandler::onLocalVideoStateChanged(LOCAL_VIDEO_STREAM_STATE state, LOCAL_VIDEO_STREAM_ERROR error)
{
	if (onLocalVideoStateChangedEvent) onLocalVideoStateChangedEvent(state, error);
}

void ArtcCliEventHandler::onRemoteAudioStateChanged(uid_t uid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed)
{
	if (onRemoteAudioStateChangedEvent) onRemoteAudioStateChangedEvent(uid, state, reason, elapsed);
}

void ArtcCliEventHandler::onFirstRemoteAudioDecoded(uid_t uid, int elapsed)
{
	if (onFirstRemoteAudioDecodedEvent) onFirstRemoteAudioDecodedEvent(uid, elapsed);
}

void ArtcCliEventHandler::onLocalAudioStats(const LocalAudioStats& stats)
{
	if (onLocalAudioStatsEvent) onLocalAudioStatsEvent(stats);
}

void ArtcCliEventHandler::onAudioMixingStateChanged(AUDIO_MIXING_STATE_TYPE state, AUDIO_MIXING_REASON_TYPE reason)
{
	if (onAudioMixingStateChangedEvent) onAudioMixingStateChangedEvent(state, reason);
}

void ArtcCliEventHandler::onRemoteAudioMixingBegin()
{
	if (onRemoteAudioMixingBeginEvent) onRemoteAudioMixingBeginEvent();
}

void ArtcCliEventHandler::onRemoteAudioMixingEnd()
{
	if (onRemoteAudioMixingEndEvent) onRemoteAudioMixingEndEvent();
}

void ArtcCliEventHandler::onRtmpStreamingStateChanged(const char* url, RTMP_STREAM_PUBLISH_STATE state, RTMP_STREAM_PUBLISH_ERROR_TYPE error)
{
	if (onRtmpStreamingStateChangedEvent) onRtmpStreamingStateChangedEvent(url, state, error);
}

void ArtcCliEventHandler::onChannelMediaRelayStateChanged(CHANNEL_MEDIA_RELAY_STATE state, CHANNEL_MEDIA_RELAY_ERROR error)
{
	if (onChannelMediaRelayStateChangedEvent) onChannelMediaRelayStateChangedEvent(state, error);
}

void ArtcCliEventHandler::onChannelMediaRelayEvent(CHANNEL_MEDIA_RELAY_EVENT event)
{
	if (onChannelMediaRelayEventEvnet) onChannelMediaRelayEventEvnet(event);
}

void ArtcCliEventHandler::onLastmileProbeResult(const LastmileProbeResult& result)
{
	if (onLastmileProbeResultEvent) onLastmileProbeResultEvent(result);
}

void ArtcCliLibrary::ArtcCliEventHandler::onAudioPublishStateChanged(const char* channel, STREAM_PUBLISH_STATE oldState, STREAM_PUBLISH_STATE newState, int elapseSinceLastState)
{
	if (onAudioPublishStateChangedEvent) onAudioPublishStateChangedEvent(channel, oldState, newState, elapseSinceLastState);
}

void ArtcCliLibrary::ArtcCliEventHandler::onVideoPublishStateChanged(const char* channel, STREAM_PUBLISH_STATE oldState, STREAM_PUBLISH_STATE newState, int elapseSinceLastState)
{
	if (onVideoPublishStateChangedEvent) onVideoPublishStateChangedEvent(channel, oldState, newState, elapseSinceLastState);
}

void ArtcCliLibrary::ArtcCliEventHandler::onAudioSubscribeStateChanged(const char* channel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState)
{
	if (onAudioSubscribeStateChangedEvent) onAudioSubscribeStateChangedEvent(channel, uid, oldState, newState, elapseSinceLastState);
}

void ArtcCliLibrary::ArtcCliEventHandler::onVideoSubscribeStateChanged(const char* channel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState)
{
	if (onVideoSubscribeStateChangedEvent) onVideoSubscribeStateChangedEvent(channel, uid, oldState, newState, elapseSinceLastState);
}

void ArtcCliLibrary::ArtcCliEventHandler::onFirstLocalAudioFramePublished(int elapsed)
{
	if (onFirstLocalAudioFrameEvent) onFirstLocalAudioFrameEvent(elapsed);
}

void ArtcCliLibrary::ArtcCliEventHandler::onFirstLocalVideoFramePublished(int elapsed)
{
	if (onFirstLocalVideoFramePublishedEvent) onFirstLocalVideoFramePublishedEvent(elapsed);
}

void ArtcCliLibrary::ArtcCliEventHandler::onRtmpStreamingEvent(const char* url, RTMP_STREAMING_EVENT eventCode)
{
	if (onRtmpStreamingEventEvent) onRtmpStreamingEventEvent(url, eventCode);
}
