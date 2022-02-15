#pragma once


#include <string>
#include <functional>
#include <thread>

#include "Gui.h"

#include "glfw/glfw3.h"
#include "KHR/khrplatform.h"

#include "Player.h"

class Application
{
public:
    Application();
    ~Application();
    void run();
    void run(const std::string& file);
    void quit();

    void play(const std::string& file);

private:

    void refresh();
    DataType::Size get_window_size();

    void swap_buffers();
    void update();

    void init_glfw_callbacks();
    void init_player_callbacks();

    static bool is_glfw_initialized;
    static bool is_glad_initialized;

    GLFWwindow* main_window = nullptr;
    Player::Player* player = nullptr;

    DataType::Size size = { 800, 600 };
    float video_aspect_ratio = 0.0f;

    std::thread render_thread{ };
    bool running = false;

    std::vector<int> texturePos = { };

    void render_func();

    Gui gui{ };

    //GLFW Callbacks
    void OnResize(const DataType::Size& _size);

    //Player Callbacks
    void OnFileLoad();
};

