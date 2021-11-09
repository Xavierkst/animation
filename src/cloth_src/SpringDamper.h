#ifndef _SPRING_DAMPER_H_
#define _SPRING_DAMPER_H_

#include "../Particle.h"

class SpringDamper {
private:
	float Ks; // spring constant
	float Kd; // damping constant
	float l_0; // nat'l spring length
	// Particle* p1;
	// Particle* p2;
	std::shared_ptr<Particle> p1;
	std::shared_ptr<Particle> p2;
public:
	
	SpringDamper();
	// SpringDamper(float kSpring, float kDamp, float natLength, Particle* part1, Particle* part2);
	SpringDamper(float kSpring, float kDamp, float natLength, 
		std::shared_ptr<Particle> part1, 
		std::shared_ptr<Particle> part2);

	~SpringDamper();

	// Computes the spring-damper for the 2 particles its
	// connected to
	void computeForce();

	void setSpringConst(float ks);
	void setDampConst(float c);
	void setNatLength(float length); 
	void setP1(Particle* p); 
	void setP2(Particle* p); 
};
#endif