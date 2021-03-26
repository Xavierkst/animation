#ifndef _SKELETON_H_
#define _SKELETON_H_
#include "Joint.h"
#include <queue>

class Skeleton {
private:
	Joint* root;
	std::vector<Dof*> allDofs;
	std::vector<Joint*> jointArr;

public:
	Skeleton();

	~Skeleton();

	bool Load(const char* fileName);

	void Update(glm::mat4& parent);

	void Draw(const glm::mat4& viewProjMtx, GLuint shader);
	
	// GETTERS --------------------------------------------

	// Returns the world matrix of with index: jointIdx
	glm::mat4 getWorldMatrixBFS(int jointIdx);

	Dof* getNextDof(int& idx); 

	std::vector<Dof*>& getDofArray(); 

	Joint* getJointBFS(int jointIdx);

	std::vector<Joint*> getJointArray();

	Joint* getRoot();
};
#endif