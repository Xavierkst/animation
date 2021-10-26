#ifndef _PARTICLE_SYS_H_
#define PARTICLE_SYS_H_

#include "../Particle.h"
#include "../FloorTile.h"

class ParticleSys {
private:
	GLuint VAO, VBO_pos, VBO_normals, EBO;
	glm::mat4 model;
	
	std::vector<Particle> particles2;
	std::vector<Particle> particles;
	std::vector<int> indices;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;

	// ParticleSystem calculation stuff
	int lastUsedParticleIdx;
	int numParticles;
	int particlesPerSecond;
	float roundOffError;
	glm::vec3 gravity;

	float rho; // fluid density
	float Cd;
	glm::vec3 vAir;

	// ParticleSystem attributes
	glm::vec3 color;
	float radius;
	float dynamic_fric_coeff;
	float restitute_const; 
	
	float lifeSpan;
	glm::vec3 originPos;
	float mass;
	float tx, ty, tz;
	float px, py, pz;
	float sVariance;

public: 
	ParticleSys(int num_particles);

	void Draw(const glm::mat4& viewProjMtx, GLuint shader, GLFWwindow* window);

	void Update(float deltaTime, FloorTile* floor);

	void renderImGui(GLFWwindow* window); 

	int findLastUsedParticleIdx();

	void applyAllForces(); 

	int& getCreationRate();

	glm::vec3& getOriginPos();

	glm::vec3& getWindVelocity();

	float& getLifeSpan();

	float& getGravity(); 

	float& getAirDensity();

	float& getDragCoeff();

	float& getParticleRadius();

	float& getCollisionElasticity();

	float& getFricCoeff();

	// wind velo variance
	float& getTx();
	float& getTy();
	float& getTz();

	// positional variance
	float& getPx();
	float& getPy();
	float& getPz();

	// life span variance
	float& getLifeVariance();
};

#endif