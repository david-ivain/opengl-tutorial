#pragma once

#include "vertex.h"

#include <vector>

namespace ngn {

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;

    unsigned VAO() const;
    const std::vector<Vertex>& vertices() const;
    const std::vector<unsigned>& indices() const;

private:
    unsigned VAO_, VBO_, EBO_;
    std::vector<Vertex> vertices_;
    std::vector<unsigned> indices_;
};

}
