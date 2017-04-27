#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H "[0.0.3@CDirectionalLight.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of directional lightsource properties structure
*		for in-shader loading.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <string>
//GLEW
#include <GL/glew.h>
//GLM
#include <GLM/glm.hpp>
//OUR
#include "assets\shader\CUniforms.h"
#include "assets\shader\CUniformStruct.h"
#include "general\CUniformVec4.h"
#include "assets\shader\CShader.h"
//DEBUG
#ifdef DEBUG_DIRECTIONALLIGHT
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/* Plain-old-data directional lightsource properties structure.
*  Struct definition: DirectionalLightPOD
*/
struct DirectionalLightPOD {
	glm::vec4 direction;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	DirectionalLightPOD(glm::vec4 _direction,	glm::vec4 _ambient,
						glm::vec4 _diffuse,		glm::vec4 _specular ) : 
						ambient(_ambient),		diffuse(_diffuse),
						specular(_specular),	direction(_direction) {}

	DirectionalLightPOD(const DirectionalLightPOD& other) :
						ambient(other.ambient),		diffuse(other.diffuse),
						specular(other.specular),	direction(other.direction) {}
	
	enum Data : int {
		DIRECTION,
		AMBIENT,
		DIFFUSE,
		SPECULAR
	};
};

#ifndef DIRECTIONAL_LIGHT_NAMES
#define DIRECTIONAL_LIGHT_NAMES dataNames
	//Static array for structure names
	static const char* DIRECTIONAL_LIGHT_NAMES[] = {
		".direction", ".ambient", ".diffuse", ".specular"
	};
#endif 

/* The storage for in-shader struct that represents directional lightsource.
*  Struct template definition: DirectionalLightsourceStorage
*/
template < class TBase, class TMember, class TMemberInterface, class TVector4 = glm::vec4, class TShader = Shader >
struct DirectionalLightsourceStorage : public TBase {
	typedef TBase Base;
	typedef TMember Member;
	typedef TMemberInterface MemberInterface;

	DirectionalLightsourceStorage(	TVector4 direction, TVector4 ambient,
									TVector4 diffuse, TVector4 specular,
									TShader* _shader, std::string _structName)
	{
		structName = std::move(_structName);
		TMember _direction(&direction, _shader, structName.substr().append(DIRECTIONAL_LIGHT_NAMES[0]));
		newElement<TMember>(_direction);

		TMember _ambient(&ambient, _shader, structName.substr().append(DIRECTIONAL_LIGHT_NAMES[1]));
		newElement<TMember>(_ambient);

		TMember _diffuse(&diffuse, _shader, structName.substr().append(DIRECTIONAL_LIGHT_NAMES[2]));
		newElement<TMember>(_diffuse);

		TMember _specular(&specular, _shader, structName.substr().append(DIRECTIONAL_LIGHT_NAMES[3]));
		newElement<TMember>(_specular);
	}

	DirectionalLightsourceStorage(	DirectionalLightPOD _light, TShader* _shader, std::string _structName) :
									DirectionalLightsourceStorage(	_light.direction,	_light.ambient,
																	_light.diffuse,		_light.specular,
																	_shader,			_structName) {}

	DirectionalLightsourceStorage() : DirectionalLightsourceStorage(TVector4(), TVector4(), TVector4(), TVector4(), nullptr, "light") {}

	DirectionalLightsourceStorage(const DirectionalLightsourceStorage& other) {
		structName = other.structName.substr();
		TMemberInterface* _buff = nullptr;
		_buff = other.data[0];
		newElement<TMember>(dynamic_cast<TMember*>(_buff));
		_buff = other.data[1];
		newElement<TMember>(dynamic_cast<TMember*>(_buff));
		_buff = other.data[2];
		newElement<TMember>(dynamic_cast<TMember*>(_buff));
		_buff = other.data[3];
		newElement<TMember>(dynamic_cast<TMember*>(_buff));
	}

	DirectionalLightsourceStorage& operator=(DirectionalLightsourceStorage other) {
		if (&other == this)
			return *this;
		TBase::operator=(other);
		return *this;
	}

