#pragma once
#include "Externals/Glfw.h"
namespace Widgets
{
    typedef GLFWwindow* WindowID;
    struct LocSize
    {
        int x = 0;
        int y = 0;
        int height = 30;
        int widht = 50;
    };

    struct Color
    {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 1.0f;
    };

    enum class BitFields
    {
        ClearColor = GL_COLOR_BUFFER_BIT
    };

    struct Size
    {
        int width = 0;
        int height = 0;
    };
}