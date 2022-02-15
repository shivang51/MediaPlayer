#pragma once


#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <windows.h>
#include <filesystem>

#include "Player.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include "glfw/glfw3.h"

#include "IconsMaterialDesign.h"
#include "IconsFontAwesome5.h"

namespace Fonts
{
    static const std::string Roboto = "Assets\\Fonts\\Roboto\\Roboto-Regular.ttf";
    static const std::string Material_Icons = "Assets\\Icons\\MaterialIcons-Regular.ttf";
    static const std::string FontAwesome_Icons = "Assets\\Icons\\MaterialIcons-Regular.ttf";
}

class Gui
{
public:
    typedef std::function<void()> ButtonClickCallback;

public:
    Gui();
    ~Gui();

    void init(GLFWwindow* window);
    void draw();

    int get_menubar_height();

    void set_duration(const float& duration);
    void set_is_playing(bool value);
    void set_player(Player::Player* player);

private:
    // Self Made Gui Controls
    void m_icon_button(const char* icon, ButtonClickCallback callback = nullptr);

    std::string m_open_file_dialog(const std::string& title, const std::string& filter_ = "");

private:
    void draw_menubar();
    void draw_progress_bar();
    void draw_bottom_controls();

    /**
    * Converts time in hh:mm:ss format
    * @param time: time in seconds 
    **/
    std::string format_time(float time);

    ImVec2 menubar_size{ };

    const float bottom_controls_height = 60;

    float progress = 0.0f;

    float current_time = 0.0f;
    float duration = 0.0f;
    std::string duration_str = "00:00";

    bool playing = false;
    bool paused = true;

    std::string temp_name = "";
    Player::Player* player = nullptr;

    enum class MouseCursors
    {
        Arrow = 0, TextInput, ResizeAll, ResizeNS, ResizeEW, ResizeNESW, ResizeNWSE, Hand, NotAllowed
    };

private:

    // Play/Pause Click event callback
    void OnPlayBtn_Click();
    ButtonClickCallback cb_play_click = std::bind(&Gui::OnPlayBtn_Click, this);

    // Player class callbacks
    void OnTimeChange(float current_time, float duration);
    void OnPlayStarts();

};

