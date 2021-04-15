#include "stdafx.h"
#include "ArtcCliPacketObserver.h"

using namespace ArtcCliLibrary;

bool ArtcCliPacketObserver::onSendAudioPacket(Packet & packet)
{
	if (onSendAudioPacketEvent) return onSendAudioPacketEvent(packet);
	return true;
}

bool ArtcCliPacketObserver::onSendVideoPacket(Packet & packet)
{
	if (onSendVideoPacketEvent) return onSendVideoPacketEvent(packet);
	return true;
}

bool ArtcCliPacketObserver::onReceiveAudioPacket(Packet & packet)
{
	if (onReceiveAudioPacketEvent) return onReceiveAudioPacketEvent(packet);
	return true;
}

bool ArtcCliPacketObserver::onReceiveVideoPacket(Packet & packet)
{
	if (onReceiveVideoPacketEvent) return onReceiveVideoPacketEvent(packet);
	return true;
}
