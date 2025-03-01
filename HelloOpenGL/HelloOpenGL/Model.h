#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include<json/json.h>
#include"Mesh.h"

using json = nlohmann::json;	//Calling json library

class Model
{
public:
	Model(const char* file);	//File directory
	void Draw(Shader& shader, Camera& camera);

private:
	const char* file;
	std::vector<unsigned char> data;
	json JSON;


	//Mesh data variables
	std::vector<Mesh> meshes;	//Vector to store all loaded meshes
	std::vector<glm::vec3> translationsMeshes;
	std::vector<glm::quat> rotationsMeshes;
	std::vector<glm::vec3> scalesMeshes;
	std::vector<glm::mat4> matricesMeshes;		//All of translation, rotation, scale combined (Use either)

	//Variables to keep track of if the texture has been loaded
	std::vector<std::string> loadedTexName;
	std::vector<Texture> loadedTex;



	//meshes vector to store all of loaded meshes
	void loadMesh(unsigned int indMesh);

	void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

	std::vector<unsigned char> getData();	//Obtain vector from binary data file

	//Leaves (bottom of hierachy) the Accessor node
	std::vector<float> getFloats(json accessor);
	std::vector<GLuint> getIndices(json accessor);


	//Automatic texture assignment
	std::vector<Texture> getTextures();


	//Functions to combine vertex and its attributes
	std::vector<Vertex> assembleVertices
	(
		std::vector<glm::vec3> positions,
		std::vector<glm::vec3> normals,
		std::vector<glm::vec2> texUVs
	);

	//Output from get function and store into group glm vectors
	std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
	std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
	std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);
};
#endif