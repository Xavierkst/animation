#include "Joint.h"
#include <iostream>
Joint::Joint()
{
	//localM = localMat;
	setBoxMinMax(glm::vec3(-.1f, -.1f, -.1f), glm::vec3(.1f, .1f, .1f));
	setOffset(glm::vec3(.0f, .0f, .0f));
	this->parent = nullptr;
	this->localM = glm::mat4(1.0f);
	this->worldM = glm::mat4(1.0f);
	this->jointName = " ";
	this->idx = 0;
	
	// Auto-initialize dofArray at least to x, y and z
	addDof(new Dof());
	addDof(new Dof());
	addDof(new Dof());

	this->cube = nullptr;
}

Joint::~Joint()
{
	for (int i = 0; i < children.size(); i++) {
		delete children[i];
	}

	for (int j = 0; j < dofArray.size(); j++) {
		delete dofArray[j];
	}
}

void Joint::setBoxMinMax(glm::vec3 min, glm::vec3 max)
{
	this->boxMin = min;
	this->boxMax = max;
}

void Joint::setOffset(glm::vec3 offset)
{
	this->offset = offset;
}

void Joint::Update(glm::mat4& parentM)
{
	// Compute local matrix L: 
	// We're given translation values (offset) and
	// rotation values (pose) so build your Local matrix
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), offset);

	glm::mat4 rot_X = glm::rotate(glm::mat4(1.0f), 
		dofArray[0]->getValue(), glm::vec3(1.0f, .0f, .0f));

	glm::mat4 rot_Y = glm::rotate(glm::mat4(1.0f), 
		dofArray[1]->getValue(), glm::vec3(.0f, 1.0f, .0f));

	glm::mat4 rot_Z = glm::rotate(glm::mat4(1.0f), 
		dofArray[2]->getValue(), glm::vec3(.0f, .0f, 1.0f));

	this->localM = trans * rot_Z * rot_Y * rot_X;

	// computing the world matrix is to multiply
	// the parent matrix with the local matrix, then
	// you'll get the transf of the curr (child) obj wrt the
	// world 
	this->worldM = parentM * this->localM;

	this->cube->model = this->worldM;

	// all transformations of the curr object (apart from its local transf) 
	// happen from changes to the "parent" matrix. If this node is a tyre, and 
	// its parent is the car body, then moving car forward means parent matrix 
	// has one of its x/y/z values increased by 1
	for (int i = 0; i < children.size(); i++) {
		children[i]->Update(worldM);
	}
}

std::string Joint::getJointName()
{
	return jointName;
}

void Joint::setJointName(std::string name)
{
	this->jointName = name;
}

bool Joint::Load(Tokenizer& token, int& jointIdx, std::vector<Dof*>& allDofs, std::vector<Joint*>& jointArr)
{
	token.FindToken("{");

	while (1) {
		// assume token always <= 256
		char temp[256]; 
		token.GetToken(temp);
		if (strcmp(temp, "offset") == 0) {
			offset.x = token.GetFloat();
			offset.y = token.GetFloat();
			offset.z = token.GetFloat();
		}

		else if (strcmp(temp, "boxmin") == 0) {
			boxMin.x = token.GetFloat(); 
			boxMin.y = token.GetFloat();
			boxMin.z = token.GetFloat();
		}
		else if (strcmp(temp, "boxmax") == 0) {
			boxMax.x = token.GetFloat();
			boxMax.y = token.GetFloat();
			boxMax.z = token.GetFloat();
		}
		else if (strcmp(temp, "rotxlimit") == 0) {
			float min = token.GetFloat();
			float max = token.GetFloat();
			dofArray[0]->setMinMax(min, max);
		}
		else if (strcmp(temp, "rotylimit") == 0) {
			float min = token.GetFloat();
			float max = token.GetFloat();
			dofArray[1]->setMinMax(min, max);

		}
		else if (strcmp(temp, "rotzlimit") == 0) {
			float min = token.GetFloat();
			float max = token.GetFloat();
			dofArray[2]->setMinMax(min, max);
		}
		else if (strcmp(temp, "pose") == 0) {
			float dofx = token.GetFloat();
			float dofy = token.GetFloat();
			float dofz = token.GetFloat(); 

			dofArray[0]->setValue(dofx);
			dofArray[1]->setValue(dofy);
			dofArray[2]->setValue(dofz);
			allDofs.push_back(dofArray[0]);
			allDofs.push_back(dofArray[1]);
			allDofs.push_back(dofArray[2]);
		}
		else if (strcmp(temp, "balljoint") == 0) {
			char nameTemp[256];
			token.GetToken(nameTemp);

			Joint* jnt = new Joint();
			jointArr.push_back(jnt);
			jnt->setJointName(nameTemp);
			jnt->setJointIndex(jointIdx);
			jnt->Load(token, ++jointIdx ,allDofs, jointArr);
			this->addJoint(jnt);
			// assign parent ptr
			jnt->parent = this;
		}
		else if (strcmp(temp, "}") == 0) {
			break;
		}
		else token.SkipLine(); 
	}
	cube = new Cube(boxMin, boxMax);
	return true;
}

void Joint::Draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	// check viewProjMatrix * world matrix notes
	cube->draw(viewProjMtx, shader);
	for (int i = 0; i < children.size(); i++) {
		children[i]->Draw(viewProjMtx, shader);
	}
}

void Joint::addJoint(Joint* childJoint)
{
	children.push_back(childJoint);
}

void Joint::addDof(Dof* newDof)
{
	dofArray.push_back(newDof);
}

glm::mat4 Joint::getWorldMatrix()
{
	return this->worldM;
}

Joint* Joint::getParent()
{
	return parent;
}

std::vector<Joint*> Joint::getChildren()
{	
	return children;
}

void Joint::setJointIndex(int index)
{
	this->idx = index;
}

int Joint::getJointIndex()
{
	return this->idx;
}

std::vector<Dof*> Joint::getDofArray()
{
	return dofArray;
}
