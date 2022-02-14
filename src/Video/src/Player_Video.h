#pragma once

#include "glad.h"

#include "Datatype.h"
#include "Graphics.h"

#include <array>

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

