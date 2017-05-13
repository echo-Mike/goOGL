#ifndef POINTLIGHT_H
#define POINTLIGHT_H "[0.0.4@CPointLight.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of point lightsource properties structure
*		for in-shader loading.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <string>
#include <type_traits>
//GLEW
#include <GL/glew.h>
//GLM
#include <GLM/glm.hpp>
//OUR
#include "assets\shader\CUniformStruct.h"
#include "general\CUniformVec4.h"
#include "general\CUniformNumber.h"
#include "assets\shader\CShader.h"
//DEBUG
#ifdef DEBUG_POINTLIGHT
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/* Helper struct with enum of in-shader struct members selectors.
*  Struct definition: PointLightStructComponents
*/
struct PointLightStructComponents {
	enum Data : int {
		POSITION,
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		CONSTANT,
		LINEAR,
		QUADRATIC
	};
};

/* Plain-old-data point lightsource properties structure.
*  Struct template definition: PointLightPOD
*/
template < class TVector4 = glm::vec4, class TNumber = float >
struct PointLightPOD : public PointLightStructComponents {
	TVector4 position;
	TVector4 ambient;
	TVector4 diffuse;
	TVector4 specular;
	float constant, linear, quadratic;

	PointLightPOD(	TVector4 _direction,TVector4 _ambient,
					TVector4 _diffuse,	TVector4 _specular,
					float _constant,	float _linear,
					float _quadratic	) : 
					ambient(_ambient),		diffuse(_diffuse),
					specular(_specular),	position(_direction),
					constant(_constant),	linear(_linear),
					quadratic(_quadratic)	{}

	PointLightPOD(	const PointLightPOD& other) :
					ambient(other.ambient),		diffuse(other.diffuse),
					specular(other.specular),	position(other.position),
					constant(other.constant),	linear(other.linear),
					quadratic(other.quadratic) {}
};

#ifndef POINT_LIGHT_NAMES
#define POINT_LIGHT_NAMES ___pointLightNames
	//Static array for structure names
	static const char* POINT_LIGHT_NAMES[] = {
		".position", ".ambient", ".diffuse", ".specular",
		".constant", ".linear", ".quadratic"
	};
#endif 

/* The storage for in-shader struct that represents point lightsource.
*  Struct template definition: PointLightsourceStorage
*/
template <	class TBase,				class TMemberVector, 
			class TMemberNumber,		class TMemberInterface, 
			class TVector4 = glm::vec4, class TNumber = float,	
			class TShader = Shader,		class TPOD = PointLightPOD<TVector4, TNumber> >
struct PointLightsourceStorage : public TBase {
	typedef TBase Base;
	typedef TMemberVector MemberVector;
	typedef TMemberNumber MemberNumber;
	typedef TMemberInterface MemberInterface;

	static_assert(std::is_base_of<TMemberInterface, TMemberVector>::value && std::is_base_of<TMemberInterface, TMemberNumber>::value,
		"WARNING::POINT_LIGHTSOURCE_STORAGE::TMemberVector and TMemberNumber must both be derived from TMemberInterface");

	/*
	*	position,	ambient
	*	diffuse,	specular,
	*	constant,	linear,	
	*	quadratic,
	*	shader,		structName
	*/
	PointLightsourceStorage(TVector4 position,	TVector4 ambient,
							TVector4 diffuse,	TVector4 specular,
							TNumber constant,	TNumber linear,
							TNumber quadratic,
							TShader* _shader,	std::string _structName)
	{
		structName = std::move(_structName);
		newElement<TMemberVector>(TMemberVector(&position, _shader, structName + DIRECTIONAL_LIGHT_NAMES[0]));
		newElement<TMemberVector>(TMemberVector(&ambient,  _shader, structName + DIRECTIONAL_LIGHT_NAMES[1]));
		newElement<TMemberVector>(TMemberVector(&diffuse,  _shader, structName + DIRECTIONAL_LIGHT_NAMES[2]));
		newElement<TMemberVector>(TMemberVector(&specular, _shader, structName + DIRECTIONAL_LIGHT_NAMES[3]));

		newElement<TMemberNumber>(TMemberNumber(&constant, _shader, structName + DIRECTIONAL_LIGHT_NAMES[4]));
		newElement<TMemberNumber>(TMemberNumber(&linear,   _shader, structName + DIRECTIONAL_LIGHT_NAMES[5]));
		newElement<TMemberNumber>(TMemberNumber(&quadratic,_shader, structName + DIRECTIONAL_LIGHT_NAMES[6]));
	}

