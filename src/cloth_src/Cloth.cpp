// we activate some section of the stb_image.h functions -- 
// essentially turning it into a .cpp file 
#define STB_IMAGE_IMPLEMENTATION
#include "Cloth.h"

// Constructor for simulation using CPU computed vertex attributes 
Cloth::Cloth(const char* computeShaderPath): model(glm::mat4(1.0f)), color(glm::vec3(1.0f))
{
	// Load vert and frag shaders only (constructor below also loads compute shader)
	renderProg.LoadShaders("src/shaders/clothShader.vert", "src/shaders/clothShader.frag");	

	glm::vec2 clothSize(4.0f, 4.0f);
	nParticles = glm::ivec2(30, 30);
	vAir = glm::vec3(0.001f, .001f, 0.001f); // air velocity

	// 1. viable values: Ks = 100.0f, Kd = 50.0f, mass = 1.1f, Cd = 1.020f
	// 2. Another viable set of values (?) Ks = 50.0f, Kd = 30.0f, mass = 1.1f, Cd = 1.020f
	// Constants: 
	springConst = 1200.0f; 
	dampConst = 30.0f; 
	Cd = 1.020f;
	rho = 1.225;
	rest_const = 0.05f;
	dynamic_fric = 0.60f;
	float pMass = 0.8f;
	gotWind = true;

	float dx = clothSize.x / (nParticles.x - 1);
	float dy = clothSize.y / (nParticles.y - 1);
	float ds = 1.0f / (nParticles.x - 1);
	float dt = 1.0f / (nParticles.y - 1);

	// Given a gridSize, we want to set the positions of the particles here 
	for (int i = nParticles.y - 1; i >= 0; i--) {
		for (int j = 0; j < nParticles.x; j++) {
			// force and velo default to 0
			// randomized z-component starter values
			// float randVal = (rand() % 100 + 1) / 1000.0f; 
			particles.push_back(new Particle(glm::vec3(.0f), glm::vec3(.0f), 
				glm::vec3((j - (nParticles.x -1) / 2.0f) * dx, i * dy * 2.0f, 0.0f), glm::vec3(.0f,.0f, 1.0f) ,pMass));
			// Push in texCoords
			texCoords.push_back(glm::ivec2(ds * j, dt * i));
		}
	}

	// Make sure force doesn't apply on the top row of particles
	for (int j = 0; j < nParticles.x; j++) {
		particles[j]->setForceApply(false);
	}

	// Create a vector of positions that reflects cloth positions always
	// So, particlePos will always reflect the positions in each of
	// the Particle objects in the vector<Particle>
	for (int m = 0; m < particles.size(); m++) {
		particlePos.push_back(particles[m]->getPos());
		particleNorm.push_back(particles[m]->getNorm());
	}

	// account for normals index 
	// and create triangles 
	for (int i = 0; i < nParticles.x - 1; i++) {
		for (int j = 0; j < nParticles.x - 1; j++) {
			// get indices for tri 1
			triIndices.push_back(i * nParticles.x + j);
			triIndices.push_back(i * nParticles.x + j + 1);
			triIndices.push_back((i+1) * nParticles.x + j);
			// make triangle pointing to particles for this triangle
			triangles.push_back(Triangle(particles[i * nParticles.x + j], 
					particles[i*nParticles.x + j + 1], 
					particles[(i+1)*nParticles.x + j]));
				
			// tri 2
			triIndices.push_back(i * nParticles.x + j + 1);
			triIndices.push_back((i+1) * nParticles.x + j + 1);
			triIndices.push_back((i+1) * nParticles.x + j);
			triangles.push_back(Triangle(particles[i * nParticles.x + j + 1], 
					particles[(i+1) * nParticles.x + j + 1], 
					particles[(i+1)*nParticles.x + j]));

		}
	}

	// Link up springs to corresponding portion:
	// link horizontal springs
	for (int i = 0; i < nParticles.x; i++) {
		for (int j = 0; j < nParticles.x - 1; j++) {
			Particle* point1 = particles[(i * nParticles.x) + j];
			Particle* point2 = particles[(i * nParticles.x) + j + 1];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			// Ks, Kd, spr_nat_length, * p1, * p2
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
			//spring_iter++;
		}
	}

	// link vertical springs
	for (int i = 0; i < nParticles.x - 1; i++) {
		for (int j = 0; j < nParticles.x; j++) {
			Particle* point1 = particles[(i * nParticles.x) + j]; 
			Particle* point2 = particles[((i + 1) * nParticles.x) + j];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
			//spring_iter++;
		}
	}

	// link diagonal from top left to btm right
	for (int i = 0; i < nParticles.x - 1; i++) {
		for (int j = 0; j < nParticles.x - 1; j++) {
			Particle* point1 = particles[(i * nParticles.x) + j]; 
			Particle* point2 = particles[((i + 1) * nParticles.x) + j + 1];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
			//spring_iter++;
		}
	}

	// link diagonal from btm left to top right
	for (int i = 0; i < nParticles.x - 1; i++) {
		for (int j = 1; j < nParticles.x; j++) {
			Particle* point1 = particles[(i * nParticles.x) + j]; 
			Particle* point2 = particles[((i + 1) * nParticles.x) + j - 1];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
			//spring_iter++;
		}
	}

	// link curr to 2 particles to its right
	for (int i = 0; i < nParticles.x; i++) {
		for (int j = 0; j < nParticles.x - 2; j++) {
			Particle* point1 = particles[(i * nParticles.x) + j]; 
			Particle* point2 = particles[(i * nParticles.x) + j + 2];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
		}
	}

	// link curr to 2 particles to its bottom
	for (int i = 0; i < nParticles.x - 2; i++) {
		for (int j = 0; j < nParticles.x; j++) {
			Particle* point1 = particles[(i * nParticles.x) + j]; 
			Particle* point2 = particles[((i+2) * nParticles.x) + j];
			float natLength = glm::length(point1->getPos() - point2->getPos());
			springs.push_back(new SpringDamper(springConst, dampConst, natLength, point1, point2));
		}
	}
	// All springs have been linked up at this point

	// initialize the grid using openGL
	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &clothVAO);
	glGenBuffers(1, &VBO_pos);
	glGenBuffers(1, &VBO_normals);
	glGenTextures(1, &clothTextureID);

	// Bind the VAO.
	glBindVertexArray(clothVAO);

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

	// Bind the texture coord buffer object
	clothTextureID = loadTexture("src/textures/redFabric.jpg");
	renderProg.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, clothTextureID);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glGenBuffers(1, &clothEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, clothEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * triIndices.size(), triIndices.data(), GL_STATIC_DRAW);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Constructor for Compute shader simulation
