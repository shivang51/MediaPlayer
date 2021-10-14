#include "Types.h"

namespace Media
{
    StreamDecoder MakeStreamDecoder(AVCodecContext* dec)
    {
        return std::unique_ptr<AVCodecContext>(dec);
    }

    SwsCtx MakeSwsCtx(SwsContext* ctx)
    {
        //return std::unique_ptr<SwsContext>(ctx);
        return ctx;
    }

    void FrameUnref(const Frame& frame)
    {
        auto buff = frame.get();
        av_frame_unref(buff);
    }

    void PacketUnref(const Packet& packet)
    {
        auto buff = packet.get();
        av_packet_unref(buff);
    }
}