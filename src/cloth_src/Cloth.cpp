#include "Cloth.h"

Cloth::Cloth(glm::vec3 pForce, glm::vec3 pVelo, float pMass, int gSize)
{
	gridSize = gSize;
	model = glm::mat4(1.0f);
	color = glm::vec3(1.0f, 1.0f, 1.0f);
	//springConst = 50.0f; // 35 
	//dampConst = 30.0f; // 30
	springConst = 200.0f; // viable values: Ks = 100.0f, Kd = 50.0f, mass = 1.1f, Cd = 1.020f
	dampConst = 30.0f; // another viable set of values: Ks = 50.0f, Kd = 30.0f, mass = 1.1f (but a bit elastic-ey), Cd = 1.020f
	vAir = glm::vec3(15.0f, .0f, -3.0f);
	Cd = 1.020f;
	rho = 1.225;
	rest_const = 0.05f;
	dynamic_fric = 0.60f;

	vAirHolder = glm::vec3(.0f);
	gotWind = true;
	transl = glm::mat4(1.0f);

	// SPRINGS: make edge from curr pt and one-right of it, and curr pt and one-down
	// *2 for horiz and vertical springs
	numSprings = gridSize * (gridSize - 1)*2;

	// SPRINGS: account for diagonal springs, *2 for both diagonals
	numSprings += (gridSize - 1) * (gridSize - 1) * 2;

	// Given a gridSize, we want to set the positions of the particles here 
	for (int i = gridSize - 1; i >= 0; i--) {
		for (int j = 0; j < gridSize; j++) {
			// force, velo, pos, normal, mass
			// force and velo default to 0
			float randVal = (rand() % 100 + 1) / 1000.0f;
			particles.push_back(new Particle(pForce, pVelo, 
				glm::vec3((float(j) - ((float(gridSize) - 1.0f) / 2.0f)) / 5.0f, (float(i) - ((float(gridSize)- 1.0f) / 2.0f)) / 5.0f, randVal), glm::vec3(.0f,.0f, 1.0f) ,pMass));
			// store the top row positions into this array for rotation
			originPts.push_back(glm::vec3((float(j) - ((float(gridSize) - 1.0f) / 2.0f)) / 5.0f, (float(i) - ((float(gridSize)- 1.0f) / 2.0f)) / 5.0f, randVal));
			// We've pushed in gridSize * gridSize # of particles
			// with their positions and normals
		}
	}

	// Make sure force doesn't apply on the top row of particles
	for (int j = 0; j < gridSize; j++) {
		particles[j]->setForceApply(false);
	}

	// Create a vector of positions that reflects cloth positions always
	// So, particlePos will always reflect the positions in each of
	// the Particle objects in the vector<Particle>
	for (int m = 0; m < particles.size(); m++) {
		particlePos.push_back(particles[m]->getPos());
		particleNorm.push_back(particles[m]->getNorm());
	}

	// Tri-areas:
	// all triangles here have the same area since they are squares
	// so we can compute once and assign same for all 
	//float area = 0.5f*glm::length(glm::cross(particles[0]->getPos() - particles[1]->getPos(), (particles[gridSize]->getPos() - particles[1]->getPos())));
		
	// account for normals index 
	// and create triangles 
	for (int i = 0; i < gridSize - 1; i++) {
		for (int j = 0; j < gridSize - 1; j++) {
			// get indices for tri 1
			triIndices.push_back(i * gridSize + j);
			triIndices.push_back(i * gridSize + j + 1);
			triIndices.push_back((i+1) * gridSize + j);
			// make triangle pointing to particles for this triangle
			triangles.push_back(
				Triangle(particles[i * gridSize + j], 
					particles[i*gridSize + j + 1], 
					particles[(i+1)*gridSize + j])
			);
				
			// tri 2
			triIndices.push_back(i * gridSize + j + 1);
			triIndices.push_back((i+1) * gridSize + j + 1);
			triIndices.push_back((i+1) * gridSize + j);
			triangles.push_back(
			// make triangle pointing to particles for this triangle
				Triangle(particles[i * gridSize + j + 1], 
					particles[(i+1) * gridSize + j + 1], 
					particles[(i+1)*gridSize + j])
			);

		}
	}

	// Link up springs to corresponding portion 
	//auto spring_iter = springs.begin();
	// link horizontal springs
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize - 1; j++) {
			Particle* point1 = particles[(i * gridSize) + j];
			Particle* point2 = particles[(i * gridSize) + j + 1];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			// Ks, Kd, spr_nat_length, * p1, * p2
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
			//spring_iter++;
		}
	}

	// link vertical springs
	for (int i = 0; i < gridSize - 1; i++) {
		for (int j = 0; j < gridSize; j++) {
			Particle* point1 = particles[(i * gridSize) + j]; 
			Particle* point2 = particles[((i + 1) * gridSize) + j];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
			//spring_iter++;
		}
	}

	// link diagonal from top left to btm right
	for (int i = 0; i < gridSize - 1; i++) {
		for (int j = 0; j < gridSize - 1; j++) {
			Particle* point1 = particles[(i * gridSize) + j]; 
			Particle* point2 = particles[((i + 1) * gridSize) + j + 1];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
			//spring_iter++;
		}
	}

	// link diagonal from btm left to top right
	for (int i = 0; i < gridSize - 1; i++) {
		for (int j = 1; j < gridSize; j++) {
			Particle* point1 = particles[(i * gridSize) + j]; 
			Particle* point2 = particles[((i + 1) * gridSize) + j - 1];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
			//spring_iter++;
		}
	}

	// link diagonal from btm left to top right
	for (int i = 0; i < gridSize - 1; i++) {
		for (int j = 1; j < gridSize; j++) {
			Particle* point1 = particles[(i * gridSize) + j]; 
			Particle* point2 = particles[((i + 1) * gridSize) + j - 1];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
			//spring_iter++;
		}
	}
	
	// link curr to 2 particles to its right
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize - 2; j++) {
			Particle* point1 = particles[(i * gridSize) + j]; 
			Particle* point2 = particles[(i * gridSize) + j + 2];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
		}
	}

	// link curr to 2 particles to its bottom
	for (int i = 0; i < gridSize - 2; i++) {
		for (int j = 0; j < gridSize; j++) {
			Particle* point1 = particles[(i * gridSize) + j]; 
			Particle* point2 = particles[((i+2) * gridSize) + j];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
		}
	}

	// All springs have been linked up at this point

	// initialize the grid using openGL
	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_pos);
	glGenBuffers(1, &VBO_normals);

	// Bind the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * particlePos.size(), particlePos.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind the 2nd VBO - Used to store normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * particleNorm.size(), particleNorm.data(), GL_STATIC_DRAW); 
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

