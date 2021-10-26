////////////////////////////////////////
// Window.cpp
////////////////////////////////////////

#include "Window.h"

////////////////////////////////////////////////////////////////////////////////

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Viewport";

// Objects to render
Cube * Window::cube;
//Skeleton* Window::skeleton;
Skin* Window::skin;
AnimationClip* Window::clip;
AnimationPlayer* Window::player;
Cloth* Window::cloth;
const char* computeShaderPath;

float time1, time2;

// Gravitational force: 
glm::vec3 gravForce; 
float clothMass;
int grid_size;

FloorTile* Window::the_floor;
int fSize; // floorSize

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;
GLuint Window::clothShaderProgram;
Shader windowShaderProg;
Shader clothShader;

// ImGui Parameters 
bool show_demo_window;
bool show_another_window;
ImVec4 clear_color;

// Particle System 
ParticleSys* Window::particle_sys;

////////////////////////////////////////////////////////////////////////////////

// Constructors and desctructors 
bool Window::initializeProgram() {

	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = windowShaderProg.LoadShaders("src/shaders/shader.vert", "src/shaders/shader.frag");
	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader programs" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	// Create a skin and its skeleton, pass in files for parsing
	skin = new Skin("src/animations/wasp/wasp.skin", "src/animations/wasp/wasp.skel");
	// clip = new AnimationClip();
	// clip->Load("src/animations/wasp/wasp_walk.anim");
	// player = new AnimationPlayer(clip, skin->getSkeleton());

	// Time initializer (to calculate dt in idleCallBack())
	time1 = clock();

	// Cloth
	clothMass = 0.8f;
	grid_size = 30;
	cloth = new Cloth();

	// Floor tile
	fSize = 30;
	the_floor = new FloorTile(fSize);

	// Gravity force:
	gravForce = glm::vec3(0.0f, -9.8f, 0.0f);

	show_demo_window = true;
	show_another_window = false;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Particle system
	particle_sys = new ParticleSys(10000);

	return true;
}

void Window::cleanUp()
{
	Window::imguiCleanUp();
	delete skin; // skin has skeleton destructor
	delete particle_sys;
	delete cloth;
	
	// Delete the shader program.
	glDeleteProgram(shaderProgram);
}

////////////////////////////////////////////////////////////////////////////////

// for the Window
GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// set up the camera
	Cam = new Camera();
	Cam->SetAspect(float(width) / float(height));

	// initialize the interaction variables
	LeftDown = RightDown = false;
	MouseX = MouseY = 0;

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	Cam->SetAspect(float(width) / float(height));
}

////////////////////////////////////////////////////////////////////////////////

// update and draw functions
void Window::idleCallback()
{
	// Perform any updates as necessary. 
	Cam->Update();

	// Calculate time step
	float new_t = clock();
	float dt = (new_t - time1);
	time1 = new_t;

	// Update Cloth vertices each frame
	int tot_steps = 60;
	cloth->Update((dt/ (CLOCKS_PER_SEC * tot_steps)), gravForce, the_floor, tot_steps); 
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClearColor(.0f, .0f, .0f, 1.0f); // background color 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Render skin
	// skin->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram, window);
	// skin->getSkeleton()->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

	// the_floor->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

	cloth->Draw(Cam->getCamPos(),Cam->GetViewProjectMtx(), window);

	//particle_sys->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram, window);

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();

	// Swap buffers.
	glfwSwapBuffers(window);
}

////////////////////////////////////////////////////////////////////////////////

// helper to reset the camera
void Window::resetCamera() 
{
	Cam->Reset();
	Cam->SetAspect(float(Window::width) / float(Window::height));
}

////////////////////////////////////////////////////////////////////////////////

// callbacks - for Interaction 
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_W: 
			cloth->togglePos(glm::vec3(.0f, .0f, -2.0f));
			break;
		case GLFW_KEY_A: 
			cloth->togglePos(glm::vec3(-2.0f, .0f, .0f));
			break;
		case GLFW_KEY_S: 
			cloth->togglePos(glm::vec3(.0f, .0f, 2.0f));
			break;
		case GLFW_KEY_D: 
			cloth->togglePos(glm::vec3(2.0f, .0f, .0f));
			break;
		case GLFW_KEY_Z: 
			cloth->togglePos(glm::vec3(.0f, -1.0f, .0f));
			break;
		case GLFW_KEY_X: 
			cloth->togglePos(glm::vec3(.0f, 1.0f, .0f));
			break;
		case GLFW_KEY_F: {
			cloth->toggleWind();
			break;
		}
		case GLFW_KEY_E: 
			cloth->windSpeed(-3.0f);
			break;		
		case GLFW_KEY_Q: 
			cloth->windSpeed(3.0f);
			break;
		case GLFW_KEY_C: 
			cloth->spin(30.0f);
			break;
		case GLFW_KEY_V: 
			cloth->spin(-30.0f);
			break;
		case GLFW_KEY_ESCAPE:
			// Close the window. Causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_R:
			resetCamera();
			break;
		default:
			break;
		}

	}
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		LeftDown = (action == GLFW_PRESS); }
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		RightDown = (action == GLFW_PRESS);
	}
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {

	int maxDelta = 100;
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

	MouseX = (int)currX;
	MouseY = (int)currY;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if (LeftDown) {
		const float rate = 1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
	}
	if (RightDown) {
		const float rate = 0.005f;
		float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		Cam->SetDistance(dist);
	}
}

void Window::imguiCleanUp() {
	// Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
////////////////////////////////////////////////////////////////////////////////