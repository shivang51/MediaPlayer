#pragma once

#ifndef MEDIA_DECODER_H
#define MEDIA_DECODER_H

#include <memory>
#include <string>
#include <vector>

#include "FFmpeg.h"
#include "Types.h"

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
        Media::Error OpenFile(Media::File* file);

        /**
        * Closes a file during decoding.
        * @param file: Object of type File which should have location of file to be closed.
        * @return Decoder::Error::None on success or returns a valid Error
        **/
        void CloseFile(Media::File* file = nullptr);

    public:
        /**
        * Gets the duration of file being decoded.
        * @return Duration or Length of currently opended file in seconds.
        **/
        float GetCurrentFileDuration();

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
        Media::Streams GetStream(Media::StreamType streamtype);

        /**
        * Gets the information of stream i.e. lanugage and title
        * @param stream: Media stream to extract information
        * @returns Object with language and title of stream
        **/
        Media::StreamInfo GetStreamInfo(Media::Stream& stream);

        /**
        * Seeks to some time in current open file
        * @param time: Time in seconds at which to seek
        **/
        void SeekTo(float time);

        void SetCurrentAudioStream(Media::Stream& stream);
        void SetCurrentVideoStream(Media::Stream& stream);

        /**
        * Gets the current frame from current media stream given time
        * @param time: time in seconds
        * @returns An audio frame
        **/
        AudioFrame GetAudioFrame(float time = -1.0f);

        uint8_t GetSampleRate();

    private:
        std::shared_ptr<AVDictionary*> mediaOptions = std::make_shared<AVDictionary*>();
        std::unique_ptr<AVFormatContext> fileFormatCtx = std::make_unique<AVFormatContext>();

        Media::Packet latestPacket = std::make_unique<AVPacket>();
        Media::Frame latestFrame = std::make_unique<AVFrame>();

        Media::StreamDecoder audioDecoder = std::make_unique<AVCodecContext>();
        Media::StreamDecoder videoDecoder = std::make_unique<AVCodecContext>();

        Media::SwrCtx swrCtx = nullptr;
        Media::SwsCtx swsCtx = nullptr;

        File openedfile = {};
        bool terminated = false;

        Media::CurrentStream CurrentAudioStream = Media::CurrentStream();
        Media::CurrentStream CurrentVideoStream = Media::CurrentStream();

        float currentTime = 0.0f;

    private:
        Media::StreamType GetStreamType(Media::Stream& stream);

        Media::Error SetStreamDecoder(Media::Stream& stream, Media::StreamDecoder& decoder);
        Media::Error SetSwrContext(Media::StreamDecoder& audioDecoder);
        Media::Error SetSwsContext(Media::StreamDecoder& videoDecoder);

        Media::DecodingError DecodeAudioPacket(Media::Packet& audioPacket, Media::AudioFrame& audioFrame);

    };
}

#endif