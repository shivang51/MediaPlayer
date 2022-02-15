#pragma once

#include <vector>
#include <string>
#include <memory>

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavdevice/avdevice.h"
    #include "libavformat/avformat.h"
    #include "libavutil/avutil.h"
    #include "libswresample/swresample.h"
    #include "libswscale/swscale.h"
}

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

    enum class DeviceType
    {
        None = 0,
        Input,
        Output,
        In_Out
    };

    enum class FrameType
    {
        None = 0,
        Audio,
        Video
    };
}


namespace DataType
{

    typedef AVRational TimeBase;
    typedef AVFrame Frame;
    typedef AVPacket Packet;
    typedef AVStream Stream; 
    typedef AVCodecContext StreamDecoder;
    typedef std::vector<Stream*> Streams;

    struct File
    {
        std::string location = "";
        std::string title = "";
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
        std::vector<int16_t> data = {};
        int buffersize = 0;
        float pts = 0.0f;
        Media::Error err = Media::Error::None;
    };

    struct VideoFrame
    {
        std::vector<std::vector<uint8_t>> data = {};
        Media::Error err = Media::Error::None;
        float pts = 0.0f;
        std::vector<int> yuv_size = { };
        int width = 0;
        int height = 0;
    };

    struct CurrentStream
    {
        int index = -1;
        TimeBase timebase = TimeBase();
    };

    struct AudioDevice
    {
        std::string name = "";
        Media::DeviceType type = Media::DeviceType::None;
        int in_channels = 0;
        int out_channels = 0;

        std::string TypeStr()
        {
            std::vector<std::string> types = {"None", "Input", "Ouput", "Input_Output" };
            return types[(int)type];
        }
    };

    struct MediaFrame
    {
        float pts = 0.0f;
        Media::FrameType type = Media::FrameType::None;
        Media::Error err = Media::Error::None;
        AudioFrame audioFrame = AudioFrame();
        VideoFrame videoFrame = VideoFrame();
    };

    struct Size {
        size_t width = 0;
        size_t height = 0;
    };
}

