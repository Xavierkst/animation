#include "FloorTile.h"

FloorTile::FloorTile(int floor_size)
{	
	floorSize = floor_size;
	color = glm::vec3(0.5f, 0.5f, 0.5f);
	model = glm::mat4(1.0f);
	velo = glm::vec3(.0f);

	for (int i = floorSize - 1; i >= 0; i--) {
		for (int j = 0; j < floorSize; j++) {
			// force, velo, pos, normal, mass
			// force and velo default to 0
			
			pos.push_back(glm::vec3((float(j) - ((float(floorSize) - 1.0f) / 2.0f)), 
				 -4.5f, (float(i) - ((float(floorSize) - 1.0f) / 2.0f))));
			normals.push_back(glm::vec3(.0f, 1.0f, .0f));
		}
	}

	for (int i = 0; i < floorSize - 1; i++) {
		for (int j = 0; j < floorSize - 1; j++) {
			// get indices for tri 1
			triIndices.push_back(i * floorSize + j);
			triIndices.push_back(i * floorSize + j + 1);
			triIndices.push_back((i+1) * floorSize + j);
			// make triangle pointing to particles for this triangle
				
			// tri 2
			triIndices.push_back(i * floorSize + j + 1);
			triIndices.push_back((i+1) * floorSize + j + 1);
			triIndices.push_back((i+1) * floorSize + j);
		}
	}

	// initialize the grid using openGL
	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_pos);
	glGenBuffers(1, &VBO_normals);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * pos.size(), pos.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind the 2nd VBO - Used to store normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW); 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * triIndices.size(), triIndices.data(), GL_STATIC_DRAW);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}



float FloorTile::getYPos()
{
	return pos[0].y;
}

void FloorTile::Draw(const glm::mat4& viewProjMtx, GLuint shader)
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
	glDrawElements(GL_TRIANGLES, triIndices.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_POINTS, 0, triIndices.size());

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

glm::vec3 FloorTile::getVelo()
{
	return velo;
}

glm::vec3 FloorTile::getNormal()
{
	// since all normals on the floor are same, just use 
	// any one
	return normals[0];
}
