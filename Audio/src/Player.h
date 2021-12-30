#pragma once

#include "../../vendors/portaudio.h"
#include "DataType.h"
#include <cstdint>

namespace Player
{

    class Audio
    {
    public:
        Audio();
        ~Audio();

        AudioError Init();
        void Close();

        void SetSampleRate(uint8_t sampleRate = 44100);

        struct UserData
        {
            size_t BufferSize;
        };

        void Play(DataType::AudioFrame* frame);
        void Stop();

    private:
        PaError err = NULL;
        bool intialized = false;

        PaStreamParameters outputParameters = PaStreamParameters();
        PaStream* stream = nullptr;
        UserData* userData = nullptr;

        uint8_t sampleRate = 44100;
        static int StreamCallback(const void* input, void* output,
            unsigned long frameCount,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void* userData);

    };
}