#pragma once

#include <cstdint>
#include <stdexcept>

#include "portaudio.h"
#include "DataType.h"

namespace Player
{
    class Audio
    {
    public:
        Audio();
        ~Audio();

        /**
        Initialize the audio device for output.
        @param sampleRate: sample rate of audio stream (default = 44100)
        **/
        AudioError init_device(int sampleRate = 44100);

        /**
        Closes current open output device.
        **/
        void close_device();

        /**
        Plays the data passed in frame.
        @param frame: audio frame with data
        **/
        void play(DataType::AudioFrame* frame);

        /**
        Stops the playback immediately.
        **/
        void stop();

        /**
        Gets all the available audio devices
        **/
        std::vector<DataType::AudioDevice> get_devices();

    private:
        PaError err = NULL;
        static bool intialized;

        PaStreamParameters outputParameters = PaStreamParameters();
        PaStream* stream = nullptr;
    };
}