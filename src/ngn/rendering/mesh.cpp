#include "mesh.h"

#include "../utils/log.h"

#include <glad/glad.h>

namespace ngn {

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices)
    : vertices_(vertices)
    , indices_(indices)
{
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned),
        &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinates));

    glBindVertexArray(0);

    LOGF("Mesh { .VAO:%u, .VBO:%u, .EBO:%u } created.", VAO_, VBO_, EBO_);
}

Mesh::~Mesh()
{
    LOGF("Mesh { .VAO:%u, .VBO:%u, .EBO:%u } deleted.", VAO_, VBO_, EBO_);
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteBuffers(1, &EBO_);
}

unsigned Mesh::VAO() const
{
    return VAO_;
}
const std::vector<Vertex>& Mesh::vertices() const
{
    return vertices_;
}

const std::vector<unsigned>& Mesh::indices() const
{
    return indices_;
}

}
