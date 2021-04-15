#include "ArtcCliChannel.h"
using namespace ArtcCliLibrary;

ArtcCliChannel::ArtcCliChannel(IChannel* channel) :
	channel(channel),
	gchs(gcnew List<GCHandle>),
	events(new ArtcCliChannelEventHandler()),
	metaObserver(new ArtcCliMetadataObserver()),
	packetObserver(new ArtcCliPacketObserver())
{
	bindEventHandler();
	bindMetaObserver();
	bindPacketObserver();

	channel->registerMediaMetadataObserver(metaObserver, ar::rtc::IMetadataObserver::METADATA_TYPE::VIDEO_METADATA);
}

ArtcCliChannel::~ArtcCliChannel()
{
	channel->release();
	for each (GCHandle handler in gchs) handler.Free();
	delete channel;
}

int ArtcCliChannel::joinChannel(String^ token, String^ info, uid_t uid, CliChannelMediaOptions^ options)
{
	return channel->joinChannel(MarshalString(token).c_str(), MarshalString(info).c_str(), uid, options);
}

int ArtcCliChannel::joinChannelWithUserAccount(String^ token, String^ account, CliChannelMediaOptions^ options)
{
	return channel->joinChannelWithUserAccount(MarshalString(token).c_str(), MarshalString(account).c_str(), options);
}

int ArtcCliChannel::leaveChannel()
{
	channel->registerPacketObserver(nullptr);
	return channel->leaveChannel();
}

int ArtcCliChannel::publish()
{
	return channel->publish();
}

int ArtcCliChannel::unpublish()
{
	return channel->unpublish();
}

String^ ArtcCliChannel::channelId()
{
	return gcnew String(channel->channelId());
}

int ArtcCliChannel::getCallId(String^% callid)
{
	ar::util::AString string;
	int result = channel->getCallId(string);
	callid = gcnew String(string->c_str());
	return result;	
}

int ArtcCliChannel::renewToken(String^ token)
{
	return channel->renewToken(MarshalString(token).c_str());
}

int ArtcCliChannel::setEncryptionSecret(String^ secret)
{
	channel->registerPacketObserver(packetObserver);
	return channel->setEncryptionSecret(MarshalString(secret).c_str());
}

int ArtcCliChannel::setEncryptionMode(String^ mode)
{
	return channel->setEncryptionMode(MarshalString(mode).c_str());
}

int ArtcCliChannel::setClientRole(EnumClientRoleType type)
{
	return channel->setClientRole(static_cast<CLIENT_ROLE_TYPE>(type));
}

int ArtcCliChannel::setRemoteVoicePosition(uid_t uid, double pan, double gain)
{
	return channel->setRemoteVoicePosition(uid, pan, gain);
}

int ArtcCliChannel::setRemoteRenderMode(uid_t uid, EnumRenderModeType renderMode, EnumVideoMirrorModeType mirrorMode)
{
	return channel->setRemoteRenderMode(uid,
		static_cast<RENDER_MODE_TYPE>(renderMode),
		static_cast<VIDEO_MIRROR_MODE_TYPE>(mirrorMode));
}

int ArtcCliChannel::setDefaultMuteAllRemoteAudioStreams(bool mute)
{
	return channel->setDefaultMuteAllRemoteAudioStreams(mute);
}

int ArtcCliChannel::setDefaultMuteAllRemoteVideoStreams(bool mute)
{
	return channel->setDefaultMuteAllRemoteVideoStreams(mute);
}

int ArtcCliChannel::muteAllRemoteAudioStreams(bool mute)
{
	return channel->muteAllRemoteAudioStreams(mute);
}

int ArtcCliChannel::adjustUserPlaybackSignalVolume(uid_t uid, int volume)
{
	return channel->adjustUserPlaybackSignalVolume(uid, volume);
}

int ArtcCliChannel::muteRemoteAudioStream(uid_t uid, bool mute)
{
	return channel->muteRemoteAudioStream(uid, mute);
}

int ArtcCliChannel::muteAllRemoteVideoStreams(bool mute)
{
	return channel->muteAllRemoteVideoStreams(mute);
}

int ArtcCliChannel::muteRemoteVideoStream(uid_t uid, bool mute)
{
	return channel->muteRemoteVideoStream(uid, mute);
}