	PointLightsourceStorage(TPOD _light, TShader* _shader, std::string _structName) :
							PointLightsourceStorage(_light.position,	_light.ambient,
													_light.diffuse,		_light.specular,
													_light.constant,	_light.linear,
													_light.quadratic,
													_shader,			_structName) {}

	PointLightsourceStorage() : PointLightsourceStorage(TVector4(), TVector4(), TVector4(), TVector4(), (TNumber)0, (TNumber)0, (TNumber)0, nullptr, "light") {}

	PointLightsourceStorage(const PointLightsourceStorage& other) {
		structName = other.structName.substr();
		TMemberInterface* _buff = nullptr;
		_buff = other.data[0];
		newElement<TMemberVector>(dynamic_cast<TMemberVector*>(_buff));
		_buff = other.data[1];
		newElement<TMemberVector>(dynamic_cast<TMemberVector*>(_buff));
		_buff = other.data[2];
		newElement<TMemberVector>(dynamic_cast<TMemberVector*>(_buff));
		_buff = other.data[3];
		newElement<TMemberVector>(dynamic_cast<TMemberVector*>(_buff));
		_buff = other.data[4];
		newElement<TMemberNumber>(dynamic_cast<TMemberNumber*>(_buff));
		_buff = other.data[5];
		newElement<TMemberNumber>(dynamic_cast<TMemberNumber*>(_buff));
		_buff = other.data[6];
		newElement<TMemberNumber>(dynamic_cast<TMemberNumber*>(_buff));
	}

	PointLightsourceStorage& operator=(PointLightsourceStorage other) {
		if (&other == this)
			return *this;
		TBase::operator=(other);
		return *this;
	}

	void operator() (TPOD _light, TShader* _shader, std::string _structName) {
		structName = std::move(_structName);
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector*>(_buff)->setValue(_light.position);
		dynamic_cast<TMemberVector*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[0]);
		dynamic_cast<TMemberVector*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TMemberVector*>(_buff)->setValue(_light.ambient);
		dynamic_cast<TMemberVector*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[1]);
		dynamic_cast<TMemberVector*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<TMemberVector*>(_buff)->setValue(_light.diffuse);
		dynamic_cast<TMemberVector*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[2]);
		dynamic_cast<TMemberVector*>(_buff)->setShader(_shader);
		_buff = data[3];
		dynamic_cast<TMemberVector*>(_buff)->setValue(_light.specular);
		dynamic_cast<TMemberVector*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[3]);
		dynamic_cast<TMemberVector*>(_buff)->setShader(_shader);
		_buff = data[4];
		dynamic_cast<TMemberNumber*>(_buff)->setValue(_light.constant);
		dynamic_cast<TMemberNumber*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[4]);
		dynamic_cast<TMemberNumber*>(_buff)->setShader(_shader);
		_buff = data[5];
		dynamic_cast<TMemberNumber*>(_buff)->setValue(_light.linear);
		dynamic_cast<TMemberNumber*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[5]);
		dynamic_cast<TMemberNumber*>(_buff)->setShader(_shader);
		_buff = data[6];
		dynamic_cast<TMemberNumber*>(_buff)->setValue(_light.quadratic);
		dynamic_cast<TMemberNumber*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[6]);
		dynamic_cast<TMemberNumber*>(_buff)->setShader(_shader);
	}

	void operator() (TPOD _light) {
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector*>(_buff)->setValue(_light.position);
		_buff = data[1];
		dynamic_cast<TMemberVector*>(_buff)->setValue(_light.ambient);
		_buff = data[2];
		dynamic_cast<TMemberVector*>(_buff)->setValue(_light.diffuse);
		_buff = data[3];
		dynamic_cast<TMemberVector*>(_buff)->setValue(_light.specular);
		_buff = data[4];
		dynamic_cast<TMemberNumber*>(_buff)->setValue(_light.constant);
		_buff = data[5];
		dynamic_cast<TMemberNumber*>(_buff)->setValue(_light.linear);
		_buff = data[6];
		dynamic_cast<TMemberNumber*>(_buff)->setValue(_light.quadratic);
	}

	void operator() (TShader* _shader) {
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TMemberVector*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<TMemberVector*>(_buff)->setShader(_shader);
		_buff = data[3];
		dynamic_cast<TMemberVector*>(_buff)->setShader(_shader);
		_buff = data[4];
		dynamic_cast<TMemberNumber*>(_buff)->setShader(_shader);
		_buff = data[5];
		dynamic_cast<TMemberNumber*>(_buff)->setShader(_shader);
		_buff = data[6];
		dynamic_cast<TMemberNumber*>(_buff)->setShader(_shader);
	}

	void operator() (std::string _structName) {
		structName = std::move(_structName);
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[0]));
		_buff = data[1];
		dynamic_cast<TMemberVector*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[1]));
		_buff = data[2];
		dynamic_cast<TMemberVector*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[2]));
		_buff = data[3];
		dynamic_cast<TMemberVector*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[3]));
		_buff = data[4];
		dynamic_cast<TMemberNumber*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[4]));
		_buff = data[5];
		dynamic_cast<TMemberNumber*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[5]));
		_buff = data[6];
		dynamic_cast<TMemberNumber*>(_buff)->setName(structName.substr().append(DIRECTIONAL_LIGHT_NAMES[6]));
	}

	#ifdef FWCPP17
		//Set value by selector C++17
		template < class T >
		void operator() (T& _value, PointLightStructComponents::Data _index = PointLightStructComponents::POSITION) {
			TMemberInterface* _buff = nullptr;
			_buff = data[_index];
			if constexpr (std::is_same<T, TVector4>::value) {
				dynamic_cast<TMemberVector*>(_buff)->setValue(_value);
			} else if constexpr (std::is_same<T, TNumber>::value) {
				dynamic_cast<TMemberNumber*>(_buff)->setValue(_value);
			}
		}
	#else
		//Set value by selector
		void operator() (TVector4& _value, PointLightStructComponents::Data _index = PointLightStructComponents::POSITION) {
			TMemberInterface* _buff = nullptr;
			_buff = data[_index];
			dynamic_cast<TMemberVector*>(_buff)->setValue(_value);
		}

		//Set value by selector
		void operator() (TNumber& _value, PointLightStructComponents::Data _index) {
			TMemberInterface* _buff = nullptr;
			_buff = data[_index];
			dynamic_cast<TMemberNumber*>(_buff)->setValue(_value);
		}
	#endif

	//Get value by selector
	template < class T >
	T operator() (PointLightStructComponents::Data _index, T& _typeValue) {
		TMemberInterface* _buff = nullptr;
		_buff = data[_index];
		if (std::is_same<T, TVector4>::value) {
			return dynamic_cast<TMemberVector*>(_buff)->getValue();
		} else if (std::is_same<T, TNumber>::value) {
			return dynamic_cast<TMemberNumber*>(_buff)->getValue();
		} else {
			return _typeValue;
		}
	}
};

