#include "Window.h"
#include "Application.h"
#include <iostream>

bool Widgets::Window::GlfwInit = false;
int Widgets::Window::WindowCount = 0;

Widgets::Window::Window()
{
    if ( !GlfwInit )
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        this->GlfwInit = true;
    }
}

Widgets::Window::~Window()
{
    if ( this->WindowCount == 0 )
    {
        glfwTerminate();
        this->GlfwInit = false;
    }
}

void Widgets::Window::setGlfwEvents()
{
    glfwSetWindowCloseCallback(this->GetWindowID(), [] (GLFWwindow* win) { 
        auto winClass = (Window*)glfwGetWindowUserPointer(win);
        winClass->Close();
    });
}

void Widgets::Window::Create(std::string name, int width, int height)
{   
    this->window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    this->WindowCount++;
    glfwSetWindowUserPointer(this->window, this);
    glfwMakeContextCurrent(this->window);
    this->setGlfwEvents();


    if ( glewInit() != GLEW_OK )
    {
        std::cout << "Failed to Intialize GLEW" << std::endl;
        this->Close();
    }

    glViewport(0, 0, width, height);
    this->isOpen = true;
}

void Widgets::Window::Close()
{
    if ( this->isMain )
    {
        if ( this->WindowCount == 1 )
        {
            this->isOpen = false;
            this->WindowCount--;
        }
    }
    else if(WindowCount != 0)
    {
        Application::windowClosed(this);
        glfwDestroyWindow(this->GetWindowID());
        this->WindowCount--;
    }
    
}

bool Widgets::Window::IsOpen()
{
    return this->isOpen;
}

void Widgets::Window::OnUpdate()
{
    glfwMakeContextCurrent(this->window);
    glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Widgets::Window::HandleEvents()
{
    glfwPollEvents();
}

Widgets::WindowID Widgets::Window::GetWindowID()
{
    return this->window;
}

void Widgets::Window::Swapbuffers()
{
    glfwSwapBuffers(this->window);
}

void Widgets::Window::SetBackgroundColor(Color color)
{
    this->bgColor = color;
}

void Widgets::Window::SetAsMain()
{
    this->isMain = true;
}

void Widgets::Window::UpdateSubWidgets()
{
    Widget::OnUpdate();
}
