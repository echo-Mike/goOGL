#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H "[0.0.5@CDirectionalLight.h]"
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
#include <type_traits>
//GLEW
#include <GL/glew.h>
//GLM
#include <GLM/glm.hpp>
//OUR
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

/* Helper struct with enum of in-shader struct members selectors.
*  Struct definition: DirectionalLightStructComponents
*/
struct DirectionalLightStructComponents {
	enum Data : int {
		DIRECTION,
		AMBIENT,
		DIFFUSE,
		SPECULAR
	};
};

/* Plain-old-data directional lightsource properties structure.
*  Struct template definition: DirectionalLightPOD
*/
template < class TVector4 = glm::vec4, class TVector3 = glm::vec3 >
struct DirectionalLightPOD : public DirectionalLightStructComponents {
	TVector4 direction;
	TVector3 ambient;
	TVector3 diffuse;
	TVector3 specular;

	/*
	* direction, ambient,
	* diffuse,	 specular
	*/
	DirectionalLightPOD(TVector4 _direction,	TVector3 _ambient,
						TVector3 _diffuse,		TVector3 _specular ) : 
						ambient(_ambient),		diffuse(_diffuse),
						specular(_specular),	direction(_direction) {}

	DirectionalLightPOD(const DirectionalLightPOD& other) :
						ambient(other.ambient),		diffuse(other.diffuse),
						specular(other.specular),	direction(other.direction) {}
};

#ifndef DIRECTIONAL_LIGHT_NAMES
#define DIRECTIONAL_LIGHT_NAMES ___directionalLightNames
	//Static array for structure names
	static const char* DIRECTIONAL_LIGHT_NAMES[] = {
		".direction", ".ambient", ".diffuse", ".specular"
	};
#endif 

/* The storage for in-shader struct that represents directional lightsource.
*  Struct template definition: DirectionalLightsourceStorage
*/
template <	class TBase,				class TMemberInterface,			
			class TMemberVector4,		class TMemberVector3,		
			class TVector4 = glm::vec4,	class TVector3 = glm::vec3,
			class TShader = Shader,		class TPOD = DirectionalLightPOD<TVector4, TVector3> >
struct DirectionalLightsourceStorage : public TBase {
	typedef TBase Base;
	typedef TMemberVector4 MemberVector4;
	typedef TMemberVector3 MemberVector3;
	typedef TMemberInterface MemberInterface;
	typedef TPOD POD;

	static_assert(	std::is_base_of<TMemberInterface, TMemberVector4>::value && std::is_base_of<TMemberInterface, TMemberVector3>::value, 
					"ASSERT_ERROR::DIRECTIONAL_LIGHTSOURCE_STORAGE::TMemberVector4 and TMemberVector3 must both be derived from TMemberInterface");

	DirectionalLightsourceStorage(	TVector4 direction,	TVector3 ambient,
									TVector3 diffuse,	TVector3 specular,
									TShader* _shader,	std::string _structName)
	{
		structName = std::move(_structName);
		newElement<TMemberVector4>(TMemberVector4(&direction, _shader, structName + DIRECTIONAL_LIGHT_NAMES[0]));
		newElement<TMemberVector3>(TMemberVector3(&ambient,   _shader, structName + DIRECTIONAL_LIGHT_NAMES[1]));
		newElement<TMemberVector3>(TMemberVector3(&diffuse,   _shader, structName + DIRECTIONAL_LIGHT_NAMES[2]));
		newElement<TMemberVector3>(TMemberVector3(&specular,  _shader, structName + DIRECTIONAL_LIGHT_NAMES[3]));
	}

	DirectionalLightsourceStorage(	TPOD _light, TShader* _shader, std::string _structName) :
									DirectionalLightsourceStorage(	_light.direction,	_light.ambient,
																	_light.diffuse,		_light.specular,
																	_shader,			_structName) {}

	DirectionalLightsourceStorage() : DirectionalLightsourceStorage(TVector4(), TVector3(), TVector3(), TVector3(), nullptr, "light") {}

	DirectionalLightsourceStorage(const DirectionalLightsourceStorage& other) {
		structName = other.structName.substr();
		TMemberInterface* _buff = nullptr;
		_buff = other.data[0];
		newElement<TMemberVector4>(dynamic_cast<TMemberVector4*>(_buff));
		_buff = other.data[1];
		newElement<TMemberVector3>(dynamic_cast<TMemberVector3*>(_buff));
		_buff = other.data[2];
		newElement<TMemberVector3>(dynamic_cast<TMemberVector3*>(_buff));
		_buff = other.data[3];
		newElement<TMemberVector3>(dynamic_cast<TMemberVector3*>(_buff));
	}

