#include"Model.h"

Model::Model(const char* file)
{
	std::string text = get_file_contents(file);
	JSON = json::parse(text);	//Abstract the json structure

	Model::file = file;
	data = getData();

	traverseNode(0);
}

void Model::Draw(Shader& shader, Camera& camera)
{
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Mesh::Draw(shader, camera, matricesMeshes[i]);
	}
}

void Model::loadMesh(unsigned int indMesh)
{
	//Obtain index values from keyword file in json
	unsigned int posAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normalAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int texAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indAccInd = JSON["meshes"][indMesh]["primitives"][0]["indices"];

	//Store mesh values as vectors from the json keyword values
	std::vector<float> posVec = getFloats(JSON["accessors"][posAccInd]);
	std::vector<glm::vec3> positions = groupFloatsVec3(posVec);
	std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
	std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);
	std::vector<float> texVec = getFloats(JSON["accessors"][texAccInd]);
	std::vector<glm::vec2> texUVs = groupFloatsVec2(texVec);

	//After successfully obtaining the values, combine the attributes to the values
	std::vector<Vertex> vertices = assembleVertices(positions, normals, texUVs);
	std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);
	std::vector<Texture> textures = getTextures();

	meshes.push_back(Mesh(vertices, indices, textures));	//Append the mesh part into vector
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix)
{
	json node = JSON["nodes"][nextNode];	//obtain node info from json file
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);	//Initialise translation vector

	//Translation
	if (node.find("translation") != node.end())	//If the node is for translation
	{
		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
			transValues[i] = (node["translation"][i]);
		translation = glm::make_vec3(transValues);
	}

	//Rotation
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);	//Initialise idetinty quat
	if (node.find("rotation") != node.end())	//If the node is for translation
	{
		float rotValues[4] =
		{
			//reorganise quaternion (x,y,z,w) -> (w,x,y,z)
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = glm::make_quat(rotValues);
	}

	//Scale
	glm::vec3 scale = glm::vec3(0.1f, 0.1f, 0.1f);
	if (node.find("scale") != node.end())
	{
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
			scaleValues[i] = (node["scale"][i]);
		scale = glm::make_vec3(scaleValues);
	}
	
	//Combined matrix
	glm::mat4 matNode = glm::mat4(1.0f);
	if (node.find("matrix") != node.end())
	{
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
			matValues[i] = (node["matrix"][i]);
		matNode = glm::make_mat2(matValues);
	}
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;		//Combine all the transformation as one matrix
	
	//Check what the next input in the json node is
	//If mesh -> apply all transformation
	if (node.find("mesh") != node.end())
	{
		translationsMeshes.push_back(translation);
		rotationsMeshes.push_back(rotation);
		scalesMeshes.push_back(scale);
		matricesMeshes.push_back(matNextNode);


		loadMesh(node["mesh"]);
	}

	//Check if the node has any childres...
	if (node.find("children") != node.end())
	{
		//If child/s exists, apply all the transformation to the child/s
		for (unsigned int i = 0; i < node["children"].size(); i++)
			traverseNode(node["children"][i], matNextNode);		//Pass the current matrix	
	}
}




std::vector<unsigned char> Model::getData()
{
	std::string bytesText;
	std::string uri = JSON["buffers"][0]["uri"];	//URI key gives data from .bin file which contains binary data

	std::string fileStr = std::string(file);		//string containing file contents
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);		//
	bytesText = get_file_contents((fileDirectory + uri).c_str());	//Look for the file in the filesystem

	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());		//Put the content into data from start to end
	return data;
}


