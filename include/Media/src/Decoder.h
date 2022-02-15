#pragma once

#ifndef MEDIA_DECODER_H
#define MEDIA_DECODER_H

#include <string>
#include <vector>
#include <stdexcept>

#include "Datatypes.h"

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
    class Decoder
    {
    public:
        Decoder();
        ~Decoder();

    public:
        /**
        * Stops all decoding process and terminates the decoder
        **/
        void TerminateDecoder();

        /**
        * Opens a file for decoding.
        * @param file: Object of type File which should have location of file to be opened.
        * @return Decoder::Error::None on success or returns a valid Error 
        **/
        Error OpenFile(DataType::File* file);

        /**
        * Closes a file during decoding.
        * @param file: Object of type File which should have location of file to be closed.
        * @return Decoder::Error::None on success or returns a valid Error
        **/
        void CloseFile(DataType::File* file = nullptr);

    public:
        /**
        * Gets the duration of file being decoded.
        * @return Duration or Length of currently opended file in seconds.
        **/
        float GetDuration();

        /**
        * Gets the best title for the opened file i.e. either embeded title or file name.
        * Embeded title is preffered over file name if title is embeded.
        * @returns Best title
        **/
        std::string GetBestFileTitle();

        /**
        * Gets media streams from the file
        * @param streamType: Type of streams
        * @returns A vector object with AVStreams as unique_ptr
        **/
        DataType::Streams GetStream(StreamType streamtype);

        /**
        * Gets the information of stream i.e. lanugage and title
        * @param stream: Media stream to extract information
        * @returns Object with language and title of stream
        **/
        DataType::StreamInfo GetStreamInfo(DataType::Stream* stream);

        /**
        * Seeks to some time in current open file
        * @param time: Time in seconds at which to seek
        **/
        void SeekTo(float time);

        /**
        * Sets current audio stream which need to be decoded.
        * @pram stream: audio stream
        **/
        void SetCurrentAudioStream(DataType::Stream* stream);


        /**
        * Sets current video stream which need to be decoded.
        * @pram stream: video stream
        **/
        void SetCurrentVideoStream(DataType::Stream* stream);

        /**
        * Gets the current frame from current media stream at given time. 
        * If time is not given frame is returned at current time.
        * @param time: time in seconds
        * @returns An audio frame
        **/
        DataType::AudioFrame GetAudioFrame(float time = -1.0f);

        /**
        * Gets the current frame from current media stream at given time. 
        * If time is not given frame is returned at current time.
        * @param time: time in seconds
        * @returns An audio frame
        **/
        DataType::VideoFrame GetVideoFrame(float time = -1.0f);

        /**
        * Gets audio frames till given time
        * @param n: time in seconds
        **/
        std::vector<DataType::AudioFrame> GetNAudioFrames(float n);

        /**
        * Gets video frames till given time
        * @param n: time in seconds
        **/
        std::vector<DataType::VideoFrame> GetNVideoFrames(float n);

        /**
        * Gets media frames till time n
        * @param n: time in seconds
        **/
        std::vector<DataType::MediaFrame> GetNFrames(float n);

        /**
        * Gets next immediate media frame.
        **/
        DataType::MediaFrame GetNextFrame();

        /**
        * Gets sample rate of current audio stream.
        **/
        int GetSampleRate();

        /**
        * Gets dimensions of video.
        **/
        DataType::Size GetVideoSize();

        /**
        * Gets aspect ratio (width / height)
        **/
        float GetAspectRatio();

    private:
        AVFormatContext* fileFormatCtx = nullptr;

        DataType::Packet* latestPacket = nullptr;
        DataType::Frame* latestFrame = nullptr;

        DataType::StreamDecoder* audioDecoder = nullptr;
        DataType::StreamDecoder* videoDecoder = nullptr;

        SwrContext* swrCtx = nullptr;
        SwsContext* swsCtx = nullptr;

        DataType::File openedfile = {};
        bool terminated = false;

        DataType::CurrentStream CurrentAudioStream = DataType::CurrentStream();
        DataType::CurrentStream CurrentVideoStream = DataType::CurrentStream();

        //Stores latest frame time in seconds
        float currentTime = 0.0f;

    private:
        StreamType GetStreamType(DataType::Stream* stream);

        Error SetStreamDecoder(DataType::Stream* stream, DataType::StreamDecoder** decoder);
        Error SetSwrContext(DataType::StreamDecoder* audioDecoder);
        Error SetSwsContext(DataType::StreamDecoder* videoDecoder);
        void FrameUnref(DataType::Frame* frame);
        void PacketUnref(DataType::Packet* packet);

        DecodingError DecodeAudioPacket(DataType::Packet* audioPacket, DataType::AudioFrame* audioFrame);

        DecodingError DecodeVideoPacket(DataType::Packet* videoPacket, DataType::VideoFrame* videoFrame);

    };
}

#endif