#include "shaderClass.h"

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}


//Shader constructor
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{	
	//Receive string from the tex files
	std::string vertexCode = get_file_contents(vertexFile);		
	std::string fragmentCode = get_file_contents(fragmentFile);

	//convert and store as character array
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);	//Create vertex shader
	glShaderSource(vertexShader, 1, &vertexSource, NULL);	//Points to source code
	glCompileShader(vertexShader);	//GPU can't read source code threfore, pre-compile

	//Check vertex shader error checking
	compileErrors(vertexShader, "VERTEX");

	//Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);	//Replace vertex with fragment
	glCompileShader(fragmentShader);

	//Check fragment shader error checking
	compileErrors(fragmentShader, "FRAGMENT");

	//Wrap to shader program
	ID = glCreateProgram();	//Only one type of shader program
	glAttachShader(ID, vertexShader);	//attach vertex shader to shader program
	glAttachShader(ID, fragmentShader);	//attach fragment shader to shader program
	//Finally wrap the entire shaderProgram with shaders attached
	glLinkProgram(ID);

	//Check shader linking error checking
	compileErrors(ID, "PROGRAM");

	//Since attached and wrapped, the shader can be deleted
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Activate()
{
	glUseProgram(ID);			//Activate shaderProgram
}

void Shader::Delete()
{
	glDeleteProgram(ID);			//Delete shaderProgram
}

void Shader::compileErrors(unsigned int shader, const char* type)
{
	GLint hasCompiled;
	char infoLog[1024];

	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);	// compile status >> hasCompiled
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER COMPULATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}