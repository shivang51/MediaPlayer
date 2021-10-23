#include "Application.h"

Application::Application()
{
}

Application::~Application()
{
}


std::vector<Widgets::Window*> Application::windows = {};

void Application::SetMainWindow(Widgets::Window* mainwindow)
{
    this->mainWindow = mainwindow;
    this->mainWindow->SetAsMain();
}

void Application::AddWindow(Widgets::Window * window)
{
    this->windows.emplace_back(window);
}

void Application::Run()
{
    while ( this->mainWindow->IsOpen() )
    {
        this->mainWindow->OnUpdate();
        this->mainWindow->HandleEvents();
        for ( auto win : this->windows )
        {
            win->OnUpdate();
            win->HandleEvents();
        }
    }
}

void Application::windowClosed(Widgets::Window* window)
{
    Application::windows.erase(std::remove(Application::windows.begin(), Application::windows.end(), window));
}
