#ifndef _CLOTH_H_
#define _CLOTH_H_

#include "SpringDamper.h"
#include "Triangle.h"
#include "stdlib.h"
#include "../FloorTile.h"
#include "../shader.h"
#include <fstream>
#include <sstream>
#include "../Mesh.h"


class Cloth {
private:
	
	Shader renderProg; // Cloth rendering shader program
	GLuint clothVAO; 
	GLuint clothVBO;
	GLuint clothEBO;
	GLuint clothTexture[5]; // Arbitrarily decided for 5 cloth texture units

	glm::ivec2 nParticles; // Num of particles in the x and y dimensions
	glm::mat4 model; // cloth model transform 
	glm::mat4 transfMat; // keeps track of all translations on the cloth 

	/* Physical constants */
	glm::vec3 vAir; // air velocity
	float springConst, dampConst; 
	float rho, Cd, rest_const, dynamic_fric; 
	
	// Cloth has an array of Particles, Triangles, and springs

	// std::vector<Particle*> particles;
	std::vector<std::shared_ptr<Particle>> particles;
	std::vector<SpringDamper*> springs;
	std::vector<Triangle> triangles; // triangle vector	
	std::vector<int> triIndices; // Indices for the EBO 
	std::vector<Vertex> vertices; // contains Position, Normal, texCoord

	// Compute shader variables -- NOT IN USE -- 
	Shader computeProg; // compute shader program
	std::vector<std::vector<glm::vec4>> posBufs; 
	std::vector<std::vector<glm::vec4>> veloBufs; 
	std::vector<glm::vec4> normalBufs; 
	std::vector<unsigned int> idxBuf;
	GLuint buff[7];
	GLuint computePosBuf[2]; // creating VBOs to read and write from the compute buffers
	GLuint computeVeloBuf[2];
	GLuint normBuf;
	GLuint computeShaderID;
	int readBuf; // toggles bet 1 and 0 to swap computed vertices from comp shader

	// For Rendering Light Cubes in the scene -- not actually part of cloth
	Shader lightRenderProg;
	GLuint lightSourcePos;
	GLuint lightSourceVAO;

public:

	Cloth(const char* computeShaderPath = "src/shaders/clothCompute.comp", 
		const char* diffuseTexPath = "src/textures/blueDenim/Blue_Denim_Texture_DIFFUSE.png", 
		const char* specularTexPath = "src/textures/blueDenim/Blue_Denim_Texture_SPECULAR.png");

	~Cloth();

	void Draw(const glm::vec3& camPos, const glm::mat4& viewProjMtx, GLFWwindow* window);

	void renderImGui(GLFWwindow* window);

	// Updates all vertex attributes each frame -- computed on the CPU bef sending to GPU 
	// integrates motion 'steps' number of times per delta_T
	void Update(FloorTile* floor, float delta_t, glm::vec3 g = glm::vec3(.0f, -9.82f, 0.f), int steps = 60); 

	void togglePos(glm::vec3 moveAmt);

	void windSpeed(float magnitude);

	glm::vec3& getWindVelo();

	void spin(float deg);

	unsigned int loadTexture(char const* path);


	/* --------------  Compute shader functions -- NOT IN USE ----------------- */ 

	// Constructor for vertex processing using compute shader -- NOT IN USE (not working for openGL 3.3)
	Cloth(float dur, const char* computeShaderPath = "src/shaders/clothCompute.comp");

	void LoadAndCompileShaders();

	void initializeBuffers();

	// An experimental update() function that uses 'clothCompute.cs' (compute shader)
	// However, only works for OpenGL version >= 4.2 
	void Update2(); 

};
#endif