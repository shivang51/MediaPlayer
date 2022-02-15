#include "Application.h"


bool Application::is_glfw_initialized = false;
bool Application::is_glad_initialized = false;

Application::Application()
{
    if (!is_glfw_initialized)
    {
        if (glfwInit() == GLFW_FALSE)
        {
            throw "Failed to init GLFW";
        }
        else
        {
            is_glfw_initialized = true;
        }
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 0);

    this->main_window = glfwCreateWindow((int)size.width, (int)size.height, "Video Player", NULL, NULL);

    if (!this->main_window)
    {
        throw "Failed to create Window";
    }

    glfwMakeContextCurrent(this->main_window);
    glfwSetWindowUserPointer(this->main_window, this);
    glfwSwapInterval(true);


    this->player = new Player::Player();
    this->init_player_callbacks();

    glfwMakeContextCurrent(nullptr);
}

Application::~Application()
{
    this->running = false;
    if (this->render_thread.joinable())
        this->render_thread.join();

    if (this->player)
        delete this->player;
    
    glfwSetWindowShouldClose(this->main_window, true);

    if (is_glfw_initialized)
    {
        if (this->main_window)
        {
            glfwDestroyWindow(main_window);
        }
        glfwTerminate();
    }
}


void Application::render_func()
{
    glfwMakeContextCurrent(this->main_window);

    if (!is_glad_initialized)
    {
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
            throw "Failed to Init Glad";
        }
        else
        {
            is_glad_initialized = true;
        }
    }

    gui.init(main_window);
    gui.set_player(player);

    while (this->running)
    {
        this->refresh();
    }
}


void Application::run()
{
    this->running = true;
    this->render_thread = std::thread(&Application::render_func, this);

    while (!is_glad_initialized);
    
    this->init_glfw_callbacks();

    while (!glfwWindowShouldClose(main_window))
    {
        glfwWaitEvents();
    }
}

void Application::run(const std::string& file)
{
    this->play(file);
    this->run();
}

void Application::quit()
{
    this->running = false;
    glfwSetWindowShouldClose(this->main_window, true);
}

void Application::play(const std::string& file)
{
    player->load_file(file);
}

void Application::refresh()
{
    this->update();
    
    if (this->player->is_playing())
    {
        this->player->update_frame();
        this->player->draw();
    }

    this->gui.draw();
    this->swap_buffers();
}

DataType::Size Application::get_window_size()
{
    int w, h;
    glfwGetWindowSize(this->main_window, &w, &h);
    return DataType::Size({ (size_t) w, (size_t) h });
}

void Application::swap_buffers()
{
    glfwSwapBuffers(this->main_window);
}

void Application::update()
{
    //glfwMakeContextCurrent(this->main_window);
    glViewport(texturePos [0], texturePos [1], texturePos [2], texturePos [3]);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Application::init_glfw_callbacks()
{
    glfwSetWindowSizeCallback(this->main_window, [] (GLFWwindow* win, int width, int height)
        {
            auto winClass = (Application*) glfwGetWindowUserPointer(win);
            winClass->OnResize({ (size_t) width, (size_t) height });
        });
    this->OnResize(size);

    glfwSetWindowCloseCallback(this->main_window, [] (GLFWwindow* win)
        {
            auto winClass = (Application*) glfwGetWindowUserPointer(win);
            winClass->running = false;
        });
}

void Application::init_player_callbacks()
{
    auto on_file_load_fp = std::bind(&Application::OnFileLoad, this);
    this->player->set_on_file_load(on_file_load_fp);
}

void Application::OnResize(const DataType::Size& _size)
{
    DataType::Size new_size;
    int menubar_height = gui.get_menubar_height();
    DataType::Size size = ( _size.width != 0 ) ? _size : this->get_window_size();
    size.height -= menubar_height;

    new_size.height = size.height;
    new_size.width = (size_t) ( size.height * video_aspect_ratio );

    if (new_size.width > size.width)
    {
        new_size.width = size.width;
        new_size.height = (size_t) ( size.width / video_aspect_ratio );
    }


    int offset_y = (int) ( ( size.height - new_size.height ) / 2 );
    int offset_x = (int) ( ( size.width - new_size.width ) / 2 );

    texturePos = { offset_x, offset_y, (int) new_size.width, (int) new_size.height };
}

void Application::OnFileLoad()
{
    glfwSetWindowTitle(main_window, player->get_file_title().c_str());
    this->video_aspect_ratio = player->get_video_ar();
    player->play();

    this->OnResize(get_window_size());
}
