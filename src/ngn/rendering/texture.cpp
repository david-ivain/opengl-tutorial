#include "texture.h"

#include "../utils/log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>

namespace ngn {

std::string TextureType::to_string(TextureType::Value type)
{
    switch (static_cast<int>(type)) {
    case Diffuse:
        return "diffuse";
    case Specular:
        return "specular";
    case Emission:
        return "emission";
    default:
        return "diffuse";
    }
}

Texture::Texture(const std::string& path, TextureType::Value type)
    : type_(type)
    , path_(path)
{
    // Texture loading
    stbi_set_flip_vertically_on_load(true);
    int width, height, number_of_channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &number_of_channels, 0);
    if (!data) {
        LOGERR("Failed to load image.");
        throw;
    }

    // Generate Texture
    glGenTextures(1, &id_);

    // Bind Texture
    glBindTexture(GL_TEXTURE_2D, id_);

    // Texture repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // For GL_CLAMP_TO_BORDER
    // float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load Texture
    unsigned color_mode = number_of_channels == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, color_mode, width, height, 0, color_mode, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free Image data
    stbi_image_free(data);
    LOGF("Texture %u created.", id_);
}

unsigned Texture::id() const
{
    return id_;
}

TextureType::Value Texture::type() const
{
    return type_;
}

const std::string& Texture::path() const
{
    return path_;
}

TexturePool TexturePool::instance_ {};

TexturePool::~TexturePool()
{
    for (auto& texture : textures_) {
        LOGF("Texture %u deleted.", texture.id());
        glDeleteTextures(1, &texture.id_);
    }
}

Texture TexturePool::instance_load(const std::string& path, TextureType::Value type)
{
    auto existing_texture = std::find_if(textures_.begin(), textures_.end(),
        [path](const Texture& texture) {
            return texture.path_ == path;
        });
    if (existing_texture != textures_.end())
        return *existing_texture;
    textures_.push_back({ path, type });
    return textures_.back();
}

}
