#ifndef _SHADER_H_
#define _SHADER_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum ShaderType { vertex, fragment, compute };

class Shader {
private:
	void checkCompileErrors(unsigned int ID, std::string type);
public:
	
	GLuint ID; // Shader ID

	GLuint Shader::LoadSingleShader(const char* shaderFilePath, ShaderType type);
	bool Shader::LinkShader(GLuint shaderID);
	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path, const char* computeFilePath = NULL);

	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setVec3(const std::string& name, glm::vec3 value);
	void setVec4(const std::string& name, glm::vec4 value);
	void setMat4(const std::string& name, glm::mat4 value);
	void use();
};
#endif
