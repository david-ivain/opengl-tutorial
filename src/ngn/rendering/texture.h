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

struct TextureOptions {
    std::string path;
    TextureType::Value type;
};

class Texture {
public:
    Texture(const std::string& path, TextureType::Value type);
    ~Texture();
    Texture(Texture&&);

    Texture(const Texture&) = delete;

    unsigned id() const;
    TextureType::Value type() const;
    const std::string& path() const;

private:
    unsigned id_;
    TextureType::Value type_;
    std::string path_;
};

}