std::vector<float> Model::getFloats(json accessor)
{

	std::vector<float> floatVec;
	/*
	* accessor[key] returns the value of key
	* accessor.value(key, 1) returns value of key if it exists, and 1 if non-exisistent
	*/

	unsigned int buffViewInd = accessor.value("bufferView", 1);		//index of bufferView points to buffer view
	unsigned int count = accessor["count"];							//How many floats to get
	unsigned int accBytesOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];	//type of data

	json bufferView = JSON["bufferViews"][buffViewInd];	//Using the index of bufferview create json variable of buffer view dictionary
	unsigned int byteOffset = bufferView["byteOffset"];	//using the bufferView and the key, obtain byte offset

	//Type checking of bufferview
	unsigned int numPerVert;
	if (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw std::invalid_argument("BufferView type invalid ( not SCALAR, VEC2/3/4...)");

	unsigned int beginningOfData = byteOffset + accBytesOffset;		//Total offset to start of bufferView
	unsigned int lengthOfData = count * 4 * numPerVert;

	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i)
	{
		unsigned char bytes[] = {data[i++], data[i++], data[i++], data[i++]};
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}
	return floatVec;
}

std::vector<GLuint> Model::getIndices(json accessor)
{
	std::vector<GLuint> indices;

	unsigned int buffViewInd = accessor.value("bufferView", 0);			//BufferView index key for buffer view
	unsigned int count = accessor["count"];								//How many datas?
	unsigned int accBytesOffset = accessor.value("byteOffset", 0);		//Offset
	unsigned int componentType = accessor["componentType"];				//What type is the data? (stored as int this time)

	json bufferView = JSON["bufferViews"][buffViewInd];					//Access bufferView with the key and store into json variable
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int beginningOfData = byteOffset + accBytesOffset;
	if (componentType == 5125)	//Check data type of index itself (It's unsigned int
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accBytesOffset + count * 4; i)
		{
			unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5123)	//Unsigned short
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accBytesOffset + count * 2; i)
		{
			unsigned char bytes[] = { data[i++], data[i++] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5122)	//Signed short
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accBytesOffset + count * 2; i)
		{
			unsigned char bytes[] = { data[i++], data[i++] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((GLuint)value);
		}
	}
	return indices;
}



//Automatic texture Assignement
std::vector<Texture> Model::getTextures()
{
	std::vector<Texture> textures;
	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	for (unsigned int i = 0; i < JSON["images"].size(); i++)
	{
		std::string texPath = JSON["images"][i]["uri"];	//Look for all images
		bool skip = false;

		//Iterate over all images
		for (unsigned int j = 0; j < loadedTexName.size(); j++) {
			if (loadedTexName[j] == texPath)
			{
				textures.push_back(loadedTex[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			//Texure type auto classify
			if (texPath.find("baseColor") != std::string::npos)
			{
				Texture diffuse = Texture((fileDirectory + texPath).c_str(), "diffuse", loadedTex.size());
				textures.push_back(diffuse);
				loadedTex.push_back(diffuse);
				loadedTexName.push_back(texPath);

			}

			else if (texPath.find("metallicRoughness") != std::string::npos)
			{
				Texture specular = Texture((fileDirectory + texPath).c_str(), "specular", loadedTex.size());
				textures.push_back(specular);
				loadedTex.push_back(specular);
				loadedTexName.push_back(texPath);
			}
		}
	}
	return textures;
}
//Functions to combine vertex and its attributes


std::vector<Vertex> Model::assembleVertices
(
	std::vector<glm::vec3> positions,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> texUVs
) 
{
	std::vector<Vertex> vertices;
	for (int i = 0; i < positions.size(); i++)
	{
		vertices.push_back
		(
			Vertex{
				positions[i],
				normals[i],
				glm::vec3(1.0f, 1.0f, 1.0f),
				texUVs[i],
				//No vertex color
			}
		);
	}
	return vertices;
}


//Grouping functions	
//Get all attribute keys for vertex

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec)
{
	std::vector<glm::vec2> vectors;
	for (int i = 0; i < floatVec.size(); i) {	//iterate over all float in float vec
		vectors.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
	}
	return vectors;
}

std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec)
{
	std::vector<glm::vec3> vectors;
	for (int i = 0; i < floatVec.size(); i) {	//iterate over all float in float vec
		vectors.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
	}
	return vectors;
}

std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec)
{
	std::vector<glm::vec4> vectors;
	for (int i = 0; i < floatVec.size(); i) {	//iterate over all float in float vec
		vectors.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
	}
	return vectors;
}