int ArtcCliChannel::setRemoteVideoStreamType(uid_t uid, EnumRemoteVideoStreamType type)
{
	return channel->setRemoteVideoStreamType(uid,
		static_cast<REMOTE_VIDEO_STREAM_TYPE>(type));
}

int ArtcCliChannel::setRemoteDefaultVideoStreamType(EnumRemoteVideoStreamType type)
{
	return channel->setRemoteDefaultVideoStreamType(
		static_cast<REMOTE_VIDEO_STREAM_TYPE>(type)
		);
}

int ArtcCliChannel::createDataStream(int% streamId, bool reliable, bool ordered)
{
	int id;
	int result = channel->createDataStream(&id, reliable, ordered);
	streamId = id;
	return result;
}

int ArtcCliChannel::sendStreamMessage(int streamId, String^ msg)
{
	std::string data = MarshalString(msg);
	return channel->sendStreamMessage(streamId, data.c_str(), data.size());
}

int ArtcCliChannel::addPublishStreamUrl(String^ url, bool transcodingEnabled)
{
	return channel->addPublishStreamUrl(MarshalString(url).c_str(), transcodingEnabled);
}

int ArtcCliChannel::removePublishStreamUrl(String^ url)
{
	return channel->removePublishStreamUrl(MarshalString(url).c_str());
}

int ArtcCliChannel::setLiveTranscoding(CliLiveTranscoding^ transcoding)
{
	return channel->setLiveTranscoding(transcoding);
}

int ArtcCliChannel::addInjectStreamUrl(String^ url, CliInjectStreamConfig^ config)
{
	return channel->addInjectStreamUrl(MarshalString(url).c_str(), config);
}

int ArtcCliChannel::removeInjectStreamUrl(String^ url)
{
	return channel->removeInjectStreamUrl(MarshalString(url).c_str());
}

int ArtcCliChannel::startChannelMediaRelay(CliChannelMediaRelayConfiguration^ config)
{
	return channel->startChannelMediaRelay(config);
}

int ArtcCliChannel::updateChannelMediaRelay(CliChannelMediaRelayConfiguration^ config)
{
	return channel->updateChannelMediaRelay(config);
}

int ArtcCliChannel::stopChannelMediaRelay()
{
	return channel->stopChannelMediaRelay();
}