	DirectionalLightsourceStorage(DirectionalLightsourceStorage&& other) : TBase(std::move(other)) {}

	DirectionalLightsourceStorage& operator=(DirectionalLightsourceStorage other) {
		if (&other == this)
			return *this;
		TBase::operator=(other);
		return *this;
	}

	void operator() (TPOD _light, TShader* _shader, std::string _structName) {
		structName = std::move(_structName);
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector4*>(_buff)->setValue(_light.direction);
		dynamic_cast<TMemberVector4*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[0]);
		dynamic_cast<TMemberVector4*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_light.ambient);
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[1]);
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_light.diffuse);
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[2]);
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
		_buff = data[3];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_light.specular);
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[3]);
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
	}

	void operator() (TPOD _light) {
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector4*>(_buff)->setValue(_light.direction);
		_buff = data[1];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_light.ambient);
		_buff = data[2];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_light.diffuse);
		_buff = data[3];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_light.specular);
	}

	void operator() (TShader* _shader) {
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector4*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
		_buff = data[3];
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
	}

	void operator() (std::string _structName) {
		structName = std::move(_structName);
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector4*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[0]);
		_buff = data[1];
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[1]);
		_buff = data[2];
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[2]);
		_buff = data[3];
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + DIRECTIONAL_LIGHT_NAMES[3]);
	}
	
	#ifdef FWCPP17
		//Set value by selector C++17
		template < class T >
		void operator() (T& _value, DirectionalLightStructComponents::Data _index = TPOD::DIRECTION) {
			TMemberInterface* _buff = nullptr;
			_buff = data[_index];
			if constexpr (std::is_same<T, TVector4>::value) {
				dynamic_cast<TMemberVector4*>(_buff)->setValue(_value);
			} else if constexpr (std::is_same<T, TVector3>::value) {
				dynamic_cast<TMemberVector3*>(_buff)->setValue(_value);
			}
		}
	#else
		//Set value by selector
		void operator() (TVector4& _value, DirectionalLightStructComponents::Data _index = TPOD::DIRECTION) {
			TMemberInterface* _buff = nullptr;
			_buff = data[_index];
			dynamic_cast<TMemberVector4*>(_buff)->setValue(_value);
		}

		//Set value by selector
		void operator() (TVector3& _value, DirectionalLightStructComponents::Data _index) {
			TMemberInterface* _buff = nullptr;
			_buff = data[_index];
			dynamic_cast<TMemberVector3*>(_buff)->setValue(_value);
		}
	#endif

	//Get value by selector
	template < class T >
	T operator() (DirectionalLightStructComponents::Data _index, T& _typeValue) {
		TMemberInterface* _buff = nullptr;
		_buff = data[_index];
		if (std::is_same<T, TVector4>::value) {
			return dynamic_cast<TMemberVector4*>(_buff)->getValue();
		} else if (std::is_same<T, TVector3>::value) {
			return dynamic_cast<TMemberVector3*>(_buff)->getValue();
		} else {
			return _typeValue;
		}
	}
};

/* The manual storage for in-shader struct that represents directional lightsource.
*  Struct template definition: DirectionalLightsourceManualStorage
*/
template < class TVector4 = glm::vec4, class TVector3 = glm::vec3, class TShader = Shader >
using DirectionalLightsourceManualStorage = DirectionalLightsourceStorage<	StructManualContainer,
																			UniformManualInteface,
																			Vec4ManualStorage<TVector4, TShader>, 
																			Vec3ManualStorage<TVector3, TShader>, 
																			TVector4,
																			TVector3,
																			TShader >;

/* The automatic storage for in-shader struct that represents directional lightsource.
*  Struct template definition: DirectionalLightsourceAutomaticStorage
*/
template < class TVector4 = glm::vec4, class TVector3 = glm::vec3, class TShader = Shader >
using DirectionalLightsourceAutomaticStorage = DirectionalLightsourceStorage<	StructAutomaticContainer,
																				UniformAutomaticInteface, 
																				Vec4AutomaticStorage<TVector4, TShader>, 
																				Vec3AutomaticStorage<TVector3, TShader>,
																				TVector4,
																				TVector3,
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
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
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
			vec3 lightDir = normalize(-light.direction).xyz;
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
			// Specular
			vec3 viewDir = normalize(viewPos - FragPos);
			vec3 reflectDir = reflect(-lightDir, norm);  
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
			vec3 specular = light.specular * (spec * material.specular);   

			color = vec4(ambient + diffuse + specular, 1.0f);
		} 
	)**");
#endif
#endif