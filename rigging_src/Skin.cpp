#include "Skin.h"

glm::mat4 Skin::getModelMat()
{
    return model;
}

Skin::Skin(const char* skinFileName, const char* skelFileName)
{
	// Model matrix.
	model = glm::mat4(1.0f);

	// The color of the cube. Try setting it to something else!
	color = glm::vec3(202.0f/255.0f, 247.0f/255.0f, 227.0f/255.0f);

	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);

    // Load the skeleton and skin files
    skel = new Skeleton();
    skel->Load(skelFileName);
    this->Load(skinFileName);
}

Skin::~Skin()
{
    delete skel;

	// Delete the VBOs and the VAO.
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void Skin::Draw(const glm::mat4& viewProjMtx, GLuint shader, GLFWwindow* window)
{
	// actiavte the shader program 
	glUseProgram(shader);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);

    this->renderImGui(window);
}

void Skin::Update(glm::mat4 topMatrix)
{
    // Update skeleton before updating skin vertices
    this->getSkeleton()->Update(topMatrix);

    std::vector<glm::mat4> M; 
    for (int k = 0; k < invBindings.size(); k++) {
        M.push_back(this->getWorldMatrix(k) * invBindings[k]);
    }
    
    // compute the skinning matrix M and update v and n 
    for (int i = 0; i < position.size(); i++) {
        glm::vec3 updatedVertex(.0f);
        glm::vec3 updatedNormal(.0f);
        for (int j = 0; j < skinWeights[i].size(); j++) {
            int jointIdx = skinWeights[i][j].first;
            float weight = skinWeights[i][j].second;
            //glm::mat4 worldm(this->getWorldMatrix(jointIdx));
            // update v to v'
            updatedVertex += glm::vec3( weight * M[jointIdx] * glm::vec4(position[i], 1.0f) );
            // update normal n to n'
            updatedNormal += glm::vec3(weight * glm::inverse(glm::transpose(M[jointIdx])) * glm::vec4(normal[i], .0f));
            // re-normalize the normal
            updatedNormal = glm::normalize(updatedNormal);
        }
        new_position[i] = updatedVertex;
        new_normal[i] = updatedNormal;
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * new_position.size(), new_position.data(), GL_DYNAMIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * new_normal.size(), new_normal.data(), GL_DYNAMIC_DRAW);

    // Unbind the VBOs
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Skin::spin(float deg)
{
	model = model * glm::rotate(glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}

bool Skin::Load(const char* fileName)
{
    Tokenizer token;
    token.Open(fileName);

    while (1) {
        // assume token always <= 256
        char temp[256];
        token.GetToken(temp);
        if (strcmp(temp, "positions") == 0) {
            int size = token.GetInt();
            // prep the vertices in the array
            //for (int k = 0; k < size; k++) {
            //    vertices.getVertices().push_back(glm::vec3(.0f));
            //}

            token.FindToken("{");
            for (int i = 0; i < size; i++) {
                float x = token.GetFloat();
                float y = token.GetFloat();
                float z = token.GetFloat();

                //vertices->getVertices().push_back(glm::vec3(x, y, z));

                position.push_back(glm::vec3(x, y, z));
                new_position.push_back(glm::vec3(x, y, z));
            }
        }

        else if (strcmp(temp, "normals") == 0) {
            int size = token.GetInt();
            token.FindToken("{");
            for (int i = 0; i < size; i++) {
                float x = token.GetFloat();
                float y = token.GetFloat();
                float z = token.GetFloat();

                //vertices->getNormals().push_back(glm::vec3(x, y, z));

                normal.push_back(glm::vec3(x, y, z));
                new_normal.push_back(glm::vec3(x, y, z));
            }
        }

        else if (strcmp(temp, "skinweights") == 0) {
            int size = token.GetInt();
            token.FindToken("{");
            for (int i = 0; i < size; i++) {
                int attachSize = token.GetInt();
                std::vector<std::pair<int, float>>vec;
                for (int j = 0; j < attachSize; j++) {
                    int jointNum = token.GetInt();
                    float weight = token.GetFloat();
                    vec.push_back(std::pair<int, float>(jointNum, weight));

                }
                //vertices->getSkinWeights().push_back(vec);
                skinWeights.push_back(vec);
            }

        }

        // parse triangles and create triangle class
        else if (strcmp(temp, "triangles") == 0) {
            int size = token.GetInt();
            token.FindToken("{");
            for (int i = 0; i < size; i++) {
                int tri1 = token.GetInt();
                int tri2 = token.GetInt();
                int tri3 = token.GetInt();

                /*triangles->getTriVerts().push_back(tri1);
                triangles->getTriVerts().push_back(tri2);
                triangles->getTriVerts().push_back(tri3);*/
                indices.push_back(tri1);
                indices.push_back(tri2);
                indices.push_back(tri3);
            }
        }

        // bindings seems to be the last item to parse, so we'll 
        // break the loop after this: 
        else if (strcmp(temp, "bindings") == 0) {
            int size = token.GetInt();
            token.FindToken("{");
            for (int i = 0; i < size; i++) {
                token.FindToken("{");
                glm::vec4 a(token.GetFloat(), token.GetFloat(), token.GetFloat(), .0f);
                glm::vec4 b(token.GetFloat(), token.GetFloat(), token.GetFloat(), .0f);
                glm::vec4 c(token.GetFloat(), token.GetFloat(), token.GetFloat(), .0f);
                glm::vec4 d(token.GetFloat(), token.GetFloat(), token.GetFloat(), 1.0f);

                bindings.push_back(glm::mat4(a, b, c, d));
            }
            // last one before breaking out of loop
            break;
        }

        else if (strcmp(temp, "}") == 0) {
            continue;
        }
    }

    token.Close();
    precomputeBindings();

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * new_position.size(), new_position.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * new_normal.size(), new_normal.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

    return true;
}

Skeleton* Skin::getSkeleton()
{
    return skel;
}

glm::mat4 Skin::getWorldMatrix(int jointIdx)
{
    return skel->getWorldMatrixBFS(jointIdx);
}

void Skin::precomputeBindings()
{
    for (int i = 0; i < bindings.size(); i++) {
        invBindings.push_back(glm::inverse(bindings[i]));
    }
}

void Skin::renderImGui(GLFWwindow* window)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Model Properties");
    }

    Skeleton* skeleton = this->getSkeleton();

    ImGui::ColorPicker4("color palette", (float*)&color);
    ImGui::SliderFloat("head X", &skeleton->getJointBFS(1)->getDofArray()[0]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("head Y", &skeleton->getJointBFS(1)->getDofArray()[1]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("head Z", &skeleton->getJointBFS(1)->getDofArray()[2]->getDOFValue(), -30.0f, 30.0f);

    ImGui::SliderFloat("Abdomen X", &skeleton->getJointBFS(2)->getDofArray()[0]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("Abdomen Y", &skeleton->getJointBFS(2)->getDofArray()[1]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("Abdomen Z", &skeleton->getJointBFS(2)->getDofArray()[2]->getDOFValue(), -30.0f, 30.0f);

    ImGui::SliderFloat("tail_01 X", &skeleton->getJointBFS(3)->getDofArray()[0]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("tail_01 Y", &skeleton->getJointBFS(3)->getDofArray()[1]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("tail_01 Z", &skeleton->getJointBFS(3)->getDofArray()[2]->getDOFValue(), -30.0f, 30.0f);

    ImGui::SliderFloat("tail_04 X", &skeleton->getJointBFS(6)->getDofArray()[0]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("tail_04 Y", &skeleton->getJointBFS(6)->getDofArray()[1]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("tail_04 Z", &skeleton->getJointBFS(6)->getDofArray()[2]->getDOFValue(), -30.0f, 30.0f);

    ImGui::SliderFloat("wing_02_r X", &skeleton->getJointBFS(13)->getDofArray()[0]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("wing_02_r Y", &skeleton->getJointBFS(13)->getDofArray()[1]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("wing_02_r Z", &skeleton->getJointBFS(13)->getDofArray()[2]->getDOFValue(), -30.0f, 30.0f);

    ImGui::SliderFloat("wing_01_r X", &skeleton->getJointBFS(14)->getDofArray()[0]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("wing_01_r Y", &skeleton->getJointBFS(14)->getDofArray()[1]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("wing_01_r Z", &skeleton->getJointBFS(14)->getDofArray()[2]->getDOFValue(), -30.0f, 30.0f);

    ImGui::SliderFloat("wing_01_l X", &skeleton->getJointBFS(21)->getDofArray()[0]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("wing_01_l Y", &skeleton->getJointBFS(21)->getDofArray()[1]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("wing_01_l Z", &skeleton->getJointBFS(21)->getDofArray()[2]->getDOFValue(), -30.0f, 30.0f);

    ImGui::SliderFloat("wing_02_l X", &skeleton->getJointBFS(22)->getDofArray()[0]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("wing_02_l Y", &skeleton->getJointBFS(22)->getDofArray()[1]->getDOFValue(), -30.0f, 30.0f);
    ImGui::SliderFloat("wing_02_l Z", &skeleton->getJointBFS(22)->getDofArray()[2]->getDOFValue(), -30.0f, 30.0f);

    {
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