void ArtcCliChannel::bindEventHandler()
{
	regEvent(events->onChannelWarningEvent, gcnew OnChannelWarningType::Type(this, &ArtcCliChannel::NativeOnChannelWarning));
	regEvent(events->onChannelErrorEvent, gcnew OnChannelErrorType::Type(this, &ArtcCliChannel::NativeOnChannelError));
	regEvent(events->onJoinChannelSuccessEvent, gcnew OnJoinChannelSuccessType::Type(this, &ArtcCliChannel::NativeOnJoinChannelSuccess));
	regEvent(events->onRejoinChannelSuccessEvent, gcnew OnRejoinChanelSuccessType::Type(this, &ArtcCliChannel::NativeOnRejoinChanelSuccess));
	regEvent(events->onLeaveChannelEvent, gcnew OnLeaveChannelType::Type(this, &ArtcCliChannel::NativeOnLeaveChannel));
	regEvent(events->onClientRoleChangedEvent, gcnew OnClientRoleChangedType::Type(this, &ArtcCliChannel::NativeOnClientRoleChanged));
	regEvent(events->onUserJoinEvent, gcnew OnUserJoinedType::Type(this, &ArtcCliChannel::NativeOnUserJoined));
	regEvent(events->onUserOfflineEvent, gcnew OnUserOfflineType::Type(this, &ArtcCliChannel::NativeOnUserOffline));
	regEvent(events->onConnectionLostEvent, gcnew OnConnectionLostType::Type(this, &ArtcCliChannel::NativeOnConnectionLost));
	regEvent(events->onRequestTokenEvent, gcnew OnRequestTokenType::Type(this, &ArtcCliChannel::NativeOnRequestToken));
	regEvent(events->onTokePrivilegeWillExpireEvent, gcnew OnTokenPrivilegeWillExpireType::Type(this, &ArtcCliChannel::NativeOnTokenPrivilegeWillExpire));
	regEvent(events->onRtcStatsEvent, gcnew OnRtcStatsType ::Type(this, &ArtcCliChannel::NativeOnRtcStats));
	regEvent(events->onNetworkQualityEvent, gcnew OnNetworkQualityType::Type(this, &ArtcCliChannel::NativeOnNetworkQuality));
	regEvent(events->onRemoteVideoStatsEvent, gcnew OnRemoteVideoStatsType::Type(this, &ArtcCliChannel::NativeOnRemoteVideoStats));
	regEvent(events->onRemoteAudioStatsEvent, gcnew OnRemoteAudioStatsType::Type(this, &ArtcCliChannel::NativeOnRemoteAudioStats));
	regEvent(events->onRemoteAudioStateChangedEvent, gcnew OnRemoteAudioStateChangedType::Type(this, &ArtcCliChannel::NativeOnRemoteAudioStateChanged));
	regEvent(events->onActiveSpeakerEvent, gcnew OnActiveSpeakerType::Type(this, &ArtcCliChannel::NativeOnActiveSpeaker));
	regEvent(events->onVideoSizeChangedEvent, gcnew OnVideoSizeChangedType::Type(this, &ArtcCliChannel::NativeOnVideoSizeChanged));
	regEvent(events->onRemoteVideoStateChangedEvent, gcnew OnRemoteVideoStateChangedType::Type(this, &ArtcCliChannel::NativeOnRemoteVideoStateChanged));
	regEvent(events->onStreamMessageEvent, gcnew OnStreamMessageType::Type(this, &ArtcCliChannel::NativeOnStreamMessage));
	regEvent(events->onStreamMessageErrorEvent, gcnew OnStreamMessageErrorType::Type(this, &ArtcCliChannel::NativeOnStreamMessageError));
	regEvent(events->onChannelMediaRelayStateChangedEvent, gcnew OnChannelMediaRelayStateChangedType::Type(this, &ArtcCliChannel::NativeOnChannelMediaRelayStateChanged));
	regEvent(events->onChannelMediaRelayEventEvent, gcnew OnChannelMediaRelayEventType::Type(this, &ArtcCliChannel::NativeOnChannelMediaRelayEvent));
	regEvent(events->onRtmpStreamingStateChangedEvent, gcnew OnRtmpStreamingStateChangedType::Type(this, &ArtcCliChannel::NativeOnRtmpStreamingStateChanged));
	regEvent(events->onTranscodingUpdatedEvent, gcnew OnTranscodingUpdatedType::Type(this, &ArtcCliChannel::NativeOnTranscodingUpdated));
	regEvent(events->onStreamInjectedStatusEvent, gcnew OnStreamInjectedStatusType::Type(this, &ArtcCliChannel::NativeOnStreamInjectedStatus));
	regEvent(events->onRemoteSubscribeFallbackToAudioOnlyEvent, gcnew OnRemoteSubscribeFallbackToAudioOnlyType::Type(this, &ArtcCliChannel::NativeOnRemoteSubscribeFallbackToAudioOnly));
	regEvent(events->onConnectionStateChangedEvent, gcnew OnConnectionStateChangedType::Type(this, &ArtcCliChannel::NativeOnConnectionStateChanged));
}

void ArtcCliChannel::bindMetaObserver()
{
	regEvent(metaObserver->onGetMaxMetadataSizeEvent, gcnew NativeOnGetMaxMetadataSizeDelegate(this, &ArtcCliChannel::NativeGetMaxMetadataSize));
	regEvent(metaObserver->onReadyToSendMetadataEvent, gcnew NativeOnReadyToSendMetadataDelegate(this, &ArtcCliChannel::NativeOnReadyToSendMetadata));
	regEvent(metaObserver->onMetadataReceivedEvent, gcnew NativeOnMetadataReceivedDelegate(this, &ArtcCliChannel::NativeOnMetadataReceived));
}

void ArtcCliChannel::bindPacketObserver()
{
	regEvent(packetObserver->onSendAudioPacketEvent, gcnew NativeOnSendAudioPacketDelegate(this, &ArtcCliChannel::NativeOnSendAudioPacket));
	regEvent(packetObserver->onSendVideoPacketEvent, gcnew NativeOnSendVideoPacketDelegate(this, &ArtcCliChannel::NativeOnSendVideoPacket));
	regEvent(packetObserver->onReceiveVideoPacketEvent, gcnew NativeOnSendVideoPacketDelegate(this, &ArtcCliChannel::NativeOnReceiveVideoPacket));
	regEvent(packetObserver->onReceiveAudioPacketEvent, gcnew NativeOnReceiveAudioPacketDelegate(this, &ArtcCliChannel::NativeOnReceiveAudioPacket));
}

