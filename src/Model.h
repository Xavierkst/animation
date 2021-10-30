#ifndef _MODEL_H_
#define _MODEL_H_

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../stb_image/stb_image.h"
#include "Mesh.h"

class Model {
public:
	// Store all textures globally to prevent (bottleneck) re-loading of the same textures 
	std::vector<Texture> loaded_textures;

	// A model can comprise of multiple meshes
	std::vector<Mesh> meshes;

	// The dir where the model file is stored
	std::string directory; 

	Model(char const* path) {
		loadModel(path);
	}

	void Draw(Shader& shaderProg);

private:
	// Uses assimp importer loader to load an aiScene containing the meshes & nodes
	void loadModel(std::string const &path);

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType texture_type, std::string type_name);

	unsigned int TextureFromFile(char const* str, std::string& dir);

};
#endif
