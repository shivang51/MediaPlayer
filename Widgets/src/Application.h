#pragma once

#include "Window.h"
#include <vector>

class Application
{
public:
    static void windowClosed(Widgets::Window* window);
    static std::vector<Widgets::Window*> windows;

    Application();
    ~Application();
    void SetMainWindow(Widgets::Window* mainwindow);
    void AddWindow(Widgets::Window* window);
    void Run();
private:
    Widgets::Window* mainWindow = nullptr;
};