Cloth::Cloth(float dur, const char* computeShaderPath) {
	// Loads and compiles vert, frag, and compute shader
	this->LoadAndCompileShaders();

	readBuf = 0;
	glm::vec2 clothSize(4.0f, 4.0f);
	nParticles = glm::ivec2(30, 30);
	
	vAir = glm::vec3(15.0f, .0f, -3.0f); // air velocity
	// Other physical constants
	springConst = 200.0f; 
	dampConst = 30.0f; 
	Cd = 1.020f;
	rho = 1.225;
	rest_const = 0.05f;
	dynamic_fric = 0.60f;
	float pMass = 0.8f;
	gotWind = true;

	float dx = clothSize.x / (nParticles.x - 1);
	float dy = clothSize.y / (nParticles.y - 1);

	// Given a gridSize, we want to set the positions of the particles here 
	for (int i = nParticles.y - 1; i >= 0; i--) {
		for (int j = 0; j < nParticles.x; j++) {
			// force and velo default to 0
			// randomized z-component starter values
			float randVal = (rand() % 100 + 1) / 1000.0f; 
			// particles.push_back(new Particle(glm::vec3(.0f), glm::vec3(.0f), 
			// 	glm::vec3((float(j) - ((float(gridSize) - 1.0f) / 2.0f)) / 5.0f, (float(i) - ((float(gridSize)- 1.0f) / 2.0f)) / 5.0f, randVal), glm::vec3(.0f,.0f, 1.0f) ,pMass));
			glm::vec4 p = glm::vec4((j - (nParticles.x - 1) / 2.0f) * dx, i * dy * 2.0f, randVal, 1.0f);
			posBufs[0].push_back(p);
			posBufs[1].push_back(p);
			// initialize velocities
			veloBufs[0].push_back(glm::vec4(glm::vec3(.1f), 0.f));
			veloBufs[1].push_back(glm::vec4(glm::vec3(0.1f),.0f));
			normalBufs.push_back(glm::vec4(0.f, 0.f, 1.0f, .0f));
		}
	}

	// Populate index buffer for EBO 
	for (int row = 0; row < nParticles.y - 1; ++row) {
		for (int col = 0; col < nParticles.x - 1; ++col) {
			// tri 1
			idxBuf.push_back(row * nParticles.x + col);
			idxBuf.push_back(row * nParticles.x + col + 1);
			idxBuf.push_back((row+1) * nParticles.x + col);
			// tri 2
			idxBuf.push_back(row * nParticles.x + col + 1);
			idxBuf.push_back((row+1) * nParticles.x + col + 1);
			idxBuf.push_back((row+1) * nParticles.x + col);
		}
	}
	
	// Initialize the buffers (VAO, VBO, EBO... etc)
	this->initializeBuffers();

	// activate the shader program 
	renderProg.use();
	// pt light uniform values
	renderProg.setFloat("pt_light.k_constant", 1.0f);
	renderProg.setFloat("pt_light.k_linear", 1.0f);
	renderProg.setFloat("pt_light.k_quad", 1.0f);
	renderProg.setVec4("pt_light.position", glm::vec4(0.0f, 5.0f, -0.5f, 1.0f));
	renderProg.setVec3("pt_light.ambient", glm::vec3(0.2f, .2f, 0.2f));
	renderProg.setVec3("pt_light.diffuse", glm::vec3(0.8f, .8f, 0.8f));
	renderProg.setVec3("pt_light.specular", glm::vec3(1.0f));
	// dir light
	renderProg.setVec3("dir_light.ambient", glm::vec3(0.2f, .2f, 0.2f));
	renderProg.setVec3("dir_light.diffuse", glm::vec3(1.0f));
	renderProg.setVec3("dir_light.specular", glm::vec3(1.0f));
	// Material
	renderProg.setVec3("material.ambient", glm::vec3(1.0f));
	renderProg.setVec3("material.diffuse", glm::vec3(1.0f));
	renderProg.setVec3("material.specular", glm::vec3(1.0f));
}

