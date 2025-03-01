#include"Texture.h"

Texture::Texture(const char* image, const char* texType, GLuint slot)
{
	type = texType;
	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);	//Flip the texutre coordinate vertically

	//Load in image
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	//Create reference val for openGL to load texture
	glGenTextures(1, &ID);	//Pass pointer
	glActiveTexture(GL_TEXTURE0 + slot);	//Activate texture
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID);

	//Texture interpolation when scaled
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);		//Nearrest -> pixel enlarged, linear -> blured
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);		//Min/Mag filter

	//Texture clipping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);			//Horizontal and vertical can have diffrenet clipping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//For clamp border
	//float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};		//Black
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
	// 
	// 
	// Check what type of color channels the texture has and load it accordingly
	if (numColCh == 4)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 3)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			bytes
		);

	else if (numColCh == 1)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else	
		throw std::invalid_argument("Automatic Texture type recognition failed");

	//Generate mipmap (LOD for textures) scales down resolution if far away
	glGenerateMipmap(GL_TEXTURE_2D);

	//Delete the image after importing
	stbi_image_free(bytes);		//Free memomry
	glBindTexture(GL_TEXTURE_2D, 0);		//Unbind texture
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint  unit)
{
	//Uniform for the texture
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(texUni, unit);
}


void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}