#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

//Include needed for opengl and text reader functions

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

//declare function read shader text vals
std::string get_file_contents(const char* filename);	//Outputs content of text file as string


//Declare shader class (Opengl shader program)
class Shader
{
	public:
		//ID
		GLuint ID;	

		//Constructor
		Shader(const char* vertexFile, const char* fragmentFile);

		//
		void Activate();
		void Delete();
	private:
		//Check if compiled
		void compileErrors(unsigned int shader, const char* type);
};
#endif