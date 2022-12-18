#pragma once

#include "mesh.h"
#include "texture.h"

#include <assimp/scene.h>
#include <string.h>
#include <string>
#include <vector>

namespace ngn {

class Model {
public:
    Model(const std::string& path);

    Model(const Model&) = delete;
    Model(Model&&) = delete;

    const std::vector<Mesh>& meshes() const;

private:
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<TextureOptions> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType::Value type_name);

    std::vector<Mesh> meshes_;
    std::string directory_;
    std::vector<std::string> loaded_textures_;
};

}
