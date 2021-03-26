#include "Vertex.h"

Vertex::Vertex()
{
    //vert = glm::vec3(.0f);
    //normals = glm::vec3(.0f);
}

Vertex::~Vertex()
{
    for (int i = 0; i < skinWeights.size(); i++) {
        for (int j = 0; j < skinWeights[j].size(); j++) {
            delete skinWeights[i][j];
        }
    }
}
//
//void Vertex::setVertices(glm::vec3 v)
//{
//    vert.x = v.x;
//    vert.y = v.y;
//    vert.z = v.z;
//}
//
//void Vertex::setNormals(glm::vec3 n)
//{
//    normals.x = n.x;
//    normals.y = n.y;
//    normals.z = n.z;
//}

std::vector<glm::vec3>& Vertex::getVertices()
{
    return position;
}


std::vector<glm::vec3>& Vertex::getNormals() {
    return normal;
}
//void Vertex::setSkinWeights()
//{
//    skinWeights
//}

std::vector<std::vector<std::pair<int, float>*>>& Vertex::getSkinWeights()
{
    return skinWeights;
}
