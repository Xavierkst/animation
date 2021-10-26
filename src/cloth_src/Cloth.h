#ifndef _CLOTH_H_
#define _CLOTH_H_

#include "SpringDamper.h"
#include "Triangle.h"
#include "stdlib.h"
#include "../FloorTile.h"
#include "../shader.h"

#include <fstream>
#include <sstream>

#include "../../stb_image/stb_image.h"

class Cloth {
private:

	Shader renderProg, computeProg;
	GLuint clothVAO; 
	GLuint VBO_pos, VBO_normals;
	GLuint clothEBO;
	GLuint clothTextureID;

	GLuint normBuf;
	GLuint computeShaderID;
	glm::ivec2 nParticles;
	int readBuf;

	glm::mat4 model; // cloth model transform 
	glm::vec3 color; // cloth color

	/* Physical constants */
	glm::vec3 vAir; // air velocity
	float springConst, dampConst; 
	float rho, Cd, rest_const, dynamic_fric; 
	
	// Cloth has an array of Particles, Triangles, and springs
	std::vector<Particle*> particles;
	std::vector<SpringDamper*> springs;
	std::vector<Triangle> triangles; // triangle vector	
	std::vector<int> triIndices; // Indices for the EBO 
	std::vector<glm::vec3> particlePos; // reflects the positions in "particles"
	std::vector<glm::vec3> particleNorm; // reflects the normals in "particles"

	// Compute shader vars
	std::vector<std::vector<glm::vec4>> posBufs; 
	std::vector<std::vector<glm::vec4>> veloBufs; 
	std::vector<glm::ivec2> texCoords; 

	std::vector<glm::vec4> normalBufs; 
	std::vector<unsigned int> idxBuf;
	GLuint buff[7];
	GLuint computePosBuf[2]; // creating VBOs to read and write from the compute buffers
	GLuint computeVeloBuf[2];

public:

	bool gotWind;

	Cloth(const char* computeShaderPath = "src/shaders/clothCompute.comp");

	Cloth(float dur, const char* computeShaderPath = "src/shaders/clothCompute.comp");

	~Cloth();

	void Draw(const glm::vec3& camPos, const glm::mat4& viewProjMtx, GLFWwindow* window);

	void renderImGui(GLFWwindow* window);

	void LoadAndCompileShaders();

	void initializeBuffers();

	// Updates all vertex attributes each frame -- computed on the CPU bef sending to GPU 
	void Update(float delta_t, glm::vec3 g, FloorTile* floor, int steps); 

	// An experimental update() function that uses 'clothCompute.cs' (compute shader)
	// However, only works for OpenGL version >= 4.2 
	void Update2(); 

	void togglePos(glm::vec3 moveAmt);

	bool toggleWind();

	void windSpeed(float magnitude);

	glm::vec3& getWindVelo();

	void spin(float deg);

	void setClothTextureID(GLuint texID);

	unsigned int loadTexture(char const* path);

};
#endif