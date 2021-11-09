#include "SpringDamper.h"

SpringDamper::SpringDamper()
{
	Ks = .0f;
	Kd = .0f;
	l_0 = .0f;
	p1 = nullptr;
	p2 = nullptr;
}

SpringDamper::SpringDamper(float kSpring, float kDamp, float natLength, 
	std::shared_ptr<Particle> part1, 
	std::shared_ptr<Particle> part2)
{
	p1 = part1;
	p2 = part2;
	Ks = kSpring;
	Kd = kDamp;
	l_0 = natLength;
}

void SpringDamper::computeForce()
{
	// find f-aero dir vector
	glm::vec3 e = p2->getPos() - p1->getPos();
	// compute curr length
	float l = glm::length(e);
	e = e / l; // normalize
	// compute closing velo: 
	float v_close = glm::dot((p1->getVelo() - p2->getVelo()), e);
	// compute final forces: 
	float f = -1.0f * Ks * (l_0 - l) - (Kd * v_close);
	glm::vec3 f1(e * f);
	glm::vec3 f2(-f1);
	if (p1->getForceApply()) {
		p1->applyForce(f1);
	}

	if (p2->getForceApply()) {
		p2->applyForce(f2);
	}
}

void SpringDamper::setSpringConst(float ks)
{ 
	Ks = ks; 
}

void SpringDamper::setDampConst(float c)
{
	Kd = c;
}

void SpringDamper::setNatLength(float length)
{
	l_0 = length;
}

