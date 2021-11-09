#include "Model.h"

void Model::loadModel(std::string const& path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	// finds the very last (right most) backslash char
	directory = path.substr(0, path.find_last_of('/'));
	// start processing all meshes from the root node, and its children nodes	
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// For each mesh in the current node, processMesh
	for (int i = 0; i < node->mNumMeshes; ++i) {
		processMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}
	// For each child in the node, call process Node on it again
	for (int j = 0; j < node->mNumChildren; ++j) {
		processNode(node->mChildren[j], scene);
	}

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> verts;
	std::vector<unsigned int> idx;
	std::vector<Texture> tex;

	for (int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex v;
		v.pos.x = mesh->mVertices[i].x;
		v.pos.y = mesh->mVertices[i].y;
		v.pos.z = mesh->mVertices[i].z;

		if (mesh->HasNormals()) {
			v.normal.x = mesh->mNormals[i].x;
			v.normal.y = mesh->mNormals[i].y;
			v.normal.z = mesh->mNormals[i].z;
		}

		// each mesh can have many texture coordinates, 
		// but if we assume they only have 1, we can use just that one 
		if (mesh->mTextureCoords[0]) {
			v.texCoord.x = mesh->mTextureCoords[i]->x;
			v.texCoord.y = mesh->mTextureCoords[i]->y;
		}

		verts.push_back(v);
	}

	if (mesh->HasFaces()) {
		for (int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; ++j) {
				idx.push_back(face.mIndices[j]);
			}
		}
	}

	aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diff_tex_maps = loadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
	std::vector<Texture> spec_tex_maps = loadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular");
	tex.insert(tex.end(), diff_tex_maps.begin(), diff_tex_maps.end());
	tex.insert(tex.end(), spec_tex_maps.begin(), spec_tex_maps.end());

	return Mesh(verts, idx, tex);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType texture_type, std::string type_name)
{
	std::vector<Texture> textures;
	for (int i = 0; i < material->GetTextureCount(texture_type); ++i) {
		Texture texture;
		aiString tex_name;
		material->GetTexture(texture_type, i, &tex_name);
		bool found = false;
		for (int j = 0; j < loaded_textures.size(); ++j) {
			if (!std::strcmp(loaded_textures[i].path.data(), tex_name.C_Str())) {
				texture = loaded_textures[j];
				found = true;
				break;
			}
		}
		if (!found) {
			// read the texture from the given path--we build it with dir and tex name
			texture.textureID = TextureFromFile(tex_name.C_Str(), directory);
			// gives the texture name -- which isnt the full path actually
			texture.path = tex_name.C_Str();
			// get texture's type (eg. texture_diffuse)
			texture.type = texture_type;
			loaded_textures.push_back(texture);
		}

		textures.push_back(texture);
	}

	return textures;
}

unsigned int Model::TextureFromFile(char const* str, std::string& dir)
{
	unsigned int texID;
	glGenTextures(1, &texID);
	std::string fileName = dir + '/' + std::string(str);
	int imgWidth, imgHt, num_components;
	unsigned char * image_data = stbi_load(fileName.c_str(), &imgWidth, &imgHt, &num_components, 0);

	if (image_data) {
		GLenum format;
		if (num_components == 1)
			format = GL_RED;
		if (num_components == 3)
			format = GL_RGB;

		if (num_components == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// send image data over to GPU w/ corresp texID
		glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHt, 0, format, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture at path: "<< str << std::endl;
	}

	// free the image data aft sending it over to GPU 
	stbi_image_free(image_data);

	return texID;
}


