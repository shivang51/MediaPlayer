#include "Player_Video.h"

namespace Player
{
    Video::Video()
    {
    }

    Video::~Video()
    {
        this->terminate();
    }

    VideoError Video::init(const DataType::Size& video_size)
    {
        this->init_graphics(video_size);

        return VideoError::None;
    }

    void Video::update_data(const DataType::VideoFrame* frame)
    {
        this->shader->Use();
        const int heights [3] = { frame->height, frame->height / 2, frame->height / 2 };

        for (int i = 0; i < 3; i++)
        {
            this->textures.at(i)->Bind();
            GlCall(
                glTexSubImage2D(GL_TEXTURE_2D, 0,
                0, 0,                               // xOffset, yOffset
                frame->yuv_size [i], heights [i],   // width, height
                GL_RED, GL_UNSIGNED_BYTE,           // version of pixel data, type of data
                frame->data.at(i).data())           // pointer to data
            );
        }
    }

    int Video::terminate()
    {
        delete this->shader;
        delete this->screen;
        for ( int i = 0; i < 3; i++ )
        {
            delete this->textures [i];
        }
        return 0;
    }

    void Video::draw()
    {
        this->shader->Use();
        this->screen->Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void Video::init_graphics(const DataType::Size& video_size)
    {
        this->screen = new Graphics::Vao(Graphics::Elements::vertices, Graphics::Elements::indices);
        this->shader = new Graphics::Shader();

        this->shader->Load("assets/shaders/yuv_rgb.glsl");

        this->shader->Use();
        this->shader->SendInt("texture_y", 0);
        this->shader->SendInt("texture_u", 1);
        this->shader->SendInt("texture_v", 2);

        for ( int i = 0; i < 3; i++ )
        {
            this->textures [i] = new Graphics::Texture(i);
            this->textures [i]->Bind();

            int width = (int)(( i == 0 ) ? video_size.width : video_size.width / 2);
            int height = (int)(( i == 0 ) ? video_size.height : video_size.height / 2);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (int) width, (int) height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
        }

    }

}