void ArtcCliChannel::NativeOnChannelWarning(IChannel* channel, int warn, const char* msg)
{
	if (onChannelWarning) onChannelWarning(this, warn, gcnew String(msg));
}

void ArtcCliChannel::NativeOnChannelError(IChannel* channel, int err, const char* msg)
{
	if (onChannelError) onChannelError(this, err, gcnew String(msg));
}

void ArtcCliChannel::NativeOnJoinChannelSuccess(IChannel* channel, uid_t uid, int elapsed)
{
	if (onJoinChannelSuccess) onJoinChannelSuccess(this, gcnew String(uid), elapsed);
}

void ArtcCliChannel::NativeOnRejoinChanelSuccess(IChannel* channel, uid_t uid, int elapsed)
{
	if (onRejoinChannelSuccess) onRejoinChannelSuccess(this, gcnew String(uid), elapsed);
}

void ArtcCliChannel::NativeOnLeaveChannel(IChannel* channel, const RtcStats& stats)
{
	if (onLeaveChannel) onLeaveChannel(this, gcnew CliRtcStats(stats));
}

void ArtcCliChannel::NativeOnClientRoleChanged(IChannel* channel, CLIENT_ROLE_TYPE old, CLIENT_ROLE_TYPE newer)
{
	if (onClientRoleChanged) onClientRoleChanged(this, static_cast<EnumClientRoleType>(old), static_cast<EnumClientRoleType>(newer));
}

void ArtcCliChannel::NativeOnUserJoined(IChannel* channel, uid_t uid, int elapsed)
{
	if (onUserJoined) onUserJoined(this, gcnew String(uid), elapsed);
}

void ArtcCliChannel::NativeOnUserOffline(IChannel* channel, uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	if (onUserOffline) onUserOffline(this, gcnew String(uid), static_cast<EnumUserOfflineReasonType>(reason));
}

void ArtcCliChannel::NativeOnConnectionLost(IChannel* channel)
{
	if (onConnectionLost) onConnectionLost(this);
}

void ArtcCliChannel::NativeOnRequestToken(IChannel* channel)
{
	if (onRequestToken) onRequestToken(this);
}

void ArtcCliChannel::NativeOnTokenPrivilegeWillExpire(IChannel* channel, const char* token)
{
	if (onTokenPrivilegeWilExpire) onTokenPrivilegeWilExpire(this, gcnew String(token));
}

void ArtcCliChannel::NativeOnRtcStats(IChannel* channel, const RtcStats& stats)
{
	if (onRtcStats) onRtcStats(this, gcnew CliRtcStats(stats));
}

void ArtcCliChannel::NativeOnNetworkQuality(IChannel* channel, uid_t uid, int txQuality, int rxQuality)
{
	if (onNetworkQuality) onNetworkQuality(this, gcnew String(uid), txQuality, rxQuality);
}

void ArtcCliChannel::NativeOnRemoteVideoStats(IChannel* channel, const RemoteVideoStats& stats)
{
	if (onRemoteVideoStats) onRemoteVideoStats(this, gcnew CliRemoteVideoStats(stats));
}

void ArtcCliChannel::NativeOnRemoteAudioStats(IChannel* channel, const RemoteAudioStats& stats)
{
	if (onRemoteAudioStats) onRemoteAudioStats(this, gcnew CliRemoteAudioStats(stats));
}

void ArtcCliChannel::NativeOnRemoteAudioStateChanged(IChannel* channel, uid_t uid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed)
{
	if (onRemoteAudioStateChanged) onRemoteAudioStateChanged(this, gcnew String(uid), static_cast<EnumRemoteAudioState>(state), static_cast<EnumRemoteAudioStateReason>(reason), elapsed);
}

void ArtcCliChannel::NativeOnActiveSpeaker(IChannel* channel, uid_t uid)
{
	if (onActiveSpeaker) onActiveSpeaker(this, gcnew String(uid));
}

void ArtcCliChannel::NativeOnVideoSizeChanged(IChannel* channel, uid_t uid, int width, int height, int rotation)
{
	if (onVideoSizeChanged) onVideoSizeChanged(this, gcnew String(uid), width, height, rotation);
}

void ArtcCliChannel::NativeOnRemoteVideoStateChanged(IChannel* channel, uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (onRemoteVideoStateChanged) onRemoteVideoStateChanged(this, gcnew String(uid), static_cast<EnumRemoteVideoState>(state), static_cast<EnumRemoteVideoStateReason>(reason), elapsed);
}

