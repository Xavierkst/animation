#include "Triangle.h"

Triangle::Triangle(Particle* part1, Particle* part2, Particle* part3)
{
    p1 = part1; 
    p2 = part2;
    p3 = part3;

    normal = glm::normalize(
        glm::cross((p2->getPos() - p1->getPos()), (p3->getPos() - p1->getPos()))
        );
    vSurface = calcVSurface();
}

glm::vec3 Triangle::calcAeroForce(glm::vec3 airVelo, float rho, float Cd)
{
    // calc rel velo 
    float v_sq = pow(glm::length(calcVSurface() - airVelo), 2.0f);
    // get contact area "a"
    float a = getContactArea(airVelo);
    // get triangle normal
    glm::vec3 norm = this->getNormal();
    glm::vec3 aeroForce = -0.5f * rho * v_sq * Cd * a * norm;
    return aeroForce;
}

float Triangle::getContactArea(glm::vec3 airVelo)
{
    float a_0 = 0.5f * glm::length(glm::cross( (p2->getPos() - p1->getPos()), (p3->getPos() - p1->getPos())));
    float a = a_0 * (glm::dot(glm::normalize(airVelo), this->getNormal()));
    return a;
}

glm::vec3 Triangle::calcVSurface()
{
   vSurface = (p1->getVelo() + p2->getVelo() + p3->getVelo()) / 3.0f;
   return vSurface;
}

glm::vec3 Triangle::getVSurface()
{
    return vSurface;
}

void Triangle::calcNormal()
{
     normal = glm::normalize(glm::cross((p3->getPos() - p1->getPos()), 
         (p2->getPos() - p1->getPos())));
}

glm::vec3 Triangle::getNormal()
{
    return normal;
}

Particle* Triangle::getP1()
{
    return p1;
}

Particle* Triangle::getP2()
{
    return p2;
}

Particle* Triangle::getP3()
{
    return p3;
}

std::vector<int>& Triangle::getTriVerts()
{
    return triVerts;
}

void Triangle::setNormal(glm::vec3 n)
{
    normal = n;
}

void Triangle::addNormal(glm::vec3 n)
{
    normal += n;
}

void Triangle::setVSurface(glm::vec3 v)
{
    vSurface = v;
}

