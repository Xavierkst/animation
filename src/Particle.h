#ifndef _PARTICLE_H_
#define _PARTICLE_H_ 

#include "core.h"

// struct Vertex {
// 	glm::vec3 pos;
// 	glm::vec3 normal;
// 	glm::vec2 texCoord;
// 	glm::vec3 color;
// };

class Particle {
private:
	glm::vec3 force;
	glm::vec3 velo;
	glm::vec3 pos;
	glm::vec3 normal;
	float mass; 
	float lifeSpan;
	bool force_apply;

public:
	Particle();

	Particle(glm::vec3 f, glm::vec3 v, glm::vec3 p, glm::vec3 norm, float m);

	void applyForce(glm::vec3& f);

	glm::vec3 integrateMotion(float delta_t); 

	// setters
	void setForce(glm::vec3 f);
	void setVelo(glm::vec3 v);
	void setPos(glm::vec3 p);
	void setMass(float m);
	void setNorm(glm::vec3 n);
	void addNorm(glm::vec3 n);
	void setForceApply(bool val);
	void setLifeSpan(float span);

	// getters
	glm::vec3 getPos();
	glm::vec3 getForce();
	glm::vec3 getVelo();
	glm::vec3 getNorm();
	float getMass();
	bool getForceApply(); 
	float getLifeSpan(); 

};

#endif