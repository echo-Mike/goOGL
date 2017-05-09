#ifndef MATERIAL_H
#define MATERIAL_H "[0.0.3@CMaterial.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of in-shader structure handler 
*		that represents simple material.
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
#include "assets\shader\CUniforms.h"
#include "assets\shader\CUniformStruct.h"
#include "general\CUniformVec3.h"
#include "general\CUniformNumber.h"
#include "assets\shader\CShader.h"
//DEBUG
#ifdef DEBUG_MATERIAL
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/* Helper struct with enum of in-shader struct members selectors.
*  Struct definition: MaterialStructComponents
*/
struct MaterialStructComponents {
	enum Data : int {
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		SHININESS
	};
};

/* Plain-old-data material structure.
*  Struct template definition: MaterialPOD
*/
template < class TVector3 = glm::vec3, class TNumber = float >
struct MaterialPOD : public MaterialStructComponents {
	TVector3 ambient;
	TVector3 diffuse;
	TVector3 specular;
	TNumber shininess;

	MaterialPOD(TVector3 _ambient, TVector3 _diffuse,
				TVector3 _specular, TNumber _shininess ) : 
				ambient(_ambient), diffuse(_diffuse),
				specular(_specular), shininess(_shininess) {}

	MaterialPOD(const MaterialPOD& other) : 
				ambient(other.ambient), diffuse(other.diffuse),
				specular(other.specular), shininess(other.shininess) {}
};

#ifndef MATERIAL_NAMES
#define MATERIAL_NAMES ___materialNames
	//Static array for structure names
	static const char* MATERIAL_NAMES[] = {
		"[0].ambient", "[0].diffuse", "[0].specular", "[0].shininess"
	};
#endif 

/* The storage for in-shader struct that represents simple material.
*  Struct template definition: MaterialStorage
*/
template <	class TBase,				class TMemberInterface,			
			class TMemberVector3,		class TMemberNumber,		
			class TVector3 = glm::vec3, class TNumber = float,
			class TShader = Shader,		class TPOD = MaterialPOD<TVector3, TNumber> >
struct MaterialStorage : public TBase {
	typedef TBase Base;
	typedef TMemberVector3 MemberVector3;
	typedef TMemberNumber MemberNumber;
	typedef TMemberInterface MemberInterface;
	typedef TPOD POD;

	static_assert(	std::is_base_of<TMemberInterface, TMemberVector3>::value && std::is_base_of<TMemberInterface, TMemberNumber>::value,
					"WARNING::MATERIAL_STORAGE::TMemberVector3 and TMemberNumber must both be derived from TMemberInterface");

	MaterialStorage(TVector3 ambient, TVector3 diffuse,
					TVector3 specular, TNumber shininess,
					Shader* _shader, std::string _structName) 
	{
		structName = std::move(_structName);
		newElement<TMemberVector3>(TMemberVector3(&ambient,  _shader, structName + MATERIAL_NAMES[0]));
		newElement<TMemberVector3>(TMemberVector3(&diffuse,  _shader, structName + MATERIAL_NAMES[1]));
		newElement<TMemberVector3>(TMemberVector3(&specular, _shader, structName + MATERIAL_NAMES[2]));
		newElement<TMemberNumber>(TMemberNumber(&shininess,  _shader, structName + MATERIAL_NAMES[3]));
	}

	MaterialStorage(TPOD _material, Shader* _shader, std::string _structName) :
					MaterialStorage(_material.ambient,	_material.diffuse, 
									_material.specular,	_material.shininess,
									_shader,			_structName) {}

	MaterialStorage() : MaterialStorage(TVector3(), TVector3(), TVector3(), (TNumber)0, nullptr, "material") {}

	MaterialStorage(const MaterialManualStorage& other) {
		structName = other.structName.substr();
		UniformManualInteface* _buff = nullptr;
		_buff = other.data[0];
		newElement<TMemberVector3>(dynamic_cast<TMemberVector3*>(_buff));
		_buff = other.data[1];
		newElement<TMemberVector3>(dynamic_cast<TMemberVector3*>(_buff));
		_buff = other.data[2];
		newElement<TMemberVector3>(dynamic_cast<TMemberVector3*>(_buff));
		_buff = other.data[3];
		newElement<TMemberNumber>(dynamic_cast<TMemberNumber*>(_buff));
	}

