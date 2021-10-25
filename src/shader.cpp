#include "shader.h"

GLuint Shader::LoadSingleShader(const char * shaderFilePath, ShaderType type) 
{
	// Create a shader id.
	GLuint shaderID = 0;
	std::string shader_type;

	if (type == vertex) {
		shaderID = glCreateShader(GL_VERTEX_SHADER);
		shader_type = "vertex";
	}
	else if (type == fragment) {
		shaderID = glCreateShader(GL_FRAGMENT_SHADER);
		shader_type = "fragment";
	}
	else if (type == compute) {
		shaderID = glCreateShader(GL_COMPUTE_SHADER);
		shader_type = "compute";
	}

	// Try to read shader codes from the shader file.
	std::string shaderCode;
	std::ifstream shaderStream(shaderFilePath, std::ios::in);
	if (shaderStream.is_open()) 
	{
		std::string Line = "";
		while (getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}
	else 
	{
		std::cerr << "Impossible to open " << shaderFilePath << ". "
			<< "Check to make sure the file exists and you passed in the "
			<< "right filepath!"
			<< std::endl;
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Shader.
	std::cerr << "Compiling shader: " << shaderFilePath << std::endl;
	char const * sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);
	checkCompileErrors(shaderID, shader_type);

	// Check Shader.
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		std::vector<char> shaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, shaderErrorMessage.data());
		std::string msg(shaderErrorMessage.begin(), shaderErrorMessage.end());
		std::cerr << msg << std::endl;
		return 0;
	}
	else 
	{
		if (type == vertex)
			printf("Successfully compiled vertex shader!\n");
		else if (type == fragment) 
			printf("Successfully compiled fragment shader!\n");
		else if (type == compute) 
			printf("Successfully compiled compute shader!\n");
	}

	return shaderID;
}

bool Shader::LinkShader(GLuint shaderID)
{
	if (shaderID == 0) {
		printf("Shader linking failed!");
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Link the program.
	printf("Linking program\n");
	glAttachShader(this->ID, shaderID);
	glLinkProgram(this->ID);

	// Check the program.
	glGetProgramiv(this->ID, GL_LINK_STATUS, &Result);
	glGetProgramiv(this->ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(this->ID, InfoLogLength, NULL, ProgramErrorMessage.data());
		std::string msg(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
		std::cerr << msg << std::endl;
		glDeleteProgram(this->ID);
		return 0;
	}
	else
	{
		printf("Successfully linked program!\n");
	}
	
	glDetachShader(this->ID, shaderID);

	return 1;
}

GLuint Shader::LoadShaders(const char * vertexFilePath, const char * fragmentFilePath, const char* computeFilePath) 
{
	// Create the vertex shader and fragment shader.
	GLuint vertexShaderID = LoadSingleShader(vertexFilePath, vertex);
	GLuint fragmentShaderID = LoadSingleShader(fragmentFilePath, fragment);
	GLuint computeShaderID;
	if (computeFilePath) {
		computeShaderID = LoadSingleShader(computeFilePath, compute);
	}

	// Check both shaders.
	if (vertexShaderID == 0 || fragmentShaderID == 0) return 0;

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Link the program.
	printf("Linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	if (computeFilePath)
		glAttachShader(programID, computeShaderID);
	glLinkProgram(programID);

	// Check the program.
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, ProgramErrorMessage.data());
		std::string msg(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
		std::cerr << msg << std::endl;
		glDeleteProgram(programID);
		return 0;
	}
	else
	{
		printf("Successfully linked program!\n");
	}

	// Detach and delete the shaders as they are no longer needed.
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	this->ID = programID;

	return programID;
}

void Shader::setBool(const std::string& name, bool value)
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, glm::vec3 value)
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string& name, glm::vec4 value)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const std::string& name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::checkCompileErrors(unsigned int ID, std::string type)
{
	GLint success;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(ID, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER::ERROR of type: " << type << "\n" << infoLog << std::endl;
		}
	} 
	else {
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM::LINKING::ERROR of type: " << type << "\n" << infoLog << std::endl;
		}
	}
}
