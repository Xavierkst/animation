#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "Shader.h"

// We use a vector of structs to encapsulate the cloth vertex
// data since struct data layout is contiguous
struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

struct Texture {
	unsigned int textureID;
	std::string type;
	std::string path;
};

class Mesh {
public: 
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex>& v, std::vector<unsigned int>& idx, std::vector<Texture>& tex);

	void Draw(Shader& shaderProg);
private: 
	unsigned int VAO, VBO, EBO;
	
	void setupMesh();

};



#endif