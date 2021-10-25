#ifndef _CLOTH_H_
#define _CLOTH_H_

#include "SpringDamper.h"
#include "Triangle.h"
#include "stdlib.h"
#include "../FloorTile.h"
#include "../shader.h"

#include <fstream>
#include <sstream>

class Cloth {
private:

	Shader renderProg, computeProg, computeProgNorm;
	GLuint clothVAO; 
	GLuint VBO_pos, VBO_normals;
	GLuint EBO;
	GLuint clothTextureID;

	GLuint computePosBuf[2]; // creating VBOs to read and write from the compute buffers
	GLuint computeVeloBuf[2];
	GLuint computeShaderProgram;
	GLuint computeShader;
	glm::vec2 nParticles;
	int readBuf;

	glm::mat4 model; // cloth model matrix (applies to all vertices of the cloth)
	glm::vec3 color; // cloth's color

	int numSprings; // numsprings: (gridSize-1)*(gridSize)*2 + (gridSize-1)*(gridSize-1)*2   which are the horiz, vert and diagonals springs
	int gridSize; // how big it should be

	glm::vec3 vAir; // air velo
	float springConst, dampConst; // physical consts
	float rho, Cd, rest_const, dynamic_fric; // other physical consts
	
	// Cloth has an array of Particles, Triangles, and springs
	std::vector<Particle*> particles;
	std::vector<SpringDamper*> springs;
	std::vector<Triangle> triangles; // triangle vector	

	std::vector<int> triIndices; // Indices for the EBO 
	std::vector<glm::vec3> particlePos; // reflects the positions in "particles"
	std::vector<glm::vec3> particleNorm; // reflects the normals in "particles"
	std::vector<std::vector<glm::vec4>> posBufs; 
	std::vector<std::vector<glm::vec4>> veloBufs; 

public:

	bool gotWind;

	Cloth(const char* computeShaderPath = "src/shaders/clothCompute.comp");

	~Cloth();

	void Draw(const glm::mat4& viewProjMtx, GLFWwindow* window);

	void renderImGui(GLFWwindow* window);

	// pass in time, and grav force (and maybe wind velocity later?)
	void Update(float delta_t, glm::vec3 g, FloorTile* floor, int steps); 

	void Update2(); 

	void togglePos(glm::vec3 moveAmt);

	bool toggleWind();

	void windSpeed(float magnitude);

	glm::vec3& getWindVelo();

	void spin(float deg);

	void setClothTextureID(GLuint texID);

};
#endif