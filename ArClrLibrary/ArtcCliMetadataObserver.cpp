#include "stdafx.h"
#include "ArtcCliMetadataObserver.h"
using namespace ArtcCliLibrary;

int ArtcCliMetadataObserver::getMaxMetadataSize()
{
	if (onGetMaxMetadataSizeEvent) return onGetMaxMetadataSizeEvent();
	return 1024;
}

bool ArtcCliMetadataObserver::onReadyToSendMetadata(Metadata& metadata)
{
	if (onReadyToSendMetadataEvent) return onReadyToSendMetadataEvent(metadata);
	return false;
}

void ArtcCliMetadataObserver::onMetadataReceived(const Metadata& metadata)
{
	if (onMetadataReceivedEvent) return onMetadataReceivedEvent(metadata);
}
