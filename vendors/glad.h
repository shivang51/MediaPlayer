#pragma once
#include <string>
#include <iostream>
extern "C" {
#include ".\\glad\\include\\glad\\glad.h"
#include ".\\glad\\include\\KHR\\khrplatform.h"
}

#ifndef GlDebugFunc
#define GlDebugFunc

static void GlClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GlLogCall(const char* file, int line)
{
    while (GLenum err = glGetError())
    {
        std::cout << "[ OpenGL Error ] " << file << " (" << line << ") " << err << std::endl;
        return false;
    }
    return true;
}

#define ASSERT(x) if(!(x)) __debugbreak();

#define GlCall(func) GlClearError(); func ; ASSERT(GlLogCall(__FILE__, __LINE__))

#endif