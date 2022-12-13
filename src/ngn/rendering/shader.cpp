#include "shader.h"

#include "../utils/log.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

constexpr auto SHADER_LOG_SIZE = 512;

namespace ngn {
Shader::Shader(const std::string& vertex_path, const std::string& fragment_path)
    : ID_(glCreateProgram())
{
    std::string vertex_source;
    std::string fragment_source;
    std::ifstream vertex_file;
    std::ifstream fragment_file;

    vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vertex_file.open(vertex_path);
        fragment_file.open(fragment_path);

        std::stringstream vertex_stream, fragment_stream;

        vertex_stream << vertex_file.rdbuf();
        fragment_stream << fragment_file.rdbuf();

        vertex_file.close();
        fragment_file.close();

        vertex_source = vertex_stream.str();
        fragment_source = fragment_stream.str();
    } catch (std::ifstream::failure e) {
        LOGERR("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
        glDeleteProgram(ID_);
        return;
    }
    const char* vertex_source_ptr = vertex_source.c_str();
    const char* fragment_source_ptr = fragment_source.c_str();

    int success;
    char info_log[SHADER_LOG_SIZE];

    unsigned vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source_ptr, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, SHADER_LOG_SIZE, NULL, info_log);
        LOGERRF("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", info_log);
    };

    unsigned fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source_ptr, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, SHADER_LOG_SIZE, NULL, info_log);
        LOGERRF("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", info_log);
    };

    glAttachShader(ID_, vertex);
    glAttachShader(ID_, fragment);
    glLinkProgram(ID_);
    glGetProgramiv(ID_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID_, SHADER_LOG_SIZE, NULL, info_log);
        LOGERRF("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", info_log);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    glDeleteProgram(ID_);
}

void Shader::use() const
{
    glUseProgram(ID_);
}

template <>
void Shader::set(const std::string& name, glm::vec4 value) const
{
    int vertexColorLocation = glGetUniformLocation(ID_, name.c_str());
    glUniform4fv(vertexColorLocation, 1, glm::value_ptr(value));
}
}
