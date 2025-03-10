//GLSL Specify versuib
#version 330 core


//Layout helps openGL to read vertex data
layout (location = 0) in vec3 aPos;		//Takes in apos (vec3) position data (vertex)

//Create new layout for the vertex color
layout (location = 1) in vec3 aNormal;

//New layout for the texture color
layout (location = 2) in vec3 aColor;	//Vertex color

//New layout for the normals
layout (location = 3) in vec2 aTex;		//Vertex UV




//Output color from the vertex shader to fragment shader
out vec3 crntPos;	//Current position
out vec3 Normal;	//Vertex Normal
out vec3 color;		//Vertex Color
out vec2 texCoord;	//Texture coordinate out




//Receive camera matrix data
uniform mat4 camMatrix;

//Obtian model normal data
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;



void main()
{
	//Current position of model
	crntPos = vec3(model * translation * -rotation * scale * vec4(aPos, 1.0f));
	Normal = aNormal;
	color  = aColor;
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;

	gl_Position = camMatrix * vec4(crntPos, 1.0f);
}