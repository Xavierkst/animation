#include "shader.h"
#include <glad/glad.h>

Shader::Shader(const char* compShaderPath) {
	std::string cShaderCode;
	std::ifstream cShaderFile;
	cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		cShaderFile.open(compShaderPath);
		std::stringstream cShaderStream;
		cShaderStream << cShaderFile.rdbuf();
		cShaderFile.close();
		cShaderCode = cShaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cout << "ERROR::COMPUTE_SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	unsigned int compute;
	const char* cShaderCode1 = cShaderCode.c_str();
	int success;
	char infoLog[512];
	compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &cShaderCode1, NULL);
	glCompileShader(compute);
	glGetShaderiv(compute, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(compute, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	this->ID = glCreateProgram();
	glAttachShader(this->ID, compute);
	glLinkProgram(this->ID);
	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
		std::cout << "ERROR::COMPUTE_SHADER::PROGRAM:: LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(compute);
}

Shader::Shader(const char * vShaderPath, const char* fShaderPath, const char* gShaderPath) {
	std::string vShaderCode, fShaderCode, gShaderCode;
	std::ifstream vShaderFile, fShaderFile, gShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vShaderFile.open(vShaderPath);
		fShaderFile.open(fShaderPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vShaderCode = vShaderStream.str();
		fShaderCode = fShaderStream.str();

		if (gShaderPath != nullptr) {
			gShaderFile.open(gShaderPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			gShaderCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode1 = vShaderCode.c_str();
	const char* fShaderCode1 = fShaderCode.c_str();

	// compile the shader files 
	unsigned int vertex, fragment, geom;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode1, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode1, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	if (gShaderPath != nullptr) {
		geom = glCreateShader(GL_GEOMETRY_SHADER);
		const char* gShaderCode1 = gShaderCode.c_str();
		glShaderSource(geom, 1, &gShaderCode1, NULL);
		glCompileShader(geom);
		glGetShaderiv(geom, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(geom, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}
	
	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertex);
	glAttachShader(this->ID, fragment);

	if (gShaderPath != nullptr) {
		glAttachShader(this->ID, geom);
	}

	glLinkProgram(this->ID);

	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM:: LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDetachShader(this->ID, vertex);
	glDetachShader(this->ID, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	if (gShaderPath != nullptr) {
		glDetachShader(this->ID, geom);
		glDeleteShader(geom);
	}
}

void Shader::setBool(const std::string& name, bool value){
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, glm::vec3 value) {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string& name, glm::vec4 value) {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const std::string& name, glm::mat4 value) {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::use() {
	glUseProgram(ID);
}

int Shader::getID() {
	return ID;
}
