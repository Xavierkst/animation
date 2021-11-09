#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "../Particle.h"

// a class that holds triangle 
class Triangle {
private:
	std::vector<int> triVerts;

	// Particle* p1;
	// Particle* p2;
	// Particle* p3;
	std::shared_ptr<Particle> p1;
	std::shared_ptr<Particle> p2;
	std::shared_ptr<Particle> p3;
	glm::vec3 normal;
	glm::vec3 vSurface;

public:
	// Triangle constructor 
	Triangle();
	
	// Triangle(Particle* part1, Particle* part2, Particle* part3);
	Triangle(std::shared_ptr<Particle> part1, 
		std::shared_ptr<Particle> part2, 
		std::shared_ptr<Particle> part3);

	// Calculates aerodynamic force on the triangle
	glm::vec3 calcAeroForce(glm::vec3 airVelo, float rho, float Cd);
	// Calculates the x-sectional area of tri in contact with air
	float getContactArea(glm::vec3 airVelo);

	// calculates and returns the average velo of 3 pts 
	glm::vec3 calcVSurface();
	
	glm::vec3 getVSurface();
	// calcs triangle's normal and returns it
	void calcNormal();

	glm::vec3 getNormal();

	// get pointer to each particle p1 p2 p3
	Particle* getP1() const;
	Particle* getP2() const;
	Particle* getP3() const;

	std::vector<int>& getTriVerts();

	void setNormal(glm::vec3 n);
	void addNormal(glm::vec3 n);
	void setVSurface(glm::vec3 v);
	
};
#endif
