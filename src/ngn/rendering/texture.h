#pragma once

#include <string>
namespace ngn {

class TextureType {
public:
    enum Value : int {
        Diffuse,
        Specular,
        Emission,
    };

    static std::string to_string(TextureType::Value);

private:
};

class Texture {
public:
    Texture(const std::string& path, TextureType::Value type);
    ~Texture();
    Texture(Texture&&);

    Texture(const Texture&) = delete;

    unsigned id() const;
    TextureType::Value type() const;

private:
    unsigned id_;
    TextureType::Value type_;
};

}
