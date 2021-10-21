#pragma once

#include <vector>

namespace Player
{
    enum class AudioError
    {
        FailedToInit = 0,
        None,
        AreadyInitialized
    };

    struct Frame
    {
        std::vector<uint8_t> data = { };
    };
}