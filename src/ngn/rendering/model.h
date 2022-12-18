#pragma once

#include "mesh.h"
#include "texture.h"

#include <assimp/scene.h>

namespace ngn {

class Model {
public:
    Model(const std::string& path);

    Model(const Model&) = delete;
    Model(Model&&) = delete;

    const std::vector<Mesh>& meshes() const;

private:
    void process_node(aiNode* node, const aiScene* scene);
    void process_mesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, TextureType::Value type_name);

    std::vector<Mesh> meshes_;
    std::string directory_;
};

}