	MaterialStorage& operator=(MaterialStorage other) {
		if (&other == this)
			return *this;
		TBase::operator=(other);
		return *this;
	}

	void operator() (TPOD _material, Shader* _shader, std::string _structName) {
		structName = std::move(_structName);
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_material.ambient);
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + MATERIAL_NAMES[0]);
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_material.diffuse);
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + MATERIAL_NAMES[1]);
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_material.specular);
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + MATERIAL_NAMES[2]);
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
		_buff = data[3];
		dynamic_cast<TMemberNumber*>(_buff)->setValue( _material.shininess);
		dynamic_cast<TMemberNumber*>(_buff)->setName( structName + MATERIAL_NAMES[3]);
		dynamic_cast<TMemberNumber*>(_buff)->setShader( _shader);
	}

	void operator() (MaterialPOD _material) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_material.ambient);
		_buff = data[1];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_material.diffuse);
		_buff = data[2];
		dynamic_cast<TMemberVector3*>(_buff)->setValue(_material.specular);
		_buff = data[3];
		dynamic_cast<TMemberNumber*>(_buff)->setValue( _material.shininess);
	}

	void operator() (Shader* _shader) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<TMemberVector3*>(_buff)->setShader(_shader);
		_buff = data[3];
		dynamic_cast<TMemberNumber*>(_buff)->setShader(_shader);
	}

	void operator() (std::string _structName) {
		structName = std::move(_structName);
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + MATERIAL_NAMES[0]);
		_buff = data[1];
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + MATERIAL_NAMES[1]);
		_buff = data[2];
		dynamic_cast<TMemberVector3*>(_buff)->setName(structName + MATERIAL_NAMES[2]);
		_buff = data[3];
		dynamic_cast<TMemberNumber*>(_buff)->setName( structName + MATERIAL_NAMES[3]);
	}

	#ifdef FWCPP17
		//Set value by selector C++17
		template < class T >
		void operator() (T& _value, MaterialStructComponents::Data _index) {
			TMemberInterface* _buff = nullptr;
			_buff = data[_index];
			if constexpr (std::is_same<T, TVector3>::value) {
				dynamic_cast<TMemberVector3*>(_buff)->setValue(_value);
			} else if constexpr (std::is_same<T, TNumber>::value) {
				dynamic_cast<TMemberNumber*>(_buff)->setValue(_value);
			}
		}
	#else
		//Set value by selector
		void operator() (TVector3& _value, MaterialStructComponents::Data _index) {
			UniformManualInteface* _buff = nullptr;
			_buff = data[_index];
			dynamic_cast<TMemberVector3*>(_buff)->setValue(_value);
		}

		//Set value by selector
		void operator() (TNumber& _value, MaterialStructComponents::Data _index) {
			UniformManualInteface* _buff = nullptr;
			_buff = data[_index];
			dynamic_cast<TMemberNumber*>(_buff)->setValue(_value);
		}
	#endif

	//Get values of any type by selector (for float use "new float" as argument)
	template < class T >
	T operator() (MaterialStructComponents::Data _index, T &_typeValue) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[_index];
		if (std::is_same<T, TVector3>::value) {
			return dynamic_cast<TMemberVector3*>(_buff)->getValue();
		} else if (std::is_same<T, TNumber>::value) {
			return dynamic_cast<TMemberNumber*>(_buff)->getValue();
		} else {
			return _typeValue;
		}
	}
};

/* The manual storage for in-shader struct that represents simple material.
*  Struct template definition: MaterialManualStorage
*/
template < class TVector3 = glm::vec3, class TNumber = float, class TShader = Shader >
using MaterialManualStorage = MaterialStorage<	StructManualContainer,
												UniformManualInteface,
												Vec3ManualStorage<TVector3, TShader>,
												NumberManualStorage<TNumber, TShader>,
												TVector3,
												TNumber,
												TShader >;

/* The automatic storage for in-shader struct that represents simple material.
*  Struct template definition: MaterialAutomaticStorage
*/
template < class TVector3 = glm::vec3, class TNumber = float, class TShader = Shader >
using MaterialAutomaticStorage = MaterialStorage<	StructAutomaticContainer,
													UniformAutomaticInteface, 
													Vec3AutomaticStorage<TVector3, TShader>,
													NumberAutomaticStorage<TNumber, TShader>,
													TVector3,
													TNumber,
													TShader >;

#endif