#include "ParticleSys.h"

ParticleSys::ParticleSys(int num_particles)
{
	numParticles = num_particles;
	lastUsedParticleIdx = 0;
	particlesPerSecond = 10000;
	roundOffError = .0f;

	mass = 5.0f;
	originPos = glm::vec3(.0f);

	dynamic_fric_coeff = 0.5f;
	restitute_const = 0.8f;
	radius = 1.0f;
	color = glm::vec3( 174.0f / 255.0f, 198.0f / 255.0f, 207.0f / 255.0f);
	lifeSpan = 4.0f;

	gravity = glm::vec3(.0f, -9.81, .0f);
	rho = 1.225f;
	Cd = 0.47;
	vAir = glm::vec3(0.0f, .0f, .0f);

	model = glm::mat4(1.0f);
	tx = ty = tz = 1.0f;
	px = py = pz = .0f;
	sVariance = 1.0f;

	// do I initialize the particles here?
	// things to randomize: 
	// position, velocity

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_pos);
	glGenBuffers(1, &VBO_normals);

	// Bind the VAO
	glBindVertexArray(VAO);
	
	// Bind the positions VBO 
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);
	//glPointSize(30.0f);

	// Unbind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// unbind VAO
	glBindVertexArray(0); 
}

void ParticleSys::Draw(const glm::mat4& viewProjMtx, GLuint shader, GLFWwindow* window)
{	
	glPointSize(this->radius);
	glUseProgram(shader);
	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	glDrawArrays(GL_POINTS, 0, positions.size() - 1);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);

	this->renderImGui(window);
}

void ParticleSys::Update(float deltaTime, FloorTile* floor)
{
	float num = deltaTime * particlesPerSecond + roundOffError;
	int new_particles = (int)num;
	roundOffError = num - (float)new_particles;

	int lastSizeIdx = particles2.size(); 

	// create particles:
	// iterate until all new particles are added
	for (int i = 0; i < new_particles; i++) {
		// find idx of particle:
		//int idx = findLastUsedParticleIdx();
		if (particles2.size() < 1000) {
			// obtain randomized velo vector: 
			float s = (rand() % 10001) / 10000.0f;
			float t = (rand() % 10001) / 10000.0f;
			float u = 2 * glm::pi<float>() * s;
			float v = sqrt(t * (1 - t));
			float dx = 2 * v * cos(u);
			float dy = 1 - (2 * t);
			float dz = 2 * v * sin(u);
			glm::vec3 velo(tx * dx, ty * dy, tz * dz);
		
			glm::vec3 sourcePos(this->originPos.x + dx * px, this->originPos.y + dy * py, this->originPos.z + dz* pz);
			// create particles
			// set all values for this particle: force, velo, position, norm, mass(randomized)
			Particle p1(glm::vec3(.0f), velo, sourcePos, glm::vec3(.0f, 1.0f, .0f), this->mass);
			p1.setLifeSpan(this->lifeSpan * s * sVariance);
			particles2.push_back(p1);
			float mini_step = (rand() % 10001)/ 10000.0f; 

			// apply forces, integrate motion, check collisions
			// apply grav force
			particles2[i].applyForce(this->mass * this->gravity); // grav force
			// apply drag force
			glm::vec3 dragV = particles2[i].getVelo() - this->vAir;
			glm::vec3 e = (-1.0f * dragV) / glm::length(dragV);
			particles2[i].applyForce(0.5f * this->rho * pow(glm::length(dragV), 2.0f) 
				* Cd * glm::pi<float>() * this->radius * this->radius * e);
			// integrate motion
			//particles2[i].integrateMotion(deltaTime);

			// check collision

			// transfer over into positions, normals and indices
			positions.push_back(particles2[i].getPos());		
			normals.push_back(glm::vec3(.0f, 1.0f, .0f));
			indices.push_back(i);
		}
	
	}

	// update particles 
	for (int i = 0; i < particles2.size(); i++) {
		// check if they have < 0 life, if so, replace the particle with one from
		// the end. Pop that one at the end
		
		// decrement their lifeSpans
		particles2[i].setLifeSpan(particles2[i].getLifeSpan() - deltaTime);
		// check if the particle is 'dead' or 'alive'
		if (particles2[i].getLifeSpan() < .0f) {
			particles2[i] = particles2.back(); 
			particles2.pop_back();

			positions.pop_back();

			normals.pop_back();
			indices.pop_back();
		}
		else {
			// if particle is still alive, apply forces:
			// apply grav force
			particles2[i].applyForce(this->mass * gravity); // grav force
			// apply drag force
			glm::vec3 dragV = particles2[i].getVelo() - this->vAir;
			glm::vec3 e = (-1.0f * dragV) / glm::length(dragV);
			particles2[i].applyForce(0.5f * rho * pow(glm::length(dragV), 2.0f) 
				* Cd * glm::pi<float>() * this->radius * this->radius * e);
		}
		// integrate motion of all particles
		particles2[i].integrateMotion(deltaTime);

		// account for collisions
		// if the particles exceed the y position of the floor
		if (particles2[i].getPos().y < floor->getYPos()) {
			// reposition them back up, and add to them a reflection impulse
			// and friction impulse
			
			// calculate impulse, as well as the tangential impulse, and add to particle's velocity
			// calc closing velo: 
			float v_close = glm::dot((particles2[i].getVelo() - floor->getVelo()), floor->getNormal());
			// calculate impulse: 
			glm::vec3 impulse = -1.0f * (1 + restitute_const) * particles2[i].getMass() * v_close * floor->getNormal();
			// compute frictional impulse -- parallel to plane
			// find tangential velo and flip value: 
			glm::vec3 v_norm = glm::dot(particles2[i].getVelo(), floor->getNormal()) * floor->getNormal();
			glm::vec3 tan_impulse_dir = -1.0f * glm::normalize(particles2[i].getVelo() - v_norm);
			glm::vec3 frictional_impulse = dynamic_fric_coeff * glm::length(impulse)* tan_impulse_dir;
			
			// add these to the particle's velo: 
			particles2[i].setVelo(particles2[i].getVelo() + ((impulse + frictional_impulse) / particles2[i].getMass()) );
			// shift the particle back up to the floor level 
			particles2[i].setPos(glm::vec3(particles2[i].getPos().x, particles2[i].getPos().y + (floor->getYPos() - 1.0f * particles2[i].getPos().y) + 0.001f, particles2[i].getPos().z));
		}

		// transfer all particle pos to array to be drawn
		positions[i] = particles2[i].getPos();
	}

    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_DYNAMIC_DRAW);

    // Unbind the VBOs
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void ParticleSys::renderImGui(GLFWwindow* window)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Particle System Options");
		ImGui::SliderInt("Particle Creation Rate", &getCreationRate(), 0, 30000);
		ImGui::SliderFloat("X Pos", &getOriginPos().x, -15.0f, 15.0f);
		ImGui::SliderFloat("Y Pos", &getOriginPos().y, -15.0f, 15.0f);
		ImGui::SliderFloat("Z Pos", &getOriginPos().z, -15.0f, 15.0f);

		ImGui::SliderFloat("X Pos variance", &getPx(), -15.0f, 15.0f);
		ImGui::SliderFloat("Y Pos variance", &getPy(), -15.0f, 15.0f);
		ImGui::SliderFloat("Z Pos variance", &getPz(), -15.0f, 15.0f);

		ImGui::SliderFloat("X Velocity", &getWindVelocity().x, -25.0f, 25.0f);
		ImGui::SliderFloat("Y Velocity", &getWindVelocity().y, -25.0f, 25.0f);
		ImGui::SliderFloat("Z Velocity", &getWindVelocity().z, -25.0f, 25.0f);

		ImGui::SliderFloat("X Velo variance", &getTx(), -15.0f, 15.0f);
		ImGui::SliderFloat("Y Velo variance", &getTy(), -15.0f, 15.0f);
		ImGui::SliderFloat("Z Velo variance", &getTz(), -15.0f, 15.0f);

		ImGui::SliderFloat("Life Span", &getLifeSpan(), 0.0f, 15.0f);
		ImGui::SliderFloat("Life Span Variance", &getLifeVariance(), 0.0f, 15.0f);
		ImGui::SliderFloat("Gravity", &getGravity(), -25.0f, 25.0f);
		ImGui::SliderFloat("Air Density", &getAirDensity(), .0f, 5.0f);
		ImGui::SliderFloat("Drag Coefficient", &getDragCoeff(), .0f, 5.0f);
		ImGui::SliderFloat("Particle Radius", &getParticleRadius(), .0f, 100.0f);
		ImGui::SliderFloat("Collision Elasticity", &getCollisionElasticity(), .0f, 1.0f);
		ImGui::SliderFloat("Collision Friction", &getFricCoeff(), .0f, 1.0f);

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

