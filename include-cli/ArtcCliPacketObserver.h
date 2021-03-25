#pragma once
#include "../ARtcKit/IArRtcEngine.h"
using namespace ar::rtc;

namespace ArtcCliLibrary {

	typedef bool(__stdcall * PFOnSendAudioPacket)(IPacketObserver::Packet & packet);
	typedef bool(__stdcall * PFOnSendVideoPacket)(IPacketObserver::Packet & packet);
	typedef bool(__stdcall * PFOnReceiveAudioPacket)(IPacketObserver::Packet & packet);
	typedef bool(__stdcall * PFOnReceiveVideoPacket)(IPacketObserver::Packet & packet);	

	//Native delegate
	delegate bool NativeOnSendAudioPacketDelegate(IPacketObserver::Packet & packet);
	delegate bool NativeOnSendVideoPacketDelegate(IPacketObserver::Packet & packet);
	delegate bool NativeOnReceiveAudioPacketDelegate(IPacketObserver::Packet & packet);
	delegate bool NativeOnReceiveVideoPacketDelegate(IPacketObserver::Packet & packet);

	public class ArtcCliPacketObserver : public IPacketObserver {
	public:
		virtual ~ArtcCliPacketObserver() = default;
		PFOnSendAudioPacket onSendAudioPacketEvent = 0;
		PFOnSendVideoPacket onSendVideoPacketEvent = 0;
		PFOnReceiveAudioPacket onReceiveAudioPacketEvent = 0;
		PFOnReceiveVideoPacket onReceiveVideoPacketEvent = 0;

		// Inherited via IPacketObserver
		bool onSendAudioPacket(Packet & packet) override;
		bool onSendVideoPacket(Packet & packet) override;
		bool onReceiveAudioPacket(Packet & packet) override;
		bool onReceiveVideoPacket(Packet & packet) override;
	};
}

