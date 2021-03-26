#ifndef _JOINT_H_
#define _JOINT_H_

#include "Dof.h"
#include "Tokenizer.h"
#include "Cube.h"
#include <string>

class Joint {
private:

	glm::mat4 localM; // defaulted
	glm::mat4 worldM; // defaulted

	glm::vec3 offset; // temporarily store offset here, then transfer
	// into a mat4 later? 
	glm::vec3 boxMin; // defaulted
	glm::vec3 boxMax; // defaulted

	// each joint has a cube 
	Cube* cube;

	// array storing all the degs of Freedom
	// for this joint
	std::vector<Dof*> dofArray; 

	Joint* parent; // defaulted
	std::vector<Joint*> children; 

	std::string jointName;

	int idx; 
	
public:
	// constructor & destructor
	Joint(); // default constructor
	~Joint();


	// GETTERS ---------------------------------------------
	std::string getJointName(); 

	// returns world matrix of this joint
	glm::mat4 getWorldMatrix();

	// returns ptr to parent node
	Joint* getParent();

	// return this joint's children joints (array)
	std::vector<Joint*> getChildren(); 

	// returns the index tagged to a particular joint
	// (note: already tagged in parsing phase constructor)
	int getJointIndex(); 

	// returns the array of DOFs
	std::vector<Dof*> getDofArray(); 

	// SETTERS ---------------------------------------------
	void setJointName(std::string name); 

	// Set the index of the joint
	void setJointIndex(int index); 

	// Set joint cube parameters
	void setBoxMinMax(glm::vec3 min, glm::vec3 max);

	// Sets the join offset relative to the parent joint
	void setOffset(glm::vec3 offset);

	// OTHERS ---------------------------------------------

	// Pass in a tokenzier object
	// creates the cube and initializes them
	// accordingly with each balljoint
	// read in
	bool Load(Tokenizer& token, int& jointIdx, std::vector<Dof*>& allDofs, std::vector<Joint*>& jointArr); 

	// takes in the viewProjMatrix and the shader, draws
	// itself, then recursively draws all its children (if it has any)
	void Draw(const glm::mat4& viewProjMtx, GLuint shader); 

	// add a child joint to the current joint
	void addJoint(Joint* childJoint);

	// add a dof to the current joint
	void addDof(Dof* newDof);

	// recursively generate local matrix & concat parent Matrices
	// with it
	void Update(glm::mat4& parent);
};
#endif