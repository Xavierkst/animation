#include "Particle.h"

Particle::Particle()
{
	force = glm::vec3(.0f);
	velo = glm::vec3(.0f);
	pos = glm::vec3(.0f);
	normal = glm::vec3(.0f);
	mass = .0f;
	force_apply = false;
	lifeSpan = -1.0f;
}

Particle::Particle(glm::vec3 f, glm::vec3 v, glm::vec3 p, glm::vec3 norm, float m)
{
	force = f;
	velo = v;
	pos = p;
	normal = norm;
	mass = m;
	force_apply = true;
	lifeSpan = -1.0f;
}

void Particle::applyForce(glm::vec3& f)
{
	if (force_apply) {
		force += f;
	}
}

glm::vec3 Particle::integrateMotion(float delta_t)
{
	// find the accel first given the force
	glm::vec3 accel = (1.0f / mass) * force;
	// find new velo
	velo += accel * delta_t;
	// find new position
	pos += velo * delta_t;
	// reset force to 0 at the end
	force = glm::vec3(0.0f); 

	return pos;
}

void Particle::setForce(glm::vec3 f)
{
	force = f;
}

void Particle::setVelo(glm::vec3 v)
{
	velo = v;
}

void Particle::setPos(glm::vec3 p)
{
	pos = p;
}

void Particle::setMass(float m)
{
	mass = m;
}

void Particle::setNorm(glm::vec3 n)
{
	normal = n;
}

void Particle::addNorm(glm::vec3 n)
{
	normal += n;
}

glm::vec3 Particle::getPos()
{
	return pos;
}

glm::vec3 Particle::getForce()
{
	return force;
}

glm::vec3 Particle::getVelo()
{
	return velo;
}

glm::vec3 Particle::getNorm()
{
	return normal;
}

float Particle::getMass()
{
	return mass;
}

void Particle::setForceApply(bool val)
{
	force_apply = val;
}

void Particle::setLifeSpan(float span)
{
	lifeSpan = span;
}

bool Particle::getForceApply()
{
	return force_apply;
}

float Particle::getLifeSpan()
{
	return lifeSpan;
}
