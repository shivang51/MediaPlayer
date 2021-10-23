#pragma once

#include <string>

#include "src.h"

namespace Widgets
{
    class Button:public Widget
    {
    public:
        Button(Widget* parent, std::string text, int x, int y, int width, int height);
        ~Button();
        virtual void OnUpdate();
        virtual void OnClick();
        virtual void OnDoubleClick();
    private:
        int vertices [4] = { 20, 60, 80, 40 };
        int indicies [6] = { };
    };
}