//VertexArray

#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"	//Needs to link VBO for VAO

class VAO
{
public:
	GLuint ID;
	VAO();		
	//link VAO
	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	void Bind();
	void Unbind();
	void Delete();

};
#endif