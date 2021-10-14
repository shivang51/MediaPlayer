#pragma once

#include "FFmpeg.h"
#include <memory>
#include <string>
#include <vector>

/**
* Contains all the type definations, structs and error definations
**/

#ifndef MEDIA_TYPES
#define MEDIA_TYPES
//--------------- Enums ----------------------
namespace Media
{
    enum class Error
    {
        None = 0,
        FileNotFound,
        Unknown,
        StreamInfoNotFound,
        FileEnded
    };

    enum class StreamType
    {
        Audio = AVMEDIA_TYPE_AUDIO,
        Video = AVMEDIA_TYPE_VIDEO,
        Subtitle = AVMEDIA_TYPE_SUBTITLE
    };

    enum class DecodingError
    {
        InsufficientData = AVERROR(EAGAIN),
        None = 0
    };
}

//--------------- Type Definations -----------
namespace Media
{
    typedef std::shared_ptr<AVStream> Stream;
    typedef std::vector<Stream> Streams;

    typedef std::unique_ptr<AVCodecContext> StreamDecoder;

    typedef std::unique_ptr<AVPacket> Packet;
    typedef std::unique_ptr<AVFrame> Frame;

    typedef SwrContext* SwrCtx;
    typedef SwsContext* SwsCtx;

    typedef AVRational TimeBase;
}

//------------- Structs ----------------------
namespace Media{
    struct File
    {
        std::string location = "";
        bool open = false;
        std::string name()
        {
            size_t i = location.find_last_of("\\");
            return location.substr(i + 1);
        }
    };

    struct StreamInfo
    {
        std::string language = "";
        std::string title = "";
    };

    struct AudioFrame
    {
        std::vector<uint8_t> data = {};
        int buffersize = 0;
        float pts = 0.0f;
        Media::Error err = Media::Error::None;
    };

    struct VideoFrame
    {
        std::vector<uint8_t> data = {};
        Media::Error err = Media::Error::None;
    };

    struct CurrentStream
    {
        int index = -1;
        Media::TimeBase timebase = Media::TimeBase();
    };
}



//----------------- Type Creation -------------
namespace Media
{
    StreamDecoder MakeStreamDecoder(AVCodecContext* dec);
    SwsCtx MakeSwsCtx(SwsContext* ctx);
    void FrameUnref(const Frame& frame);
    void PacketUnref(const Packet& frame);
}

#endif