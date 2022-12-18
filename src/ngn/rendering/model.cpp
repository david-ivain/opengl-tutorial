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

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureOptions> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
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
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<TextureOptions> diffuseMaps = loadMaterialTextures(material,
            aiTextureType_DIFFUSE, TextureType::Diffuse);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<TextureOptions> specularMaps = loadMaterialTextures(material,
            aiTextureType_SPECULAR, TextureType::Specular);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<TextureOptions> emissionMaps = loadMaterialTextures(material,
            aiTextureType_EMISSIVE, TextureType::Emission);
        textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());
    }

    meshes_.emplace_back(vertices, indices, textures);
}

std::vector<TextureOptions> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType::Value typeName)
{
    std::vector<TextureOptions> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (auto& loaded_texture : loaded_textures_) {
            if (std::strcmp(loaded_texture.c_str(), str.C_Str()) == 0) {
                skip = true;
                break;
            }
        }
        if (skip)
            continue;

        TextureOptions texture {
            .path = directory_ + "/" + str.C_Str(),
            .type = typeName
        };
        textures.push_back(texture);
        loaded_textures_.push_back(str.C_Str());
    }
    return textures;
}

const std::vector<Mesh>& Model::meshes() const
{
    return meshes_;
}

}
