#ifndef _Skin_H_
#define _Skin_H_

#include "../cloth_src/Triangle.h"
#include "Skeleton.h"
#include <iostream>

class Skin {
private:
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	glm::vec3 color;

	Skeleton* skel;
	Shader renderProg; // skin render program
	glm::mat4 model;
	// Vertex Information
	std::vector<glm::vec3> position;
	std::vector<glm::vec3> new_position;

	std::vector<glm::vec3> normal;
	std::vector<glm::vec3> new_normal;

	std::vector<unsigned int> indices;
	std::vector<glm::mat4> bindings;
	std::vector<glm::mat4> invBindings;
	std::vector<std::vector<std::pair<int, float>>> skinWeights;

public:

	glm::mat4 getModelMat();

	// Parses in a '.skin' and '.skel' file data  
	Skin(const char* skinFileName = "src/animations/wasp/wasp.skin", const char* skelFileName = "src/animations/wasp/wasp.skel");

	// Deletes a pointer to the skeleton object
	~Skin();

	void Draw(const glm::mat4& viewProjMtx, GLFWwindow* window);

	void Update(glm::mat4 topMatrix);

	void spin(float deg);

	bool Load(const char* fileName);

	Skeleton* getSkeleton();

	glm::mat4 getWorldMatrix(int jointIdx);

	void precomputeBindings();

	void renderImGui(GLFWwindow* window); 
};
#endif