	void operator() (DirectionalLightPOD _light, TShader* _shader, std::string _structName) {
		structName = std::move(_structName);
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMember*>(_buff)->setValue(_light.direction);
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[0]));
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TMember*>(_buff)->setValue(_light.ambient);
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[1]));
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<TMember*>(_buff)->setValue(_light.diffuse);
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[2]));
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
		_buff = data[3];
		dynamic_cast<TMember*>(_buff)->setValue(_light.specular);
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[3]));
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
	}

	void operator() (DirectionalLightPOD _light) {
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMember*>(_buff)->setValue(_light.direction);
		_buff = data[1];
		dynamic_cast<TMember*>(_buff)->setValue(_light.ambient);
		_buff = data[2];
		dynamic_cast<TMember*>(_buff)->setValue(_light.diffuse);
		_buff = data[3];
		dynamic_cast<TMember*>(_buff)->setValue(_light.specular);
	}

	void operator() (TShader* _shader) {
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
		_buff = data[3];
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
	}

	void operator() (std::string _structName) {
		structName = std::move(_structName);
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[0]));
		_buff = data[1];
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[1]));
		_buff = data[2];
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[2]));
		_buff = data[3];
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[3]));
	}

	//Set value by selector
	void operator() (TVector4 _vector, DirectionalLightPOD::Data _index = DirectionalLightPOD::DIRECTION) {
		TMemberInterface* _buff = nullptr;
		_buff = data[_index];
		dynamic_cast<TMember*>(_buff)->setValue(_vector);
	}

	//Get value by selector
	TVector4 operator() (DirectionalLightPOD::Data _index = DirectionalLightPOD::DIRECTION, TVector4& _typeValue = glm::vec4()) {
		TMemberInterface* _buff = nullptr;
		_buff = data[_index];
		return dynamic_cast<TMember*>(_buff)->getValue();
	}
};

/* The manual storage for in-shader struct that represents directional lightsource.
*  Struct template definition: DirectionalLightsourceManualStorage
*/
template < class TVector4 = glm::vec4, class TShader = Shader >
using DirectionalLightsourceManualStorage = DirectionalLightsourceStorage<	StructManualContainer,
																			Vec4ManualStorage<TVector4, TShader>, 
																			UniformManualInteface, 
																			TVector4,
																			TShader >;

/* The automatic storage for in-shader struct that represents directional lightsource.
*  Struct template definition: DirectionalLightsourceAutomaticStorage
*/
template < class TVector4 = glm::vec4, class TShader = Shader >
using DirectionalLightsourceAutomaticStorage = DirectionalLightsourceStorage<	StructAutomaticContainer,
																				Vec4AutomaticStorage<TVector4, TShader>, 
																				UniformAutomaticInteface, 
																				TVector4,
																				TShader >;
#ifdef EXAMPLE_SHADERS
	const std::string DirectionalLightShaderExampleVS(R"**(
		#version 330 core

		layout(location = 0) in vec3 position;
		layout(location = 1) in vec3 normal;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
		uniform vec3 viewPos;

		out vec3 Normal;
		out vec3 FragPos;

		void main()
		{
			gl_Position = projection * view * model * vec4(position, 1.0); 
			FragPos = vec3(model * vec4(position, 1.0f));
			Normal = mat3(transpose(inverse(model))) * normal;
		}
	)**");

	const std::string DirectionalLightShaderExampleFS(R"**(
		#version 330 core
		struct Material {
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
			float shininess;
		};  

		struct Light {
			vec4 direction;
			vec4 ambient;
			vec4 diffuse;
			vec4 specular;
		};

		in vec3 FragPos;  
		in vec3 Normal;
  
		out vec4 color;
  
		uniform vec3 viewPos;
		uniform Material material;
		uniform Light light;

		void main()
		{
			// Ambient
			vec3 ambient = light.ambient.xyz * material.ambient;
  	
			// Diffuse 
			vec3 norm = normalize(Normal);
			vec3 lightDir = normalize(-light.direction).xyz;
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = light.diffuse.xyz * (diff * material.diffuse);
    
			// Specular
			vec3 viewDir = normalize(viewPos - FragPos);
			vec3 reflectDir = reflect(-lightDir, norm);  
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
			vec3 specular = light.specular.xyz * (spec * material.specular);   

			color = vec4(ambient + diffuse + specular, 1.0f);
		} 
	)**");
#endif
#endif