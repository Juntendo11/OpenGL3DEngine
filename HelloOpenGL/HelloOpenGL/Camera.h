#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

//Include needed for opengl and text reader functions

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "shaderClass.h"

class Camera
{
public:
	glm::vec3 Position;										//Camera position
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);	//Camera orientation
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);				//Up vector

	glm::mat4 cameraMatrix = glm::mat4(1.0f);	//Camera matrix

	bool firstClick = true;			//Mouse click first (debounce sort of...)

	int width;
	int height;

	float speed = 0.1f;			//Speed of camera
	float sensitivity = 100.0f;	//Sensitivity of camera

	Camera(int width, int height, glm::vec3 position);	//Camera constructor

	void updateMatrix(float FOVdeg, float nearPlane, float farPlane);

	void Matrix(Shader& shader, const char* uniform);	
	void Inputs(GLFWwindow* window);
};

#endif