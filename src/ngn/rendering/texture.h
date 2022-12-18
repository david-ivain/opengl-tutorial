#pragma once

#include <string>
#include <vector>
namespace ngn {

class TexturePool;

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
    ~Texture() = default;
    Texture(const Texture&) = default;

    unsigned id() const;
    TextureType::Value type() const;
    const std::string& path() const;

private:
    Texture(const std::string& path, TextureType::Value type);

    unsigned id_;
    TextureType::Value type_;
    std::string path_;

    friend TexturePool;
};

class TexturePool {
public:
    TexturePool(const TexturePool&) = delete;
    TexturePool(TexturePool&&) = delete;

    static inline Texture load(const std::string& path, TextureType::Value type)
    {
        return instance_.instance_load(path, type);
    }

private:
    TexturePool() = default;
    ~TexturePool();

    Texture instance_load(const std::string& path, TextureType::Value type);

    static TexturePool instance_;

    std::vector<Texture> textures_ {};
};

}
