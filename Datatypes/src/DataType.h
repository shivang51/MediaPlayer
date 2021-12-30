#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../../vendors/ffmpeg.h"

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


namespace DataType
{

    typedef AVRational TimeBase;

    typedef std::shared_ptr<AVStream> Stream;

    typedef std::vector<Stream> Streams;

    typedef std::unique_ptr<AVCodecContext> StreamDecoder;

    typedef std::unique_ptr<AVPacket> Packet;

    typedef std::unique_ptr<AVFrame> Frame;

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
        TimeBase timebase = TimeBase();
    };
}

