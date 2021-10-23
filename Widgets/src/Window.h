#pragma once

#include <string>
#include <functional>

#include "Types.h"
#include "Externals/Glfw.h"
#include "Widget.h"

namespace Widgets
{
    class Window : public Widget
    {
    public:

        static bool GlfwInit;
        static int WindowCount;
        Window();
        ~Window();

        void Create(std::string name, int width, int height);
        void Close();
        bool IsOpen();

        WindowID GetWindowID();

    public:
        virtual void OnUpdate();
        virtual void HandleEvents();
    public:
        void Swapbuffers();
        void SetBackgroundColor(Color color);
        void SetAsMain();
        void UpdateSubWidgets();
            
    private:
        GLFWwindow* window = nullptr;
        Color bgColor = Color();
        void setGlfwEvents();
        bool isOpen = false;
        bool isMain = false;
    };
}
