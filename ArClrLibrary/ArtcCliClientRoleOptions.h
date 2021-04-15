#pragma once
#include "ArtcCliLibraryEnum.h"

namespace ArtcCliLibrary {
	public ref class ArtcCliClientRoleOptions
	{
	public:
		EnumAudienceLatencyLevelType audienceLatencyLevel = EnumAudienceLatencyLevelType::AUDIENCE_LATENCY_LEVEL_LOW_LATENCY;

	public:
		operator ar::rtc::ClientRoleOptions() {
			ar::rtc::ClientRoleOptions raw;
			raw.audienceLatencyLevel = static_cast<ar::rtc::AUDIENCE_LATENCY_LEVEL_TYPE>(this->audienceLatencyLevel);
			return raw;
		}
	};
}

