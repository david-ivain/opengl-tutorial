#include "model.h"

#include "../utils/log.h"
#include "texture.h"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>

namespace ngn {

Model::Model(const std::string& path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOGERRF("ERROR::ASSIMP::%s", import.GetErrorString());
        return;
    }
    directory_ = path.substr(0, path.find_last_of('/'));

    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        process_mesh(mesh, scene);
    }
    // then do the same for each of its children
    for (unsigned i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
}

void Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    std::vector<Texture> textures;

    for (unsigned i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texture_coordinates = vec;
        } else
            vertex.texture_coordinates = glm::vec2(0.0f, 0.0f);
        vertices.push_back(vertex);
    }
    for (unsigned i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuse_maps = load_material_textures(material,
            aiTextureType_DIFFUSE, TextureType::Diffuse);
        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

        std::vector<Texture> specular_maps = load_material_textures(material,
            aiTextureType_SPECULAR, TextureType::Specular);
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

        std::vector<Texture> emission_maps = load_material_textures(material,
            aiTextureType_EMISSIVE, TextureType::Emission);
        textures.insert(textures.end(), emission_maps.begin(), emission_maps.end());
    }

    meshes_.emplace_back(vertices, indices, textures);
}

std::vector<Texture> Model::load_material_textures(aiMaterial* mat, aiTextureType type, TextureType::Value typeName)
{
    std::vector<Texture> textures;
    for (unsigned i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        Texture texture = TexturePool::load(directory_ + "/" + str.C_Str(), typeName);
        textures.push_back(texture);
    }
    return textures;
}

const std::vector<Mesh>& Model::meshes() const
{
    return meshes_;
}

}
