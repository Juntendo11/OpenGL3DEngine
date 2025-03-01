//#include<filesystem>
//namespace fs = std::filesystem;

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Model.h"

//Screen window size
const unsigned int width = 800;
const unsigned int height = 800;


int main() {
	glfwInit();	//Initialise GLFW

	//What major version of GLFW to use
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//What profile to use -> Core profile -> modern packages
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//glfwCreateWindow(width, hight, windowName, Fullscreen, NULL );
	GLFWwindow* window = glfwCreateWindow(width, height, "HelloOpenGL", NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//Start window
	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0,0,width,height);	//glViewport(bottomLeft, top right)

	//Generate shader object from default vert and frag
	Shader shaderProgram("default.vert", "default.frag");

	Shader outliningProgram("outlining.vert", "outlining.frag");


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);	//White light
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);		//Export to fragment shader
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	//Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	//Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);	//Keep front face
	glFrontFace(GL_CW);	//Front face => CCW indexed

	//Depth draw order
	glDepthFunc(GL_LESS);	//If depth less, then dont draw behind

	glEnable(GL_STENCIL_TEST);	//enable the stencil test
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);		//if both depth and stencil tests passes, replace the pixel

	//Camera
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));	//Constuct camera	(width, height, position (vec3))

	//Load in model
	std::string Path = "Resources//Models//";
	Model model((Path + "scene.gltf").c_str());

	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	//Vsync off ie stop FPS limit to 60FPS
	glfwSwapInterval(0);

	//Main loop
	while (!glfwWindowShouldClose(window)) 
	{	
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;
		if (timeDiff >=  1.0 / 30.0)
		{
			std::string FPS = std::to_string((1.0f / timeDiff) * counter);
			std:: string ms = std::to_string((timeDiff / counter * 1000));
			std::string newTitle = "HelloOpenGL: " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			prevTime = crntTime;
			counter = 0;

			//camera input here for no fps limit
			camera.Inputs(window);
		}
		// Specify the color of the background
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f); //R,G,B,Alpha
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	//Also clear GL depth buffer bit

		//Camera controls
		//camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);



		//Draw model
		//Specify stencil test always passes set reference  value to 1
		glStencilFunc(GL_ALWAYS, 1, 0xFF);	//Func, ref mask
		glStencilMask(0xFF);				//Fill all the stencil mask to 1

		model.Draw(shaderProgram, camera);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);	//Want outlint to not pass when its not equal to one
		glStencilMask(0x00);					//0 ie outside fill
		glDisable(GL_DEPTH_TEST);				//Finish test

		//Send to outline shader
		//outliningProgram.Activate();
		//glUniform1f(glGetUniformLocation(outliningProgram.ID, "outlining"), 0.0008f);
		model.Draw(outliningProgram, camera);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);


		glfwSwapBuffers(window);	//update image
		glfwPollEvents();			//Wait for action in the window
	}

	shaderProgram.Delete();
	//Clean up
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}