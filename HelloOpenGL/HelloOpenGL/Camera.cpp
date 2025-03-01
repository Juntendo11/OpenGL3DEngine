#include"Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
	//Camera constructor method
{
	Camera::width = width;
	Camera::height = height;
	Position = position;

}


void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);	//Vector eye, vector center, vector up
	projection = glm::perspective(glm::radians(FOVdeg), (float)(width / height), nearPlane, farPlane);

	//export the matrix to the vertex shader
	cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	//export the matrix to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));

}

void Camera::Inputs(GLFWwindow* window) {
	//WASD input FRONT/BACK/Sides
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += speed * Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));	//Cross product of orientation and up vector
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += speed * -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}


	//SPACE CTRL UP/DOWN
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * -Up;
	}

	//SHIFT
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)	//Increase speed
	{
		speed = 0.4f;
	}

	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)	//Decrease speed
	{
		speed = 0.1f;
	}
	

	//Mouse controls
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)		//If left mouse button pressed
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);		//Hide mouse

		if (firstClick)
			//Reset the mouse position to center
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));		//Move cursor to center of screen
			firstClick = false;

		}

		//Mouse coordinate
		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);	//Obtain mouse coordinate in a window

		//Normalize and shift the cursor coordinate to begin in the middle of screen.
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		//Calculate vertical change in orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		//Avoid camera barrel rolling like crazy
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
			//If camera angle greater than 85 deg, stop rotating up further u dumb fuck
		{
			Orientation = newOrientation;
		}

		//Rotate orientation left/Right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		//Set cursor to middle of the screen (Prevent stupid movement of curosr)
		glfwSetCursorPos(window, (width / 2), (height / 2));		//Move cursor to center of screen
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);		//Hide mouse
		firstClick = true;	//Release debounce
	}
}