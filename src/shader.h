#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glm/glm.hpp>

class Shader {
public:
	Shader(const char* cShaderPath);
	Shader(const char* vShaderPath, const char* fShaderPath, const char* gShaderPath);

	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setVec3(const std::string& name, glm::vec3 value);
	void setVec4(const std::string& name, glm::vec4 value);
	void setMat4(const std::string& name, glm::mat4 value);
	void use();
	int getID();

private:
	int ID;
};

#endif // SHADER_H