/* The manual storage for in-shader struct that represents point lightsource.
*  Struct template definition: PointLightsourceManualStorage
*/
template < class TVector4 = glm::vec4, class TNumber = float, class TShader = Shader >
using PointLightsourceManualStorage = PointLightsourceStorage<	StructManualContainer,
																Vec4ManualStorage<TVector4, TShader>,
																NumberManualStorage<TNumber, TShader>,
																UniformManualInteface, 
																TVector4,
																TNumber,
																TShader >;

/* The automatic storage for in-shader struct that represents point lightsource.
*  Struct template definition: PointLightsourceAutomaticStorage
*/
template < class TVector4 = glm::vec4, class TNumber = float, class TShader = Shader >
using PointLightsourceAutomaticStorage = PointLightsourceStorage<	StructAutomaticContainer,
																	Vec4AutomaticStorage<TVector4, TShader>,
																	NumberAutomaticStorage<TNumber, TShader>,
																	UniformAutomaticInteface, 
																	TVector4,
																	TNumber,
																	TShader >;
#ifdef EXAMPLE_SHADERS
	const std::string PointLightShaderExampleVS(R"**(
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

	const std::string PointLightShaderExampleFS(R"**(
		#version 330 core
		struct Material {
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
			float shininess;
		};  

		struct Light {
			vec4 position;  
  
			vec4 ambient;
			vec4 diffuse;
			vec4 specular;
	
			float constant;
			float linear;
			float quadratic;
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
			vec3 ambient = light.ambient * material.ambient;
  	
			// Diffuse 
			vec3 norm = normalize(Normal);
			vec3 lightDir = normalize(light.position - FragPos);
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
			// Specular
			vec3 viewDir = normalize(viewPos - FragPos);
			vec3 reflectDir = reflect(-lightDir, norm);  
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
			vec3 specular = light.specular * (spec * material.specular);   
			
			float distance    = length(light.position - FragPos);
			float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   
			
			color = vec4(attenuation * (ambient + diffuse + specular), 1.0f);
		} 
	)**");
#endif
#endif