int ParticleSys::findLastUsedParticleIdx()
{
	for (int i = lastUsedParticleIdx; i < numParticles; i++) {
		if (particles2[i].getLifeSpan() < .0f) {
			lastUsedParticleIdx = i;
			return i;
		}
	}
	
	for (int i = 0; i < lastUsedParticleIdx; i++) {
		if (particles2[i].getLifeSpan() < .0f) {
			lastUsedParticleIdx = i;
			return i;
		}
	}
	// default: update the first particle if all
	// particles have lifespan in them
	return 0;
}

int& ParticleSys::getCreationRate()
{
	return particlesPerSecond;
}

glm::vec3& ParticleSys::getOriginPos()
{
	return originPos;
}

glm::vec3& ParticleSys::getWindVelocity()
{
	return vAir;
}

float& ParticleSys::getLifeSpan()
{
	return lifeSpan;
}

float& ParticleSys::getGravity()
{
	return gravity.y;
}

float& ParticleSys::getAirDensity()
{
	return rho;
}

float& ParticleSys::getDragCoeff()
{
	return Cd;
}

float& ParticleSys::getParticleRadius()
{
	return radius;
}

float& ParticleSys::getCollisionElasticity()
{
	return restitute_const;
}

float& ParticleSys::getFricCoeff()
{
	return dynamic_fric_coeff;
}

float& ParticleSys::getTx()
{
	return tx;
}

float& ParticleSys::getTy()
{
	return ty;
}

float& ParticleSys::getTz()
{
	return tz;
}

float& ParticleSys::getPx()
{
	return px;
}

float& ParticleSys::getPy()
{
	return py;
}

float& ParticleSys::getPz()
{
	return pz;
}

float& ParticleSys::getLifeVariance()
{
	return sVariance;
}
