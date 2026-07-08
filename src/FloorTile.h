#ifndef _FLOOR_TILE_H_
#define _FLOOR_TILE_H_

#include <vector>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

#include "shader.h"

class FloorTile {
private:
	std::shared_ptr<Shader> renderProg;
	std::vector<glm::vec3> pos;
	std::vector<glm::vec3> normals;
	std::vector<int> triIndices;
	glm::vec3 color;
	glm::mat4 model;
	glm::vec3 velo;

	int floorSize;

	GLuint VAO;
	GLuint VBOPos, VBONormals;
	GLuint EBO;

public:
	FloorTile(int floor_size = 30);
	~FloorTile();
	float getYPos(); 
	void Draw(const glm::vec3& camPos, const glm::mat4& viewProjMtx);
	glm::vec3 getVelo();
	glm::vec3 getNormal();
};

#endif