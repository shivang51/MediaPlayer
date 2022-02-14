#include "Graphics.h"
#include <gl/GL.h>

namespace Player::Graphics::Elements
{
    float vertices[ ] = {
        // position  // texture coords
         1.0f,  1.0f,   1.0f, 0.0f, // top right
         1.0f, -1.0f,   1.0f, 1.0f, // bottom right
        -1.0f, -1.0f,   0.0f, 1.0f, // bottom left
        -1.0f,  1.0f,   0.0f, 0.0f  // top left
    };

    GLuint indices[ ] = {
        0, 1, 2,  // first triangle
        0, 2, 3,  // second triangle
    };

}

//VAO
namespace Player::Graphics
{
    Vao::Vao()
    {
    }

    Vao::Vao(const float vertices[16], const GLuint indices[6])
    {
        glGenVertexArrays(1, &id);
        glBindVertexArray(id);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) ( 2 * sizeof(float) ));
        glEnableVertexAttribArray(1);
    }

    Vao::~Vao()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &id);
    }

    void Vao::Bind()
    {
        glBindVertexArray(id);
    }

    GLuint Vao::GetId()
    {
        return this->id;
    }
}

//Shader
namespace Player::Graphics
{
    Shader::Shader()
    { }

    bool Shader::FileExist(const std::string& name)
    {
        struct stat buffer;
        return ( stat(name.c_str(), &buffer) == 0 );
    }

    Shader::Shader(const std::string& shader)
    {
        std::string line = "";
        auto type = ShaderType::None;
        std::string vertexShader = "";
        std::string fragmentShader = "";

        auto fragmentPos = shader.find("//#fragmentShader");
        vertexShader = shader.substr(0, fragmentPos);
        fragmentShader = shader.substr(fragmentPos, shader.size());
        this->Load(vertexShader, fragmentShader);
    }

    Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
    {
        this->Load(vertexShader, fragmentShader);
    }

    void Shader::Load(const std::string& file_location)
    {
        std::string line = "";
        auto type = ShaderType::None;
        std::string VertexShaderSrc = "";
        std::string FragShaderSrc = "";

        if (FileExist(file_location))
        {
            auto filestream = std::ifstream(file_location.c_str());
            while (std::getline(filestream, line))
            {
                if (std::strcmp(line.c_str(), "//#vertexShader") == 0)
                {
                    type = ShaderType::Vertex;
                }
                else if (std::strcmp(line.c_str(), "//#fragmentShader") == 0)
                {
                    type = ShaderType::Fragment;
                }
                else
                {
                    if (type == ShaderType::Vertex)
                    {
                        VertexShaderSrc.append(line);
                        VertexShaderSrc.append("\n");
                    }
                    else if (type == ShaderType::Fragment)
                    {
                        FragShaderSrc.append(line);
                        FragShaderSrc.append("\n");
                    }
                }
            }
            filestream.close();

            Load(VertexShaderSrc, FragShaderSrc);
        }
        else
        {
            std::cout << "current working dir = " << std::filesystem::current_path() << std::endl;
            std::cout << "shader file not found " + file_location << std::endl;
        }
    }

    void Shader::Load(const std::string& vertexShader, const std::string& fragmentShader)
    {
        const char* VSSrc = vertexShader.c_str();
        const char* FSSrc = fragmentShader.c_str();

        VertexShader = glCreateShader(GL_VERTEX_SHADER);
        FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(VertexShader, 1, &VSSrc, NULL);
        glCompileShader(VertexShader);

        int success;
        char infoLog [512];
        glGetShaderiv(this->VertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(this->VertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        glShaderSource(this->FragmentShader, 1, &FSSrc, NULL);
        glCompileShader(this->FragmentShader);

        glGetShaderiv(this->FragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(this->FragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        this->program = glCreateProgram();

        glAttachShader(this->program, this->VertexShader);
        glAttachShader(this->program, this->FragmentShader);
        glLinkProgram(this->program);

        glGetProgramiv(this->program, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(this->program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }

    Shader::~Shader()
    {
        glDeleteShader(this->VertexShader);
        glDeleteShader(this->FragmentShader);
        glDeleteProgram(this->program);
    }

    void Shader::Use()
    {
        glUseProgram(this->program);
    }

    void Shader::Free()
    {
        glUseProgram(0);
    }

    GLuint Shader::Program()
    {
        return this->program;
    }
    void Shader::SendMat4(const std::string& name, const glm::mat4& matrix)
    {
        auto n = glGetUniformLocation(this->program, name.c_str());
        glUniformMatrix4fv(glGetUniformLocation(this->program, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void Shader::SendMat3(const std::string& name, const glm::mat3& matrix)
    {
        glUniformMatrix3fv(glGetUniformLocation(this->program, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void Shader::SendMat2(const std::string& name, const glm::mat2& matrix)
    {
        glUniformMatrix2fv(glGetUniformLocation(this->program, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void Shader::SendVec3f(const std::string& name, const glm::vec3& vec)
    {
        glUniform3f(glGetUniformLocation(this->program, name.c_str()), vec.x, vec.y, vec.z);
    }
    void Shader::SendVec4f(const std::string& name, const glm::vec4& vec)
    {
        glUniform4f(glGetUniformLocation(this->program, name.c_str()), vec.x, vec.y, vec.z, vec.w);
    }
    void Shader::SendVec2f(const std::string& name, const glm::vec2& vec)
    {
        glUniform2f(glGetUniformLocation(this->program, name.c_str()), vec.x, vec.y);
    }
    void Shader::SendFloat(const std::string& name, float value)
    {
        glUniform1f(glGetUniformLocation(this->program, name.c_str()), value);
    }
    void Shader::SendInt(const std::string& name, int value)
    {
        glUniform1i(glGetUniformLocation(this->program, name.c_str()), value);
    }
}

//Texture
namespace Player::Graphics
{
    Texture::Texture(unsigned int index)
    {
        this->index = index;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &id);
    }

    void Texture::Bind()
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    unsigned int Texture::GetId()
    {
        return this->id;
    }
}