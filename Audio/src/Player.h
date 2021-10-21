#pragma once

#include "portaudio.h"

#include "Types.h"

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

    private:
        PaError err = NULL;
        bool intialized = false;

        PaStreamParameters outputParameters = PaStreamParameters();
        PaStream* stream = nullptr;

        uint8_t sampleRate = 44100;
        static int StreamCallback(const void* input, void* output,
            unsigned long frameCount,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void* userData);

    };
}