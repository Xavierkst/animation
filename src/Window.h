#ifndef _WINDOW_H_
#define _WINDOW_H_

// we activate some section of the stb_image.h functions -- essentially turning it into a .cpp file 
#include "main.h"
#include "shader.h"
#include "Camera.h"
#include "rigging_src/Skin.h"
#include <glm/gtx/string_cast.hpp>
#include "rigging_src/AnimationPlayer.h"
#include "cloth_src/Cloth.h"
#include "FloorTile.h"
#include "particle_sys_src/ParticleSys.h"
#include <time.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
////////////////////////////////////////////////////////////////////////////////

class Window
{
public:
	// Window Properties
	static int width;
	static int height;
	static const char* windowTitle;

	// Objects to render
	static Cube* cube;
	static Skin* skin;
	static AnimationPlayer* player;
	static AnimationClip* clip;
	static Cloth* cloth;
	static FloorTile* the_floor;
	static ParticleSys* particle_sys;

	// Shader Program 
	static GLuint shaderProgram;
	// static GLuint clothShaderProgram;

	// Act as Constructors and desctructors 
	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();

	// for the Window
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);

	// update and draw functions
	static void idleCallback();
	static void displayCallback(GLFWwindow*);

	// helper to reset the camera
	static void resetCamera();

	// callbacks - for interaction
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_callback(GLFWwindow* window, double currX, double currY);

	static void imguiCallback(GLFWwindow* window); 
	static void imguiCleanUp();
	
	// Loading textures
	static unsigned int loadTexture(char const* path);
};

////////////////////////////////////////////////////////////////////////////////

#endif