// we activate some section of the stb_image.h functions -- 
// essentially turning it into a .cpp file 
#define STB_IMAGE_IMPLEMENTATION
#include "../../stb_image/stb_image.h"
#include "Cloth.h"

// 6 sides, 36 vertices, position, normal and tex coord data 
float cubeVertices[] = {
	// positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

// Constructor for simulation using CPU computed vertex attributes 
Cloth::Cloth(const char* computeShaderPath, const char* diffuseTexPath, const char* specularTexPath): model(glm::mat4(1.0f)), transfMat(glm::mat4(1.0f)) 
{
	glm::vec2 clothSize(6.0f, 8.0f);
	nParticles = glm::ivec2(40, 40);
	vAir = glm::vec3(0.001f, .001f, 0.001f); // air velocity

	// Constants: 
	// 1. viable values: Ks = 100.0f, Kd = 50.0f, mass = 1.1f, Cd = 1.020f
	// 2. Another viable set of values (?) Ks = 50.0f, Kd = 30.0f, mass = 1.1f, Cd = 1.020f
	springConst = 200.0f; 
	dampConst = 15.0f; 
	Cd = 1.020f;
	rho = 1.225;
	rest_const = 0.05f;
	dynamic_fric = 0.60f;
	float pMass = 0.8f;

	float dx = clothSize.x / (float)(nParticles.x - 1.0f);
	float dy = clothSize.y / (float)(nParticles.y - 1.0f);
	float ds = 1.0f / (float)(nParticles.x - 1);
	float dt = 1.0f / (float)(nParticles.y - 1);

	// Given a gridSize, we want to set the positions of the particles here 
	for (int i = nParticles.y - 1; i >= 0; i--) {
		for (int j = 0; j < nParticles.x; j++) {
			// force and velo default to 0
			// randomized z-component starter values
			float randVal = (rand() % 100 + 1) / 1000.0f; 
			glm::vec3 position = glm::vec3((j - (nParticles.x - 1) / 2.0f) * dx, i * dy, randVal);
			glm::vec3 norm = glm::vec3(.0f, .0f, 1.0f);
			particles.push_back(new Particle(glm::vec3(.0f), 
				glm::vec3(.0f), position, norm, pMass));

			Vertex v;
			v.pos = position;
			v.normal = norm;
			v.texCoord = glm::vec2(ds * (float)j, dt * (float)i);
			vertices.push_back(v);
		}
	}

	// Make sure force doesn't apply on the top row of particles
	for (int j = 0; j < nParticles.x; j++) {
		particles[j]->setForceApply(false);
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

	// Load vert and frag shaders only (constructor below also loads compute shader)
	renderProg.LoadShaders("src/shaders/clothShader.vert", "src/shaders/clothShader.frag");	
	lightRenderProg.LoadShaders("src/shaders/clothShader.vert", "src/shaders/lightSource.frag");

	// initialize the grid using openGL
	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &clothVAO);
	glGenBuffers(1, &clothVBO);
	glGenBuffers(1, &clothEBO);
	glGenTextures(1, &clothTexture[0]);

	// Bind the VAO.
	glBindVertexArray(clothVAO);

	glBindBuffer(GL_ARRAY_BUFFER, clothVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
	// vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
	glEnableVertexAttribArray(0);
	// vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// tex coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, clothEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triIndices.size(), triIndices.data(), GL_STATIC_DRAW);

	// Bind the texture coord buffer object
	clothTexture[0] = loadTexture(diffuseTexPath);
	clothTexture[1] = loadTexture(specularTexPath);
	// clothTexture[0] = loadTexture("src/textures/bladeRunner.jpg");
	// clothTexture[0] = loadTexture("src/textures/awesomeface.png");
	// clothTexture[0] = loadTexture("src/textures/leatherDiamond/Leather_Diamond_Patches_002_basecolor.jpg");
	// clothTexture[1] = loadTexture("src/textures/leatherDiamond/Leather_Diamond_Patches_002_metallic.jpg");

	renderProg.use();
	renderProg.setInt("material.texture_diffuse1", 0);
	renderProg.setInt("material.texture_specular1", 1);
	renderProg.setFloat("material.shininess", 64);

	// pt light
	// set 2 pt light uniform values
	for (int i = 0; i < 2; ++i) {
		std::string num = std::to_string(i+1);
		renderProg.setFloat("pt_light" + num + ".k_constant", 1.0f);
		renderProg.setFloat("pt_light" + num + ".k_linear", 0.09f);
		renderProg.setFloat("pt_light" + num + ".k_quad", 0.032f);
		renderProg.setVec3("pt_light" + num + ".ambient", glm::vec3(0.2f, .2f, 0.2f));
		renderProg.setVec3("pt_light" + num + ".diffuse", glm::vec3(0.8f, .8f, 0.8f));
		renderProg.setVec3("pt_light" + num + ".specular", glm::vec3(1.0f));
	}
	// dir light
	renderProg.setVec3("dir_light.direction", glm::vec3(-2.0f, -8.0f, -4.0));
	renderProg.setVec3("dir_light.ambient", glm::vec3(0.2f, .2f, 0.2f));
	renderProg.setVec3("dir_light.diffuse", glm::vec3(0.5f, .5f, 0.5f));
	renderProg.setVec3("dir_light.specular", glm::vec3(1.0f));

	// Generate cube light source buffers
	glGenBuffers(1, &lightSourcePos);
	glGenVertexArrays(1, &lightSourceVAO);
	glBindVertexArray(lightSourceVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightSourcePos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices[0], GL_STATIC_DRAW);
	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
	// Tex coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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

	// delete[] vertices;

	// glDeleteProgram(renderProg.ID);
}
// Render the cloth each frame 
void Cloth::Draw(const glm::vec3& camPos, const glm::mat4& viewProjMtx, GLFWwindow* window)
{
	float t_time = glfwGetTime();
	float t1 = fabsf(sin( t_time*2.0f));
	float t2 = fabsf(cos(t_time *2.0f));
	float t3 = fabsf(sin(t_time * 2.0f));

	// activate the shader program 
	renderProg.use();
	// renderProg.setFloat("material.texture_diffuse1", 1);
	renderProg.setFloat("material.shininess", 64);
	// renderProg.setFloat("material.texture_diffse1", 0);
	// pt light uniform values
	glm::mat4 lightModel1(1.0f);
	glm::vec4 lightPos1(glm::vec3(0.0f), 1.0f);
	glm::vec4 lightPos2(glm::vec3(0.0f), 1.0f);
	float dist = 5.0f;
	float rotSpeed = t_time * 50.0f;
	lightModel1 = glm::rotate(lightModel1, glm::radians(-rotSpeed), glm::vec3(.0f, 1.0f, .0f));
	lightModel1 = glm::translate(lightModel1, glm::vec3(dist));
	lightPos1 = lightModel1 * lightPos1;
	glm::vec4 light_col(t1, t2, t3, 1.0f);
	glm::vec3 LightCol(t1, t2, t3);
	renderProg.setVec3("LightColor", LightCol);
	renderProg.setVec4("pt_light1.position", lightPos1);
	renderProg.setVec3("pt_light1.ambient", 0.2f * glm::vec3(t1, t2, t3));
	renderProg.setVec3("pt_light1.diffuse", 0.5f * glm::vec3(t1, t2, t3));
	renderProg.setVec3("pt_light1.specular", glm::vec3(t1, t2, t3));
	glm::vec3 white_light(1.0f);
	lightModel1 = glm::mat4(1.0f);
	lightModel1 = glm::rotate(lightModel1, glm::radians(rotSpeed), glm::vec3(.0f, 1.0f, 0.0f));
	lightModel1 = glm::translate(lightModel1, glm::vec3(-dist, 2.0f, -dist));
	lightPos2 = lightModel1 * lightPos2;
	renderProg.setVec4("pt_light2.position", lightPos2);
	renderProg.setVec3("pt_light2.ambient", 0.2f * white_light);
	renderProg.setVec3("pt_light2.diffuse", 0.5f * white_light);
	renderProg.setVec3("pt_light2.specular", white_light);
	// get the locations and send the uniforms to the shader 
	renderProg.setMat4("model", this->model);
	renderProg.setMat4("viewProj", viewProjMtx);
	renderProg.setVec3("view_position", camPos);

	// Set the active texture unit 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, clothTexture[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, clothTexture[1]);
	// Bind the clothVAO
	glBindVertexArray(clothVAO);
	glBindBuffer(GL_ARRAY_BUFFER, clothVBO);
	glDrawElements(GL_TRIANGLES, triIndices.size(), GL_UNSIGNED_INT, 0);

	// Bind and draw light cubes
	glBindVertexArray(lightSourceVAO);
	lightRenderProg.use();
	lightModel1 = glm::mat4(1.0f);
	lightModel1 = glm::rotate(lightModel1, glm::radians(-rotSpeed), glm::vec3(.0f, 1.0f, 0.0f));
	lightModel1 = glm::translate(lightModel1, glm::vec3(dist));
	lightRenderProg.setMat4("model", lightModel1);
	lightRenderProg.setMat4("viewProj", viewProjMtx);
	lightRenderProg.setVec3("view_position", camPos);
	lightRenderProg.setVec3("lightColor", glm::vec3(t1, t2, t3));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	lightModel1 = glm::mat4(1.0f);
	lightModel1 = glm::rotate(lightModel1, glm::radians(rotSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
	lightModel1 = glm::translate(lightModel1, glm::vec3(-dist, 2.0f, -dist));
	lightRenderProg.setMat4("model", lightModel1);
	lightRenderProg.setVec3("lightColor", white_light);
	glDrawArrays(GL_TRIANGLES, 0, 36);

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
			triangles[i].getP1()->applyForce(aeroF);
			triangles[i].getP2()->applyForce(aeroF);
			triangles[i].getP3()->applyForce(aeroF);
		}

		// iterate thru particles and normalize normals and finally:
		// integrate the motion in this pass
		for (int i = 0; i < particles.size(); i++) {
			particles[i]->setNorm(glm::normalize(particles[i]->getNorm()));
			vertices[i].normal = particles[i]->getNorm();
			// integrate motion
			vertices[i].pos = particles[i]->integrateMotion(dt);

			// After integrating motion, check if collided with objects 
			// in this case, FloorTile
			if (particles[i]->getPos().y < floor->getYPos()) {
				// shift the y pos of the cloth vertex up to
				// the surface

				// calculate impulse, tangential impulse. Add to particle's velocity:
				// closing velo
				float v_close = glm::dot((particles[i]->getVelo() - floor->getVelo()), floor->getNormal());
				// impulse
				glm::vec3 impulse(-1.0f * (1 + rest_const) * particles[i]->getMass() * 
					v_close * floor->getNormal());

				// compute frictional impulse -- parallel to plane
				// find tangential velo and flip value
				glm::vec3 v_norm(glm::dot(particles[i]->getVelo(), floor->getNormal()) * floor->getNormal());
				glm::vec3 tan_impulse_dir(-1.0f * 
				glm::normalize(particles[i]->getVelo() - v_norm));
				glm::vec3 frictional_impulse(dynamic_fric * glm::length(impulse) * tan_impulse_dir);

				// add these to the particle's velo: 
				particles[i]->setVelo(particles[i]->getVelo() + 0.2f * ((impulse + frictional_impulse) / particles[i]->getMass()));

				particles[i]->setPos(glm::vec3(particles[i]->getPos().x, 
					particles[i]->getPos().y + (floor->getYPos() - vertices[i].pos.y) + 0.005f, 
					particles[i]->getPos().z));

				vertices[i].pos = particles[i]->getPos();
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, clothVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex) , vertices.data(), GL_DYNAMIC_DRAW);
	
	// Unbind VBO
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
	glBindBuffer(GL_ARRAY_BUFFER, clothVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * posBufs[readBuf].size(), posBufs[readBuf].data(), GL_DYNAMIC_DRAW);

	// Unbind the VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cloth::togglePos(glm::vec3 moveAmt)
{
	glm::mat4 moveMat = glm::translate(glm::mat4(1.0f), moveAmt);
	transfMat = moveMat * transfMat;
	for (int i = 0; i < nParticles.x; i++) {
		// glm::vec3 newPos = moveMat * glm::vec4(particles[i]->getPos(), 1.0f);
		glm::vec3 newPos = moveMat * glm::vec4(particles[i]->getPos(), 1.0f);
		particles[i]->setPos(newPos);
	}
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
		// glm::mat4 newTransl(glm::inverse(model));
		glm::mat4 invTransf(glm::inverse(transfMat));
		glm::vec3 nPos = transfMat * rot * invTransf * glm::vec4(particles[i]->getPos(), 1.0f);
		particles[i]->setPos(nPos);
		// particlePos[i] = nPos;
	}
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

	stbi_set_flip_vertically_on_load(true);
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
		std::cout << "Successfully loaded texture" << std::endl;
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(image_data);

	return textureID;
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
	// set 2 pt light uniform values
	for (int i = 0; i < 2; ++i) {
		std::string num = std::to_string(i+1);
		renderProg.setFloat("pt_light" + num + ".k_constant", 1.0f);
		renderProg.setFloat("pt_light" + num + ".k_linear", 1.0f);
		renderProg.setFloat("pt_light" + num + ".k_quad", 1.0f);
	}
	// dir light
	renderProg.setVec3("dir_light.ambient", glm::vec3(0.2f, .2f, 0.2f));
	renderProg.setVec3("dir_light.diffuse", glm::vec3(1.0f));
	renderProg.setVec3("dir_light.specular", glm::vec3(1.0f));
	// Material
	renderProg.setVec3("material.ambient", glm::vec3(1.0f));
	renderProg.setVec3("material.diffuse", glm::vec3(1.0f));
	renderProg.setVec3("material.specular", glm::vec3(1.0f));
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