Cloth::~Cloth()
{
	// Springs class doesn't delete both particles connected to it
	// Need to iterate thru particles arr to delete them 
	for (int k = 0; k < springs.size(); k++) {
		delete springs[k];
	}
	for (int i = 0; i < particles.size(); ++i) {
		delete particles[i];
	}

	glDeleteProgram(renderProg.ID);
}

// For the constructor that uses the compute shader
// Loads and compiles all shaders (vert, frag, compute)
void Cloth::LoadAndCompileShaders() {
	renderProg.LoadShaders("src/shaders/clothShader.vert", "src/shaders/clothShader.frag");	
	GLuint computeShaderID = renderProg.LoadSingleShader("src/shaders/clothCompute.comp", compute);
	renderProg.LinkShader(computeShaderID);
}

// For the constructor that uses the Compute shader 
void Cloth::initializeBuffers() {
	// Generate the VBOs and EBO 
	glGenBuffers(7, buff);
	computePosBuf[0] = buff[0];
	computePosBuf[1] = buff[1];
	computeVeloBuf[0] = buff[2];
	computeVeloBuf[1] = buff[3];
	normBuf = buff[4];
	clothEBO = buff[5];

	// PosBuffer 1 & 2
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computePosBuf[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 
		sizeof(glm::vec4) * posBufs[0].size(), posBufs[0].data(), GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computePosBuf[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 
	// NULL for 2nd pos buf since we're copying it over from GPU to shader 
		sizeof(glm::vec4) * posBufs[1].size(), NULL, GL_DYNAMIC_DRAW); 

	// VeloBuffer 1 & 2
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, computeVeloBuf[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * veloBufs[0].size(), veloBufs[0].data(), GL_DYNAMIC_COPY);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, computeVeloBuf[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * veloBufs[1].size(), NULL, GL_DYNAMIC_COPY);

	// Index Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, clothEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * idxBuf.size(), &idxBuf[0], GL_DYNAMIC_COPY);

	// Normal buffer
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, normBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)* normalBufs.size(), NULL, GL_DYNAMIC_COPY);

	// Generate, and set up VAO
	glGenVertexArrays(1, &clothVAO);
	glBindVertexArray(clothVAO);

	glBindBuffer(GL_ARRAY_BUFFER, computePosBuf[0]);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, normBuf);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, clothEBO);

	// Unbind the VAO
	glBindVertexArray(0);
}

