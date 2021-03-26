#include "Skeleton.h"

Skeleton::Skeleton()
{
    root = nullptr;
}

Skeleton::~Skeleton()
{
    // Note: Dofs and jointArr are deleted 
    // in Joint destructor
    delete root;
}

bool Skeleton::Load(const char* fileName)
{
    Tokenizer token;
    token.Open(fileName);
    token.FindToken("balljoint");
    char jointName[256];
    token.GetToken(jointName);

    // parse tree 
    root = new Joint();
    jointArr.push_back(root); 
    root->setJointName(jointName);

    // create 3 translational DOF values in the dofArray (x,y,z)
    allDofs.push_back(new Dof());
    allDofs.push_back(new Dof());
    allDofs.push_back(new Dof());
    int jointCount = 0;
    root->setJointIndex(jointCount);
    root->Load(token, ++jointCount,allDofs, jointArr);

    token.Close();
    return true;
}

void Skeleton::Update(glm::mat4& parent)
{
    // the first 3 are the x,y,z translation dofs, 
    // while the remaining are rotational DOFs
    glm::vec3 offset(allDofs[0]->getValue(), 
        allDofs[1]->getValue(), allDofs[2]->getValue());

	glm::mat4 trans = glm::translate(glm::mat4(1.0f), offset);
    root->Update(trans * parent);
}

void Skeleton::Draw(const glm::mat4& viewProjMtx, GLuint shader)
{
    root->Draw(viewProjMtx, shader);
}

Dof* Skeleton::getNextDof(int& idx)
{
    if ( -1 < idx && idx < allDofs.size()) {
        return allDofs[idx];
    }
    else return nullptr;
}

std::vector<Dof*>& Skeleton::getDofArray()
{
    return allDofs;
}

glm::mat4 Skeleton::getWorldMatrixBFS(int jointIdx)
{
    Joint* jointPtr = this->getJointBFS(jointIdx);

    if (jointPtr) {
        return jointPtr->getWorldMatrix();
    }
    // if nothing, return ID matrix
    else {
        return glm::mat4(1.0f);
    }
}

Joint* Skeleton::getJointBFS(int jointIdx)
{
    std::queue<Joint*> jointQueue;

    jointQueue.push(root);
    Joint* jointPtr;

    while (!jointQueue.empty()) {
        jointPtr = jointQueue.front();
        jointQueue.pop();
        
        if (jointPtr->getJointIndex() == jointIdx) {
            return jointPtr;
        }
       
        for (int i = 0; i < jointPtr->getChildren().size(); i++) {
            jointQueue.push(jointPtr->getChildren()[i]);
        }
    }
    return nullptr;
}

std::vector<Joint*> Skeleton::getJointArray()
{
    return jointArr;
}

Joint* Skeleton::getRoot()
{
    return root;
}
