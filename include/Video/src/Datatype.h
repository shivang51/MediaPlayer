#pragma once
#include "Datatypes.h"

namespace Player
{
    enum class VideoError
    {
        None = 0,
        GLFW_Init,
        Window_Create,
        Glad_Init,
    };
}