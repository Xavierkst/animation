#ifndef _CLOTH_H_
#define _CLOTH_H_

#include "SpringDamper.h"
#include "Triangle.h"
#include "stdlib.h"
#include "FloorTile.h"

class Cloth {
private:

	GLuint VAO; 
	GLuint VBO_pos, VBO_normals;
	GLuint EBO;
	glm::mat4 model; // cloth model matrix (applies to all vertices of the cloth)
	glm::vec3 color; // cloth's color

	int numSprings; // numsprings is given by (gridSize-1)*(gridSize)*2 + (gridSize-1)*(gridSize-1)*2   which are the horiz, vert and diagonals springs
	int gridSize; 

	float springConst, dampConst;
	glm::vec3 vAir;
	// swaps bet. vAir and 0 for toggling wind
	glm::vec3 vAirHolder;
	float rho, Cd, rest_const, dynamic_fric;

	
	// Cloth has an array of Particles, Triangles, and springs
	std::vector<Particle*> particles;
	std::vector<SpringDamper*> springs;
	std::vector<Triangle> triangles; // triangle vector	

	std::vector<int> triIndices;
	std::vector<glm::vec3> particlePos; // reflects the positions in "particles"
	std::vector<glm::vec3> particleNorm; // reflects the normals in "particles"
	std::vector<glm::vec3> originPts; // the original positions for the top row of cloth
	glm::mat4 transl;
	//glm::mat4 rot;
public:

	bool gotWind;

	Cloth(glm::vec3 pForce, glm::vec3 pVelo, float pMass, int gSize);
	~Cloth();

	void Draw(const glm::mat4& viewProjMtx, GLuint shader, GLFWwindow* window);

	void renderImGui(GLFWwindow* window);

	// pass in time, and grav force (and maybe wind velocity later?)
	void Update(float delta_t, glm::vec3 g, FloorTile* floor, int steps); 

	void togglePos(glm::vec3 moveAmt);

	bool toggleWind();

	void windSpeed(float magnitude);

	glm::vec3& getWindVelo();

	void spin(float deg);

};
#endif