#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "core.h"

class Vertex {
private: 
	std::vector<glm::vec3> position; 
	std::vector<glm::vec3> normal; 
	// stores the joint index and its corresponding weight
	std::vector<std::vector<std::pair<int, float>*>> skinWeights;

public:
	Vertex(); // def constructor

	~Vertex();

	void setVertices(glm::vec3 v); 
	void setNormals(glm::vec3 n);

	std::vector<glm::vec3>& getVertices();

	std::vector<glm::vec3>& getNormals();

	//void setSkinWeights();

	std::vector<std::vector<std::pair<int, float>*>>& getSkinWeights();



};

#endif