#ifndef WINDOW_H
#define WINDOW_H

#include <memory>

#include "shader.h"
#include "Camera.h"
#include "rigging_src/Skin.h"
#include "rigging_src/AnimationPlayer.h"
#include "cloth_src/Cloth.h"
#include "particle_sys_src/ParticleSys.h"

class Window
{
public:
	// Window Properties
	static int width;
	static int height;
	static const char* windowTitle;

	// Objects to render
	static std::unique_ptr<Cube> cube; // static Cube* cube;
	static std::unique_ptr<Skin> skin; // static Skin* skin;
	static std::unique_ptr<AnimationPlayer> player;
	static std::unique_ptr<AnimationClip> clip;
	static std::unique_ptr<Cloth> cloth;
	static std::unique_ptr<FloorTile> the_floor;
	static std::unique_ptr<ParticleSys> particle_sys;

	// Shader Program 
	static GLuint shaderProgram;
	static GLuint clothShaderProgram;

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

	// for Camera
	static std::unique_ptr<Camera> Cam;
	static void resetCamera(); // helper to reset the camera

	// callbacks - for interaction
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_callback(GLFWwindow* window, double currX, double currY);

	static void imguiCallback(GLFWwindow* window); 
	static void imguiCleanUp();
	
};

#endif // WINDOW_H