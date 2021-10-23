#pragma once
#include <vector>
#include "Externals/Glfw.h"
namespace Widgets
{
    class Widget
    {
    public:
        Widget();
        ~Widget();
        virtual void OnUpdate();
        virtual void AddWidget(Widget* widget);
    private:
        std::vector<Widget*> SubWidgets = {};
    };
}
