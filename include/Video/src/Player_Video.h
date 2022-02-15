#pragma once

#include <array>

#include "glad/glad.h"
#include "KHR/khrplatform.h"

#include "Datatype.h"
#include "Graphics.h"

namespace Player
{
    class Video
    {
    public:
        Video();
        ~Video();
        VideoError init(const DataType::Size& video_size);
        void update_data(const DataType::VideoFrame* frame);
        void draw();
    
    private:
        int terminate();
        void init_graphics(const DataType::Size& video_size);

        Graphics::Shader* shader = nullptr;
        Graphics::Vao* screen = nullptr;
        std::array<Graphics::Texture*, 3> textures = { nullptr };

    };
}

