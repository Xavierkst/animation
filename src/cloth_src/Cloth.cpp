#include "Cloth.h"

Cloth::Cloth(glm::vec3 pForce, glm::vec3 pVelo, float pMass, int gSize, const char* computeShaderPath): model(glm::mat4(1.0f)), color(glm::vec3(1.0f)), gridSize(gSize)
{
	// viable values: Ks = 100.0f, Kd = 50.0f, mass = 1.1f, Cd = 1.020f
	// Another viable set of values (?) Ks = 50.0f, Kd = 30.0f, mass = 1.1f, Cd = 1.020f
	renderProg.LoadShaders("src/shaders/clothShader.vert", "src/shaders/clothShader.frag");	

	vAir = glm::vec3(15.0f, .0f, -3.0f); // air velocity
	// Other physical constants
	springConst = 200.0f; 
	dampConst = 30.0f; 
	Cd = 1.020f;
	rho = 1.225;
	rest_const = 0.05f;
	dynamic_fric = 0.60f;

	gotWind = true;

	// SPRINGS: make edge from curr pt and one-right of it, and curr pt and one-down
	// *2 for horiz and vertical springs
	numSprings = gridSize * (gridSize - 1)*2;

	// SPRINGS: account for diagonal springs, *2 for both diagonals
	numSprings += (gridSize - 1) * (gridSize - 1) * 2;

	// Given a gridSize, we want to set the positions of the particles here 
	for (int i = gridSize - 1; i >= 0; i--) {
		for (int j = 0; j < gridSize; j++) {
			// force and velo default to 0
			// randomized z-component starter values
			float randVal = (rand() % 100 + 1) / 1000.0f; 
			particles.push_back(new Particle(pForce, pVelo, 
				glm::vec3((float(j) - ((float(gridSize) - 1.0f) / 2.0f)) / 5.0f, (float(i) - ((float(gridSize)- 1.0f) / 2.0f)) / 5.0f, randVal), glm::vec3(.0f,.0f, 1.0f) ,pMass));
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
			triangles.push_back(Triangle(particles[i * gridSize + j], 
					particles[i*gridSize + j + 1], 
					particles[(i+1)*gridSize + j]));
				
			// tri 2
			triIndices.push_back(i * gridSize + j + 1);
			triIndices.push_back((i+1) * gridSize + j + 1);
			triIndices.push_back((i+1) * gridSize + j);
			triangles.push_back(Triangle(particles[i * gridSize + j + 1], 
					particles[(i+1) * gridSize + j + 1], 
					particles[(i+1)*gridSize + j]));

		}
	}

	// Link up springs to corresponding portion:

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

	// send the positions to the position and velocity buffers:
	this->posBufs.push_back(std::vector<glm::vec4>());
	this->posBufs.push_back(std::vector<glm::vec4>());
	this->veloBufs.push_back(std::vector<glm::vec4>());
	this->veloBufs.push_back(std::vector<glm::vec4>());
	for (int i = 0; i < particlePos.size(); ++i) {
		posBufs[0].push_back(glm::vec4(particlePos[i], 1.0f));
		posBufs[1].push_back(glm::vec4(particlePos[i], 1.0f));
		veloBufs[0].push_back(glm::vec4(0.0f));
		veloBufs[1].push_back(glm::vec4(0.0f));
	}

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

	// Create compute shader
	this->nParticles.x = 30;
	this->nParticles.y = 30;
	this->readBuf = 0;
	this->computeShaderProgram = glCreateProgram();
	this->computeShader = glCreateShader(GL_COMPUTE_SHADER);
	// Try to read shader codes from the shader file.
	std::string shaderCode;
	std::ifstream shaderStream(computeShaderPath, std::ios::in);
	if (shaderStream.is_open()) 
	{
		std::string Line = "";
		while (getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}
	else 
	{
		std::cerr << "Impossible to open " << computeShaderPath << ". "
			<< "Check to make sure the file exists and you passed in the "
			<< "right filepath!"
			<< std::endl;
		return;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Shader.
	std::cerr << "Compiling compute shader: " << computeShaderPath << std::endl;
	char const * sourcePointer = shaderCode.c_str();
	glShaderSource(computeShader, 1, &sourcePointer, NULL);
	glCompileShader(computeShader);
	
	// Check Shader.
	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(computeShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		std::vector<char> shaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(computeShader, InfoLogLength, NULL, shaderErrorMessage.data());
		std::string msg(shaderErrorMessage.begin(), shaderErrorMessage.end());
		std::cerr << msg << std::endl;
		return;
	}

	glAttachShader(computeShaderProgram, computeShader);
	glLinkProgram(computeShaderProgram);
	
	// Check the program.
	glGetProgramiv(computeShaderProgram, GL_LINK_STATUS, &Result);
	glGetProgramiv(computeShaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(computeShaderProgram, InfoLogLength, NULL, ProgramErrorMessage.data());
		std::string msg(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
		std::cerr << msg << std::endl;
		glDeleteProgram(computeShaderProgram);
		return;
	}
	else
	{
		printf("Successfully linked program!\n");
	}

	// glDetachShader(computeShaderProgram, computeShader);
	// glDeleteShader(computeShader);

	// Generate the buffers to read and write from in the Compute shader
	glGenBuffers(1, &computeBufPos[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computeBufPos[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * posBufs[0].size(), &posBufs[0], GL_DYNAMIC_READ);
	glGenBuffers(1, &computeBufPos[1]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computeBufPos[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * posBufs[1].size(), &posBufs[1], GL_DYNAMIC_READ);

	glGenBuffers(1, &computeBufVelo[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, computeBufVelo[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * veloBufs[0].size(), &veloBufs[0], GL_DYNAMIC_READ);
	glGenBuffers(1, &computeBufVelo[1]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, computeBufVelo[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * veloBufs[1].size(), &veloBufs[1], GL_DYNAMIC_READ);

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

	glDeleteProgram(renderProg.ID);
}

void Cloth::Draw(const glm::mat4& viewProjMtx, GLFWwindow* window)
{
	// activate the shader program 
	renderProg.use();
	
	// light uniform values
	renderProg.setFloat("point_light.k_constant", 1.0f);
	renderProg.setFloat("point_light.k_linear", 1.0f);
	renderProg.setFloat("point_light.k_quad", 1.0f);

	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, clothTextureID);

	// get the locations and send the uniforms to the shader 
	renderProg.setMat4("model", this->model);
	renderProg.setMat4("viewProj", viewProjMtx);
	renderProg.setVec3("material.diffuse_texture1", glm::vec3(0.5f));

	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, triIndices.size(), GL_UNSIGNED_INT, 0);

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
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Cloth::Update2() {
	glUseProgram(this->computeShaderProgram);
	int size = gridSize * gridSize;

	for (int i = 0; i < size; ++i) {
		glDispatchCompute((unsigned int)this->nParticles.x / 10, 
			(unsigned int)this->nParticles.y / 10, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// swapping buffers:
		readBuf = 1 - readBuf;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computeBufPos[readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computeBufPos[1-readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, computeBufVelo[readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, computeBufVelo[1-readBuf]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * posBufs[readBuf].size(), posBufs[readBuf].data(), GL_DYNAMIC_DRAW);

    // Unbind the VBOs
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

		// Apply aerodynamic forces on each particle by looping thru 
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
	model *= moveMat;
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
		glm::mat4 newTransl(glm::inverse(model));
		glm::vec3 nPos = model * rot * newTransl * glm::vec4(particles[i]->getPos(), 1.0f);
		particles[i]->setPos(nPos);
		particlePos[i] = nPos;
	}
}

void Cloth::setClothTextureID(GLuint texID)
{
	clothTextureID = texID;
}


