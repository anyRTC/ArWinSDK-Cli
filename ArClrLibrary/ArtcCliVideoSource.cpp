#include "ArtcCliVideoSource.h"

//ArtcCliLibrary::CliVideoSource::CliVideoSource() : raw(new VideoSource())
//{
//}

ArtcCliLibrary::InnerVideoSource::InnerVideoSource(CliVideoSource^ clr) : 
    clr(clr),
    raw(new VideoSource()),
    gchs(gcnew List<GCHandle>())
{
    this->linkCliToRaw();
}

ArtcCliLibrary::InnerVideoSource::~InnerVideoSource()
{
    dispose();
}

ArtcCliLibrary::InnerVideoSource::!InnerVideoSource()
{
    dispose();
}

void ArtcCliLibrary::InnerVideoSource::dispose()
{
    delete raw;
    for each (GCHandle handle in gchs) handle.Free();
}

void ArtcCliLibrary::InnerVideoSource::linkCliToRaw()
{
    this->raw->onInitializeEvent = static_cast<VideoSource::OnInitialize::Pointer>(regEvent(gcnew VideoSource::OnInitialize::Type(this, &InnerVideoSource::NativeOnInitialize)));
    this->raw->onDisposeEvent = static_cast<VideoSource::OnDispose::Pointer>(regEvent(gcnew VideoSource::OnDispose::Type(this, &InnerVideoSource::NativeOnDispose)));
    this->raw->onStartEvent = static_cast<VideoSource::OnStart::Pointer>(regEvent(gcnew VideoSource::OnStart::Type(this, &InnerVideoSource::NativeOnStart)));
    this->raw->onStopEvent = static_cast<VideoSource::OnStop::Pointer>(regEvent(gcnew VideoSource::OnStop::Type(this, &InnerVideoSource::NativeOnStop)));
    raw->getBufferTypeEvent = static_cast<VideoSource::GetBufferType::Pointer>(regEvent(gcnew VideoSource::GetBufferType::Type(this, &InnerVideoSource::NativeGetBufferType)));
    raw->getVideoCaptureTypeEvent = static_cast<VideoSource::GetVideoCaptureType::Pointer>(regEvent(gcnew VideoSource::GetVideoCaptureType::Type(this, &InnerVideoSource::NativeGetVideoCaptureType)));
    raw->getVideoContentHintEvent = static_cast<VideoSource::GetVideoContentHint::Pointer>(regEvent(gcnew VideoSource::GetVideoContentHint::Type(this, &InnerVideoSource::NativeGetVideoContentHint)));

       
}

void* ArtcCliLibrary::InnerVideoSource::regEvent(Object^ obj)
{
    gchs->Add(GCHandle::Alloc(obj));
    return Marshal::GetFunctionPointerForDelegate(obj).ToPointer();
}

bool ArtcCliLibrary::InnerVideoSource::NativeOnInitialize()
{
    return clr->onInitialize();
}

void ArtcCliLibrary::InnerVideoSource::NativeOnDispose()
{
    clr->onDispose();
}

bool ArtcCliLibrary::InnerVideoSource::NativeOnStart()
{
    return clr->onStart();
}

void ArtcCliLibrary::InnerVideoSource::NativeOnStop()
{
    clr->onStop();
}

ExternalVideoFrame::VIDEO_PIXEL_FORMAT ArtcCliLibrary::InnerVideoSource::NativeGetBufferType()
{
    return static_cast<ExternalVideoFrame::VIDEO_PIXEL_FORMAT>(clr->getBufferType());
}

bool ArtcCliLibrary::VideoSource::onInitialize(IVideoFrameConsumer* consumer)
{
    this->consumer = consumer;
    if (this->onInitializeEvent) this->onInitializeEvent();
    else return false;
	return true;
}

void ArtcCliLibrary::VideoSource::onDispose()
{
    if (this->onDisposeEvent) this->onDisposeEvent();
}

bool ArtcCliLibrary::VideoSource::onStart()
{
    if (this->onStartEvent) return this->onStartEvent();
    else return false;
}

void ArtcCliLibrary::VideoSource::onStop()
{
    if (this->onStopEvent) this->onStopEvent();
}

ExternalVideoFrame::VIDEO_PIXEL_FORMAT ArtcCliLibrary::VideoSource::getBufferType()
{
    if (this->getBufferTypeEvent) return this->getBufferTypeEvent();
    else return ExternalVideoFrame::VIDEO_PIXEL_FORMAT::VIDEO_PIXEL_NV21;
}

VIDEO_CAPTURE_TYPE ArtcCliLibrary::VideoSource::getVideoCaptureType()
{
    if (this->getVideoCaptureTypeEvent) return this->getVideoCaptureTypeEvent();
    else return VIDEO_CAPTURE_TYPE::VIDEO_CAPTURE_CAMERA;
}

VideoContentHint ArtcCliLibrary::VideoSource::getVideoContentHint()
{
    if (this->getVideoContentHintEvent) return this->getVideoContentHintEvent();
    else return VideoContentHint::CONTENT_HINT_MOTION;
}

void ArtcCliLibrary::VideoSource::Consume(const unsigned char* buffer, ExternalVideoFrame::VIDEO_PIXEL_FORMAT frameType, int width, int height, int rotation, long timestamp)
{
    this->consumer->consumeRawVideoFrame(buffer, frameType, width, height, rotation, timestamp);
}

VIDEO_CAPTURE_TYPE ArtcCliLibrary::InnerVideoSource::NativeGetVideoCaptureType()
{
    return (VIDEO_CAPTURE_TYPE)(this->clr->getVideoCaptureType());
}

VideoContentHint ArtcCliLibrary::InnerVideoSource::NativeGetVideoContentHint()
{
    return (VideoContentHint)(clr->getVideoContentHint());
}

void ArtcCliLibrary::InnerVideoSource::Consume(array<Byte>^ buffer, EnumVideoPixelFormate frameType, int width, int height, int rotation, long timestamp)
{
    auto rawBuffer = new unsigned char[buffer->Length]{};
    Marshal::Copy(buffer, 0, IntPtr(rawBuffer), buffer->Length);
    this->raw->Consume(
        rawBuffer, static_cast<ExternalVideoFrame::VIDEO_PIXEL_FORMAT>(frameType), 
        width, height, rotation, timestamp
    );
    delete[] rawBuffer;
}
