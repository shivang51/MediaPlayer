#include "Gui.h"

Gui::Gui()
{
}

Gui::~Gui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::init(GLFWwindow* window)
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Adding text font first fixed allignment!
    io.FontDefault = io.Fonts->AddFontFromFileTTF("assets\\fonts\\Roboto\\Roboto-Regular.ttf", 16.0f);

    ImFontConfig config;
    config.MergeMode = true;
    static const ImWchar mat_icon_ranges[ ] = { MaterialIcons::ICON_MIN_MD, MaterialIcons::ICON_MAX_MD, 0 };
    io.Fonts->AddFontFromFileTTF("assets\\icons\\MaterialIcons-Regular.ttf", 16.0f, &config, mat_icon_ranges);

    const ImWchar fa_icon_ranges[] = { FontAwesomeIcons::SIZE_MIN_FAB, FontAwesomeIcons::SIZE_MAX_FAB, 0 };
    io.Fonts->AddFontFromFileTTF("assets\\icons\\fa-brands-400.ttf", 16.0f, &config, fa_icon_ranges);

    static const ImWchar fa_icon_ranges_r[ ] = { FontAwesomeIcons::SIZE_MIN_FA, FontAwesomeIcons::SIZE_MAX_FA, 0 };
    //io.Fonts->AddFontFromFileTTF("Assets\\Icons\\fa-regular-400.ttf", 16.0f, &config, fa_icon_ranges_r);

    io.Fonts->AddFontFromFileTTF("assets\\icons\\fa-solid-900.ttf", 16.0f, &config, fa_icon_ranges_r);


    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;
    ImGui::StyleColorsDark(&style);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}


int Gui::get_menubar_height()
{
    return (int)this->menubar_size.y;
}

void Gui::set_duration(const float& duration)
{
    this->duration = duration;
    this->duration_str = format_time(duration);
}

void Gui::set_is_playing(bool value)
{
    this->playing = value;
    this->paused = !this->playing;
}

void Gui::set_player(Player::Player* player)
{
    this->player = player;

    auto time_change_fp = std::bind(&Gui::OnTimeChange, this, std::placeholders::_1, std::placeholders::_2);
    this->player->set_on_time_change(time_change_fp);

    auto void_fp = std::bind(&Gui::OnPlayStarts, this);
    this->player->set_on_play_starts(void_fp);

    void_fp = std::bind(&Gui::OnPlayStops, this);
    this->player->set_on_play_stops(void_fp);
}

void Gui::m_icon_button(const char* icon, ButtonClickCallback callback)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.8f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.8f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.8f, 0.8f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    ImGui::SetWindowFontScale(1.5f);
    auto size = ImGui::CalcTextSize(icon);
    if (ImGui::Button(icon, size))
    {
        if (callback)
            callback();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor((ImGuiMouseCursor)MouseCursors::Hand);
        auto text = this->playing ? this->paused ? "Play" : "Pause" : "No File Opened";
        ImGui::SetTooltip(text);
    }


    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor(4);
}

std::string Gui::m_open_file_dialog(const std::string& title, const std::string& filter_)
{
    OPENFILENAMEA ofn = { 0 };
    std::string filename;
    filename.resize(250);
    std::string filter;

    auto current_dir = std::filesystem::current_path();

    if (filter_ == "")
    {
        filter = "All\0*.*\0";
    }
    else
    {
        for (auto ch : filter_)
        {
            if (ch == '0')
                ch = '\0';
            filter += ch;
        }
    }


    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = (LPSTR)filename.c_str();
    ofn.nMaxFile = 250;
    ofn.lpstrFilter = filter.c_str();
    ofn.lpstrTitle = (LPCSTR)title.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT;

    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        SetCurrentDirectoryA((LPCSTR)current_dir.string().c_str());
        return filename;
    }
    else
    {
        switch (CommDlgExtendedError())
        {
        case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
        case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
        case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
        case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
        case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
        case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
        case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
        case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
        case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
        case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
        case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
        case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
        case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
        case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
        case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
        default: std::cout << "You cancelled.\n";
        }
    }
    return "";
}

void Gui::draw()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    this->draw_menubar();

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoScrollWithMouse;
    flags |= ImGuiWindowFlags_NoScrollbar;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImVec2 pos = { 0, viewport->Size.y - bottom_controls_height };

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize({viewport->Size.x, bottom_controls_height});
    ImGui::SetNextWindowBgAlpha(0.5);

    ImGui::Begin("bottom_panel", NULL, flags);

    this->draw_progress_bar();

    this->draw_bottom_controls();

    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::draw_menubar()
{
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File"))
    {
        temp_name = FontAwesomeIcons::FA_FOLDER_PLUS;
        temp_name += " Open";
        if (ImGui::MenuItem(temp_name.c_str()))
        {
            if (this->player != nullptr)
            {
                this->player->load_file(m_open_file_dialog("Select a media file"));
            }
        };
        ImGui::EndMenu();
    }
    menubar_size = ImGui::GetWindowSize();
    ImGui::EndMainMenuBar();
}

void Gui::draw_progress_bar()
{
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.8f, 0.8f, 0.8f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    ImGui::ProgressBar(this->progress, ImVec2(-1, 3.0f));
    ImGui::PopStyleColor(2);
}

void Gui::draw_bottom_controls()
{
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

    // play/pause button
    m_icon_button(paused ? MaterialIcons::PLAY_ARROW : MaterialIcons::PAUSE, cb_play_click);
    ImGui::SameLine();

    ImGui::Spacing();
    ImGui::SameLine();

    // current time label
    ImGui::Text(this->playing ? format_time(current_time).c_str() : "00:00");
    ImGui::SameLine();
    
    ImGui::Text("/");
    ImGui::SameLine();

    // duration label
    ImGui::Text(this->duration_str.c_str());
    ImGui::SameLine();

}

std::string Gui::format_time(float time_)
{
    std::string time_str = "";
    int time = (int)time_;
    //hh
    auto ff = int(time / 3600);
    time = time % 3600;
    if (ff > 0)
        time_str = (( ff < 10 ) ? "0" + std::to_string(ff) : std::to_string(ff)) + ":";
    //mm
    ff = int(time / 60);
    time = time % 60;
    time_str += ( ff < 10 ) ? "0" + std::to_string(ff) : std::to_string(ff);
    time_str += ":";
    //ss
    ff = time;
    time_str += ( ff < 10 ) ? "0" + std::to_string(ff) : std::to_string(ff);
    return time_str;
}

void Gui::OnPlayBtn_Click()
{
    if (this->playing)
    {
        if (this->paused)
        {
            this->player->unpause();
        }
        else
        {
            this->player->pause();
        }
        
        this->paused = !this->paused;
    }
}

void Gui::OnTimeChange(float current_time, float duration)
{
    this->current_time = current_time;
    this->progress = current_time / duration;
}

void Gui::OnPlayStarts()
{
    this->set_duration(this->player->get_duration());
    this->set_is_playing(true);
    this->paused = false;
}

void Gui::OnPlayStops()
{
    this->set_duration(0.0f);
    this->set_is_playing(false);
    this->current_time = 0.0f;
    this->progress = 0.0f;
}