// Render the cloth each frame 
void Cloth::Draw(const glm::vec3& camPos, const glm::mat4& viewProjMtx, GLFWwindow* window)
{
	// activate the shader program 
	renderProg.use();
	// Set the active texture unit 
	// (even tho by default tex-unit 0 is alr activated)
	// Material
	renderProg.setInt("material.texture_diffuse1", 0);
	renderProg.setInt("material.texture_specular1", 1);
	// renderProg.setFloat("material.texture_diffuse1", 1);
	renderProg.setFloat("material.shininess", 5);
	// renderProg.setFloat("material.texture_diffse1", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->clothTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->clothTextureID);

	// pt light uniform values
	renderProg.setFloat("pt_light.k_constant", 1.0f);
	renderProg.setFloat("pt_light.k_linear", 1.0f);
	renderProg.setFloat("pt_light.k_quad", 1.0f);
	renderProg.setVec4("pt_light.position", glm::vec4(0.0f, 5.0f, -0.5f, 1.0f));
	renderProg.setVec3("pt_light.ambient", glm::vec3(0.2f, .2f, 0.2f));
	renderProg.setVec3("pt_light.diffuse", glm::vec3(0.8f, .8f, 0.8f));
	renderProg.setVec3("pt_light.specular", glm::vec3(1.0f));
	// dir light
	renderProg.setVec3("dir_light.direction", glm::vec3(-2.0f, -8.0f, -4.0));
	renderProg.setVec3("dir_light.ambient", glm::vec3(0.2f, .2f, 0.2f));
	renderProg.setVec3("dir_light.diffuse", glm::vec3(0.8f, .8f, 0.8f));
	renderProg.setVec3("dir_light.specular", glm::vec3(1.0f));
	// renderProg.setVec3("material.diffuse", glm::vec3(1.0f));
	// renderProg.setVec3("material.specular", glm::vec3(1.0f));

	// get the locations and send the uniforms to the shader 
	renderProg.setMat4("model", this->model);
	renderProg.setMat4("viewProj", viewProjMtx);
	renderProg.setVec3("view_position", camPos);

	// Bind the VAO
	glBindVertexArray(clothVAO);
	glDrawElements(GL_TRIANGLES, triIndices.size(), GL_UNSIGNED_INT, 0);
	// glDrawElements(GL_TRIANGLES, idxBuf.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);

	this->renderImGui(window);
}

// Render the GUI interface with sliders for real-time toggling
void Cloth::renderImGui(GLFWwindow* window)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Cloth Simulation Options");
		ImGui::SliderFloat("Air Velo X", &getWindVelo().x, -3.0f, 3.0f);
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

// Updates all the forces acting on the cloth particles
// by iterating thru every particle each update
void Cloth::Update(FloorTile* floor, float delta_t, glm::vec3 g, int steps)
{
	
	// convert delta_t from ms to seconds and break dt into 'steps' number of time steps 
	float dt = delta_t / (float)(CLOCKS_PER_SEC * steps);
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

		// step 1: zero out each particle's normals to start from scratch (done in the grav. pass above)
		// step 2: reassign them by summing-- for each triangle..
		for (int i = 0; i < triangles.size(); i++) {
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
			particlePos[i] = particles[i]->integrateMotion(dt);

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

// An experimental update() function that uses clothCompute.cs (the cloth compute shader)
// However, only works for OpenGL version >= 4.2 
void Cloth::Update2() {
	computeProg.use();
	int size = nParticles.x * nParticles.y;

	for (int i = 0; i < size; ++i) {
		glDispatchCompute((unsigned int)this->nParticles.x / 30, 
			(unsigned int)this->nParticles.y / 30, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// swapping buffers:
		readBuf = 1 - readBuf;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computePosBuf[readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computePosBuf[1-readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, computeVeloBuf[readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, computeVeloBuf[1-readBuf]);
	}

	renderProg.use();
	glBindVertexArray(clothVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * posBufs[readBuf].size(), posBufs[readBuf].data(), GL_DYNAMIC_DRAW);

	// Unbind the VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cloth::togglePos(glm::vec3 moveAmt)
{
	glm::mat4 moveMat = glm::translate(glm::mat4(1.0f), moveAmt);
	model *= moveMat;
	for (int i = 0; i < nParticles.x; i++) {
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
	for (int i = 0; i < nParticles.x; i++) {
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

unsigned int Cloth::loadTexture(char const* path) {
	unsigned int textureID;
	unsigned char* image_data;
	int imgWidth, imgHt, num_color_channels;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image_data = stbi_load(path, &imgWidth, &imgHt, &num_color_channels, 0);
	if (image_data) {
		GLenum format;
		if (num_color_channels == 1)
			format = GL_RED;
		else if (num_color_channels == 3)
			format = GL_RGB;
		else if (num_color_channels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHt, 0, format, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(image_data);

	return textureID;
}