Cloth::~Cloth()
{
	for (int i = 0; i < particles.size(); i++) {
		delete particles[i];
	}
	for (int k = 0; k < springs.size(); k++) {
		delete springs[k];
	}
}

void Cloth::Draw(const glm::mat4& viewProjMtx, GLuint shader, GLFWwindow* window)
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

	this->renderImGui(window);
}

void Cloth::renderImGui(GLFWwindow* window)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Cloth Simulation Options");
		ImGui::SliderFloat("Air Velo X", &getWindVelo().x, -15.0f, 15.0f);
		ImGui::SliderFloat("Air Velo Y", &getWindVelo().y, -15.0f, 15.0f);
		ImGui::SliderFloat("Air Velo Z", &getWindVelo().z, -15.0f, 15.0f);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    //glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Updates all the forces acting on the cloth particles
// by iterating thru every particle each update
void Cloth::Update(float delta_t, glm::vec3 g, FloorTile* floor, int steps)
{
	// Update cloth pos and norm accordingly with
	// particlePos and particleNorm
	for (int j = 0; j < steps; j++) {
		// for each particle, apply gravity --
		// and zero out the normal for aerodynamic part below
		for (int i = 0; i < particles.size(); i++) {
			particles[i]->applyForce(particles[i]->getMass() * g);
			// zero out normal for tri-norm computation
			particles[i]->setNorm(glm::vec3(.0f));
		}

		// Apply spring-damper forces on each particle by looping thru springs
		// for each spring, apply spring-damp force on each particle
		for (int i = 0; i < springs.size(); i++) {
			// implicitly calls applyForce()
			springs[i]->computeForce();
		}

		// Apply aerodynamic forces on ea particle by looping thru 
		// every triangle 

		// step 1: zero out each particle's normals (done in the grav. pass above)
		// step 2: reassign them by summing-- for each triangle..
		for (int i = 0; i < triangles.size(); i++) {
			// do I have to zero out the normal before re-assigning it
			// for each particle? Yes, because you're summing a particle's normals
			// based on each of its neighboring triangles
			triangles[i].calcNormal();
			glm::vec3 triN = triangles[i].getNormal();
			triangles[i].getP1()->addNorm(triN);
			triangles[i].getP2()->addNorm(triN);
			triangles[i].getP3()->addNorm(triN);
			// Apply Aerodynamic force on p1 p2 p3
			glm::vec3 aeroF = triangles[i].calcAeroForce(this->vAir, this->rho, this->Cd) / 3.0f;
			if (gotWind) {
				triangles[i].getP1()->applyForce(aeroF);
				triangles[i].getP2()->applyForce(aeroF);
				triangles[i].getP3()->applyForce(aeroF);
			}
		}

		// iterate thru particles and normalize normals and finally:
		// integrate the motion in this pass
		for (int i = 0; i < particles.size(); i++) {
			particles[i]->setNorm(glm::normalize(particles[i]->getNorm()));
			particleNorm[i] = particles[i]->getNorm();
			// integ. motion
			particlePos[i] = particles[i]->integrateMotion(delta_t);

			// After integrating motion, check if collided with objects 
			// in this case, FloorTile
			if (particlePos[i].y < floor->getYPos()) {
				// shift the y pos of the cloth vertex up to
				// the surface

				// calculate impulse, as well as the tangential impulse,
				// and add to particle's velocity
				// calc closing velo: 
				float v_close = glm::dot(
					(particles[i]->getVelo() - floor->getVelo()), 
					floor->getNormal()
				);
				// calculate impulse: 
				glm::vec3 impulse(
					-1.0f * (1 + rest_const) * particles[i]->getMass() * 
					v_close * floor->getNormal());

				// compute frictional impulse -- parallel to plane
				// find tangential velo and flip value: 
				glm::vec3 v_norm(glm::dot(particles[i]->getVelo(), 
					floor->getNormal()) * floor->getNormal());
				glm::vec3 tan_impulse_dir(-1.0f * 
					glm::normalize(particles[i]->getVelo() - v_norm));
				glm::vec3 frictional_impulse(dynamic_fric * 
					glm::length(impulse) * tan_impulse_dir);

				// add these to the particle's velo: 
				particles[i]->setVelo(
					particles[i]->getVelo() + 
					((impulse + frictional_impulse) / particles[i]->getMass()));

				particles[i]->setPos(glm::vec3(particles[i]->getPos().x, 
					particles[i]->getPos().y + 
					(floor->getYPos() - 1.0f * particlePos[i].y) + 0.001f, 
					particles[i]->getPos().z));

				particlePos[i].y = particlePos[i].y + 
					(floor->getYPos() - 1.0f * particlePos[i].y) + 0.001f;
			}
		}
	}

    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * particlePos.size(), particlePos.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * particleNorm.size(), particleNorm.data(), GL_DYNAMIC_DRAW);

    // Unbind the VBOs
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Cloth::togglePos(glm::vec3 moveAmt)
{
	glm::mat4 moveMat = glm::translate(glm::mat4(1.0f), moveAmt);
	transl *= moveMat;
	for (int i = 0; i < gridSize; i++) {
		glm::vec3 newPos = moveMat * glm::vec4(particles[i]->getPos(), 1.0f);
		particles[i]->setPos(newPos);
	}
}

bool Cloth::toggleWind()
{
	this->gotWind = !this->gotWind;
	return this->gotWind;
}

void Cloth::windSpeed(float magnitude)
{
	vAir += magnitude * glm::vec3(1.0f);
}

glm::vec3& Cloth::getWindVelo()
{
	return this->vAir;
}

void Cloth::spin(float deg)
{
	// Update the model matrix by multiplying a rotation matrix
	//model = model * glm::rotate(glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
	for (int i = 0; i < gridSize; i++) {
		glm::mat4 rot(glm::rotate(glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::mat4 newTransl(glm::inverse(transl));
		glm::vec3 nPos = transl * rot * newTransl * glm::vec4(particles[i]->getPos(), 1.0f);
		particles[i]->setPos(nPos);
		particlePos[i] = nPos;
	}
}


