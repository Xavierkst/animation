#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>& v, std::vector<unsigned int>& idx, std::vector<Texture>& tex)
{
	vertices = v;
	indices = idx;
	textures = tex;

	setupMesh();
}

void Mesh::Draw(Shader& shaderProg)
{
	int num_diffuse_tex = 1;
	int num_specular_tex = 1;
	glBindVertexArray(VAO);
	for (int i = 0; i < textures.size(); ++i) {
		std::string tex_type = textures[i].type;
		std::string name;
		glActiveTexture(GL_TEXTURE0 + i);
		if (tex_type.compare("texture_diffuse") == 0) {
			name = "material." + tex_type + std::to_string(num_diffuse_tex++);
		}
		else if (tex_type.compare("texture_specular") == 0) {
			name = "material." + tex_type + std::to_string(num_specular_tex++);
		}

		shaderProg.setInt(name, i);
		glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
	}

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	// unbind VAO
	glBindVertexArray(0);
}

void Mesh::setupMesh()
{
	// Setup VAO, VBO, EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	// pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	// tex
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
}


