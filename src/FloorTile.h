#ifndef _FLOOR_TILE_H_
#define _FLOOR_TILE_H_

#include "core.h"
#include "shader.h"

class FloorTile {
private:
	Shader renderProg;
	std::vector<glm::vec3> pos;
	std::vector<glm::vec3> normals;
	std::vector<int> triIndices;
	glm::vec3 color;
	glm::mat4 model;

	glm::vec3 velo;

	int floorSize;

	GLuint VAO;
	GLuint VBO_pos, VBO_normals;
	GLuint EBO;

public:
	FloorTile(int floor_size);
	~FloorTile();

	float getYPos(); 

	void Draw(const glm::vec3& camPos, const glm::mat4& viewProjMtx);

	glm::vec3 getVelo();

	glm::vec3 getNormal();
};

#endif