void ArtcCliChannel::NativeOnStreamMessage(IChannel* channel, uid_t uid, int streamId, const char* data, size_t length)
{
	auto bytes = gcnew array<Byte>(length);
	if (length > 0) Marshal::Copy(IntPtr(reinterpret_cast<void*>(const_cast<char*>(data))), bytes, 0, length);
	if (onStreamMessage) onStreamMessage(this, gcnew String(uid), streamId, bytes);
}

void ArtcCliChannel::NativeOnStreamMessageError(IChannel* channel, uid_t uid, int streamId, int code, int missed, int cached)
{
	if (onStreamMessageError) onStreamMessageError(this, gcnew String(uid), streamId, code, missed, cached);
}

void ArtcCliChannel::NativeOnChannelMediaRelayStateChanged(IChannel* channel, CHANNEL_MEDIA_RELAY_STATE state, CHANNEL_MEDIA_RELAY_ERROR code)
{
	if (onChannelMediaRelayStateChanged)
		onChannelMediaRelayStateChanged(this,
			static_cast<EnumChannelMediaRelayState>(state),
			static_cast<EnumChannelMediaRelayError>(code));
}

void ArtcCliChannel::NativeOnChannelMediaRelayEvent(IChannel* channel, CHANNEL_MEDIA_RELAY_EVENT code)
{
	if (onChannelMediaRelayEvent)
		onChannelMediaRelayEvent(this,
			static_cast<EnumChannelMediaRelayEvent>(code));
}

void ArtcCliChannel::NativeOnRtmpStreamingStateChanged(IChannel* channel, const char* url, RTMP_STREAM_PUBLISH_STATE state, RTMP_STREAM_PUBLISH_ERROR code)
{
	if (onRtmpStreamingStateChanged)
		onRtmpStreamingStateChanged(this, gcnew String(url),
			static_cast<EnumRtmpStreamPublishState>(state),
			static_cast<EnumRtmpStreamPublishError>(code));
}

void ArtcCliChannel::NativeOnTranscodingUpdated(IChannel* channel)
{
	if (onTranscodingUpdated) onTranscodingUpdated(this);
}

void ArtcCliChannel::NativeOnStreamInjectedStatus(IChannel* channel, const char* url, uid_t uid, int status)
{
	if (onStreamInjectedStatus)
		onStreamInjectedStatus(this, gcnew String(url), gcnew String(uid), status);
}

void ArtcCliChannel::NativeOnRemoteSubscribeFallbackToAudioOnly(IChannel* channel, uid_t uid, bool isFallbackOrRecover)
{
	if (onRemoteSubscribeFallbackToAudioOnly)
		onRemoteSubscribeFallbackToAudioOnly(this, gcnew String(uid), isFallbackOrRecover);
}

void ArtcCliChannel::NativeOnConnectionStateChanged(IChannel* channel, CONNECTION_STATE_TYPE state, CONNECTION_CHANGED_REASON_TYPE reason)
{
	if (onConnectionStateChanged)
		onConnectionStateChanged(this,
			static_cast<EnumConnectionStateType>(state),
			static_cast<EnumConnectionChangedReasonType>(reason));
}

int ArtcCliChannel::NativeGetMaxMetadataSize()
{
	return 1024;
}

bool ArtcCliChannel::NativeOnReadyToSendMetadata(IMetadataObserver::Metadata& metadata)
{
	return onReadyToSendMetadata ? onReadyToSendMetadata(gcnew CliMetadata(metadata)) : false;
}

void ArtcCliChannel::NativeOnMetadataReceived(const IMetadataObserver::Metadata& metadata)
{
	if (onMetadataReceived)
		onMetadataReceived(gcnew CliMetadata(metadata));
}

bool ArtcCliChannel::NativeOnSendAudioPacket(ar::rtc::IPacketObserver::Packet& packet)
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

bool ArtcCliChannel::NativeOnSendVideoPacket(ar::rtc::IPacketObserver::Packet& packet)
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

bool ArtcCliChannel::NativeOnReceiveAudioPacket(ar::rtc::IPacketObserver::Packet& packet)
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

bool ArtcCliChannel::NativeOnReceiveVideoPacket(ar::rtc::IPacketObserver::Packet& packet)
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
