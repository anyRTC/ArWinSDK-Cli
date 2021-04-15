#include "stdafx.h"
#include "ArtcCliRawFrameObserver.h"

using namespace ArtcCliLibrary;

ArtcCliAudioFrameObserver::ArtcCliAudioFrameObserver()
{

}

bool ArtcCliAudioFrameObserver::onRecordAudioFrame(AudioFrame & audioFrame)
{
	if (onRecordAudioFrameEvent) return onRecordAudioFrameEvent(audioFrame);
	return true;
}

bool ArtcCliAudioFrameObserver::onPlaybackAudioFrame(AudioFrame & audioFrame)
{
	if (onPlaybackAudioFrameEvent) return onPlaybackAudioFrameEvent(audioFrame);
	return true;
}

bool ArtcCliAudioFrameObserver::onMixedAudioFrame(AudioFrame & audioFrame)
{
	if (onMixedAudioFrameEvent) return onMixedAudioFrameEvent(audioFrame);
	return false;
}

bool ArtcCliAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(uid_t uid, AudioFrame & audioFrame)
{
	if (onPlaybackAudioFrameBeforeMixingEvent) return onPlaybackAudioFrameBeforeMixingEvent(uid, audioFrame);
	return true;
}

bool ArtcCliAudioFrameObserver::isMultipleChannelFrameWanted()
{
	if (isMultipleChannelFrameWantedEvent) return isMultipleChannelFrameWantedEvent();
	return false;
}

bool ArtcCliAudioFrameObserver::onPlaybackAudioFrameBeforeMixingEx(const char* channelId, uid_t uid,
	AudioFrame& audioFrame)
{
	if (onPlaybackAudioFrameBeforeMixingExEvent) return onPlaybackAudioFrameBeforeMixingExEvent(channelId,uid, audioFrame);
	return true;
}

ArtcCliVideoFrameObserver::ArtcCliVideoFrameObserver()
{
}
bool ArtcCliVideoFrameObserver::onCaptureVideoFrame(VideoFrame & videoFrame)
{
	if (onCaptureVideoFrameEvent) return onCaptureVideoFrameEvent(videoFrame);
	return true;
}

bool ArtcCliVideoFrameObserver::onPreEncodeVideoFrame(VideoFrame& videoFrame)
{
	if (onPreEncodeVideoFrameEvent) return onPreEncodeVideoFrameEvent(videoFrame);
	return true;
}

bool ArtcCliVideoFrameObserver::getSmoothRenderingEnabled()
{
	if (getSmoothRenderingEnabledEvent) return getSmoothRenderingEnabledEvent();
	return false;
}

bool ArtcCliVideoFrameObserver::onRenderVideoFrame(uid_t uid, VideoFrame & videoFrame)
{
	if (onRenderVideoFrameEvent) return onRenderVideoFrameEvent(uid, videoFrame);
	return true;
}

ar::media::IVideoFrameObserver::VIDEO_FRAME_TYPE ArtcCliVideoFrameObserver::getVideoFormatPreference()
{
	if (onGetVideoFormatPreferenceEvent) return onGetVideoFormatPreferenceEvent();
	return FRAME_TYPE_YUV420;
}

bool ArtcCliVideoFrameObserver::getRotationApplied()
{
	if (onGetRotationAppliedEvent) return onGetRotationAppliedEvent();
	return false;
}

bool ArtcCliVideoFrameObserver::getMirrorApplied()
{
	if (onGetMirrorAppliedEvent) return onGetMirrorAppliedEvent();
	return false;
}

bool ArtcCliVideoFrameObserver::isMultipleChannelFrameWanted()
{
	if (isMultipleChannelFrameWantedEvent) return isMultipleChannelFrameWantedEvent();
	return false;
}

bool ArtcCliVideoFrameObserver::onRenderVideoFrameEx(const char* channelId, uid_t uid, VideoFrame& videoFrame)
{
	if (onRenderVideoFrameExEvent) return onRenderVideoFrameExEvent(channelId,uid, videoFrame);
	return true;
}

UINT ArtcCliVideoFrameObserver::getObservedFramePosition()
{
	if (getObservedFramePositionEvent) return getObservedFramePositionEvent();
	return static_cast<uint32_t>(POSITION_POST_CAPTURER | POSITION_PRE_RENDERER);
}


