#pragma once

#include "texture.h"
#include "vertex.h"

#include <string>
#include <vector>

namespace ngn {

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices, const std::vector<TextureOptions>& texture_options);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;

    unsigned VAO() const;
    const std::vector<Vertex>& vertices() const;
    const std::vector<unsigned>& indices() const;
    const std::vector<Texture>& textures() const;

private:
    unsigned VAO_, VBO_, EBO_;
    std::vector<Vertex> vertices_;
    std::vector<unsigned> indices_;
    std::vector<Texture> textures_;
};

}
