#version 330 core		//Specify GLSL version

//Output of the fragment shader
out vec4 FragColor;		//Function output returns vec4 containing Fragment color



//Input from the vertex shader
in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;



//Texture unit from the main func	//uniform datatype variableName
uniform sampler2D diffuse0;		//tex
uniform sampler2D specular0;	//Spec


//Light color vector
uniform vec4 lightColor;

uniform vec3 lightPos;
// Gets the position of the camera from the main function

//Get Camera position
uniform vec3 camPos;


vec4 pointLight()
{
	vec3 lightVec = lightPos - crntPos;
	float dist = length(lightVec);	//calculate the distance

	//Intensity = 1/(ad^2 + bd + 1)
	float a = 3.0;
	float b = 0.7;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	//Ambient lighting
	float ambient = 0.2f;
	
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);

	//Diffuse lighting
	float diffuse = max(dot(normal, lightDirection), 0.0f);	//Maximum normal from dot product of normal and light direction vector (since dot product maybe -ve, take max)
	
	//Specular lighting
	float specularLight = 0.50f;	//Specular strength multiplier
	vec3 viewDirection = normalize(camPos - crntPos);		//Direction from the camera to the object (crntPos)
	vec3 reflectionDirection = reflect(-lightDirection, normal);		//Light bounce to normal and gets reflected
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);	// Calculate the amount of specular reflection pow => regular math power ^8
	float specular = specAmount * specularLight;

	FragColor = texture(diffuse0, texCoord) * lightColor * (diffuse + ambient) + texture(specular0, texCoord) * specular;		//Multiply texture by the color
	//			texture						Light					Ambient						Specular
	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor ;		//Multiply texture by the color

}


vec4 directLight()
{
	float ambient = 0.50f;
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0));		//Constant light direction vector

	//Diffuse lighting
	float diffuse = max(abs(dot(normal, lightDirection)), 0.0f);	//Maximum normal from dot product of normal and light direction vector (since dot product maybe -ve, take max)
	

	//Specular lighting
	float specularLight = 0.50f;	//Specular strength multiplier
	vec3 viewDirection = normalize(camPos - crntPos);		//Direction from the camera to the object (crntPos)
	vec3 reflectionDirection = reflect(-lightDirection, normal);		//Light bounce to normal and gets reflected
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);	// Calculate the amount of specular reflection pow => regular math power ^8
	float specular = specAmount * specularLight;

	if(texture(diffuse0, texCoord).a < 0.1)
	{
	discard;
	}

	FragColor = texture(diffuse0, texCoord) * lightColor * (diffuse + ambient) + texture(specular0, texCoord) * specular;		//Multiply texture by the color
	//			texture						Light					Ambient						Specular
	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor ;		//Multiply texture by the color

}



vec4 spotLight()
{
	float outerCone = 0.90f;
	float innerCone = 0.95f;


	//Ambient lighting
	float ambient = 0.5f;
	
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);

	//Diffuse lighting
	float diffuse = max(dot(normal, lightDirection), 0.0f);	//Maximum normal from dot product of normal and light direction vector (since dot product maybe -ve, take max)
	
	//Specular lighting
	float specularLight = 0.50f;	//Specular strength multiplier
	vec3 viewDirection = normalize(camPos - crntPos);		//Direction from the camera to the object (crntPos)
	vec3 reflectionDirection = reflect(-lightDirection, normal);		//Light bounce to normal and gets reflected
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);	// Calculate the amount of specular reflection pow => regular math power ^8
	float specular = specAmount * specularLight;

	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);		//Clamp the out 0 to 1


	FragColor = texture(diffuse0, texCoord) * lightColor * (diffuse + ambient) + texture(specular0, texCoord) * specular;		//Multiply texture by the color
	//			texture						Light					Ambient						Specular
	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor ;		//Multiply texture by the color

}

vec4 celLight()
{

	float ambient = 0.50f;
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0));		//Constant light direction vector

	//Diffuse lighting
	float diffuse = max(dot(normal, lightDirection), 0.0f);	//Maximum normal from dot product of normal and light direction vector (since dot product maybe -ve, take max)
	if(diffuse > 0.5)
	{
		return texture(diffuse0, texCoord) * (diffuse + ambient) * lightColor;		//Multiply texture by the color
	}
	else
	{
		return vec4(0.1f, 0.1f, 0.1f, 0.1f);
	}
	
}

float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness = 0.5f, float offset = 5.0f)
{
	//steepness => gradient of depth color change
	//offset => depth to start gradient
	float zVal = linearizeDepth(depth);
	return (1/ (1+exp(-steepness * (zVal - offset))));
}

void main()
{
	FragColor = celLight();
}