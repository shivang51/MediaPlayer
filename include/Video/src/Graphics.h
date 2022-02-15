#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "glad/glad.h"
#include "Shaders.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Player::Graphics::Elements
{
    float vertices[ ];

    unsigned int indices[ ];

    namespace Shaders
    {
        const std::string BasicShader = basic_shader;
        const std::string YUV_RGB = yuv_rgb;
    }
}

namespace Player::Graphics
{
    class Vao
    {
    public:
        Vao();
        Vao(const float vertices[24], const GLuint indices [6]);
        ~Vao();
        void Bind();
        GLuint GetId();
    private:
        GLuint id = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
    };

    class Shader
    {
    public:
        Shader();
        Shader(const std::string& shader);
        Shader(const std::string& vertexShader, const std::string& fragmentShader);
        void Load(const std::string& file_location);
        void Load(const std::string& vertexShader, const std::string& fragmentShader);

        ~Shader();

        void Use();
        void Free();
        GLuint Program();

        void SendMat4(const std::string& name, const glm::mat4& matrix);
        void SendMat3(const std::string& name, const glm::mat3& matrix);
        void SendMat2(const std::string& name, const glm::mat2& matrix);
        void SendVec3f(const std::string& name, const glm::vec3& vec);
        void SendVec4f(const std::string& name, const glm::vec4& vec);
        void SendVec2f(const std::string& name, const glm::vec2& vec);
        void SendFloat(const std::string& name, float value);
        void SendInt(const std::string& name, int value);
    private:
        bool FileExist(const std::string& name);
        GLuint FragmentShader = 0;
        GLuint VertexShader = 0;
        GLuint program = 0;

        enum class ShaderType
        {
            None = -1,
            Vertex,
            Fragment
        };
    };

    class Texture
    {
    public:
        Texture(unsigned int ind = 0);
        ~Texture();

        void Bind();
        unsigned int GetId();

    private:
        unsigned int id = 0;
        unsigned int index = 0;
    };
};

