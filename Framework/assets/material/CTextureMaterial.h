#ifndef TEXTUREMATERIAL_H
#define TEXTUREMATERIAL_H "[0.0.3@CTextureMaterial.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of in-shader structure handler 
*		that represents material by texture maps (diffuse and specular).
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
//OUR
#include "assets\shader\CUniforms.h"
#include "assets\shader\CUniformStruct.h"
#include "general\CUniformNumber.h"
#include "assets\textures\CTexture.h"
#include "assets\textures\CUniformTexture.h"
#include "assets\shader\CShader.h"
//DEBUG
#ifdef DEBUG_TEXTUREMATERIAL
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/* Helper struct with enum of in-shader struct members selectors.
*  Struct definition: TextureMaterialStructComponents
*/
struct TextureMaterialStructComponents {
	enum Data : int {
		DIFFUSE,
		SPECULAR,
		SHININESS
	};
};

/* Plain-old-data material structure with texture maps.
*  Struct template definition: TextureMaterialPOD
*/
template < class TNumber = float >
struct TextureMaterialPOD : public TextureMaterialStructComponents {
	std::string diffuseMap;
	std::string specularMap;
	TNumber shininess;

	/*
	* _diffuseMap : path to diffuse texture
	* _specularMap : path to specular texture
	* _shininess : the shininess amount of object
	*/
	TextureMaterialPOD(	std::string &_diffuseMap, 
						std::string &_specularMap,
						TNumber _shininess) :
						diffuseMap(_diffuseMap),
						specularMap(_specularMap),
						shininess(_shininess) {}

	/*
	* _diffuseMap : path to diffuse texture
	* _specularMap : path to specular texture
	* _shininess : the shininess amount of object
	*/
	TextureMaterialPOD(	const std::string &_diffuseMap, 
						const std::string &_specularMap,
						TNumber _shininess) :
						diffuseMap(_diffuseMap),
						specularMap(_specularMap),
						shininess(_shininess) {}

	TextureMaterialPOD(	const TextureMaterialPOD& other) :
						diffuseMap(other.diffuseMap), 
						specularMap(other.specularMap),
						shininess(other.shininess) {}
};

#ifndef TEXTURE_MATERIAL_NAMES
#define TEXTURE_MATERIAL_NAMES ___textureMaterialNames
	//Static array for structure names
	static const char* TEXTURE_MATERIAL_NAMES[] = {
		"[0].diffuse", "[0].specular", "[0].shininess"
	};
#endif 

/* The storage for in-shader struct that represents material by texture maps (diffuse and specular).
*  Struct template definition: TextureMaterialStorage
*/
template <	class TBase,				class TMemberInterface,			
			class TMemberTexture,		class TMemberNumber,		
			class TTexture = Texture,	class TNumber = float,
			class TShader = Shader,		class TPOD = TextureMaterialPOD<TNumber> >
struct TextureMaterialStorage : public TBase {
	typedef TBase Base;
	typedef TMemberTexture MemberTexture;
	typedef TMemberNumber  MemberNumber;
	typedef TMemberInterface MemberInterface;
	typedef TPOD POD;

	static_assert(	std::is_base_of<TMemberInterface, TMemberTexture>::value && std::is_base_of<TMemberInterface, TMemberNumber>::value,
					"WARNING::TEXTURE_MATERIAL_STORAGE::TMemberTexture and TMemberNumber must both be derived from TMemberInterface");

	TextureMaterialStorage(	TTexture diffuseMap,
							TTexture specularMap, 
							TNumber shininess,
							TShader* _shader, 
							std::string _structName) 
	{
		structName = std::move(_structName);
		newElement<TMemberTexture>(TMemberTexture(&diffuseMap,  _shader, 0, 0, structName + TEXTURE_MATERIAL_NAMES[0]));
		newElement<TMemberTexture>(TMemberTexture(&specularMap, _shader, 0, 0, structName + TEXTURE_MATERIAL_NAMES[1]));
		newElement<TMemberNumber>(TMemberNumber(&shininess,		_shader, structName + TEXTURE_MATERIAL_NAMES[2]));

		dynamic_cast<TMemberTexture*>(data[0])->LoadTexture();
		dynamic_cast<TMemberTexture*>(data[1])->LoadTexture();
	}

	TextureMaterialStorage(	std::string diffuseMap,
							std::string specularMap,
							TNumber shininess,
							TShader* _shader,
							std::string _structName) :
							TextureMaterialStorage(TTexture(diffuseMap), TTexture(specularMap), shininess, _shader, _structName) {}

	TextureMaterialStorage(	TPOD _material, TShader* _shader, std::string _structName) :
							TextureMaterialStorage( _material.diffuseMap, _material.specularMap, _material.shininess, _shader, _structName) {}

	TextureMaterialStorage() : TextureMaterialStorage(TTexture(), TTexture(), (TNumber)0, nullptr, "material") {}

	TextureMaterialStorage(const TextureMaterialStorage& other) {
		structName = other.structName.substr();
		UniformManualInteface* _buff = nullptr;
		_buff = other.data[0];
		newElement<TMemberTexture>(dynamic_cast<TMemberTexture*>(_buff));
		_buff = other.data[1];
		newElement<TMemberTexture>(dynamic_cast<TMemberTexture*>(_buff));
		_buff = other.data[2];
		newElement<TMemberNumber>(dynamic_cast<TMemberNumber*>(_buff));

		dynamic_cast<TMemberTexture*>(data[0])->LoadTexture();
		dynamic_cast<TMemberTexture*>(data[1])->LoadTexture();
	}

	MaterialManualStorage& operator=(MaterialManualStorage other) {
		if (&other == this)
			return *this;
		TBase::operator=(other);
		dynamic_cast<TMemberTexture*>(data[0])->LoadTexture();
		dynamic_cast<TMemberTexture*>(data[1])->LoadTexture();
		return *this;
	}

	void operator() (TPOD _material, Shader* _shader, std::string _structName) {
		structName = std::move(_structName);
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberTexture*>(_buff)->setValue(TTexture(_material.diffuseMap));
		dynamic_cast<TMemberTexture*>(_buff)->setName(structName + TEXTURE_MATERIAL_NAMES[0]);
		dynamic_cast<TMemberTexture*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TMemberTexture*>(_buff)->setValue(TTexture(_material.specularMap));
		dynamic_cast<TMemberTexture*>(_buff)->setName(structName + TEXTURE_MATERIAL_NAMES[1]);
		dynamic_cast<TMemberTexture*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<TMemberNumber*>(_buff)->setValue(_material.shininess);
		dynamic_cast<TMemberNumber*>(_buff)->setName(structName + TEXTURE_MATERIAL_NAMES[2]);
		dynamic_cast<TMemberNumber*>(_buff)->setShader(_shader);

		dynamic_cast<TMemberTexture*>(data[0])->LoadTexture();
		dynamic_cast<TMemberTexture*>(data[1])->LoadTexture();
	}

	void operator() (TPOD _material) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberTexture*>(_buff)->setValue(TTexture(_material.diffuseMap));
		_buff = data[1];
		dynamic_cast<TMemberTexture*>(_buff)->setValue(TTexture(_material.specularMap));
		_buff = data[2];
		dynamic_cast<TMemberNumber*>(_buff)->setValue(_material.shininess);

		dynamic_cast<TMemberTexture*>(data[0])->LoadTexture();
		dynamic_cast<TMemberTexture*>(data[1])->LoadTexture();
	}

	void operator() (Shader* _shader) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberTexture*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TMemberTexture*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<TMemberNumber*>(_buff)->setShader(_shader);
	}

	void operator() (std::string _structName) {
		structName = std::move(_structName);
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMemberTexture*>(_buff)->setName(structName + TEXTURE_MATERIAL_NAMES[0]);
		_buff = data[1];
		dynamic_cast<TMemberTexture*>(_buff)->setName(structName + TEXTURE_MATERIAL_NAMES[1]);
		_buff = data[2];
		dynamic_cast<TMemberNumber*>(_buff)->setName(structName + TEXTURE_MATERIAL_NAMES[2]);
	}
	
	#ifdef FWCPP17
		//Set value by selector C++17
		template < class T >
		void operator() (T& _value, TextureMaterialStructComponents::Data _index) {
			TMemberInterface* _buff = nullptr;
			_buff = data[_index];
			if constexpr (std::is_same<T, TTexture>::value) {
				dynamic_cast<TMemberTexture*>(_buff)->setValue(_value);
				dynamic_cast<TMemberTexture*>(_buff)->LoadTexture();
			} else if constexpr (std::is_same<T, TNumber>::value) {
				dynamic_cast<TMemberNumber*>(_buff)->setValue(_value);
			}
		}
	#else
		//Set values of TTexture type
		void operator() (TTexture& _value, TextureMaterialStructComponents::Data _index) {
			UniformManualInteface* _buff = nullptr;
			_buff = data[_index];
			dynamic_cast<TMemberTexture*>(_buff)->setValue(_texture);
			dynamic_cast<TMemberTexture*>(_buff)->LoadTexture();
		}

		//Set values of float type
		void operator() (TNumber& _value, TextureMaterialStructComponents::Data _index) {
			UniformManualInteface* _buff = nullptr;
			_buff = data[_index];
			dynamic_cast<TMemberNumber*>(_buff)->setValue(_value);
		}
	#endif

	//Get values of any type by selector (for float use "new float" as argument)
	template < class T >
	T operator() (TextureMaterialStructComponents::Data _index, T &_typeValue) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[_index];
		if (std::is_same<T, TTexture>::value) {
			return dynamic_cast<TMemberTexture*>(_buff)->getValue();
		} else if (std::is_same<T, TNumber>::value) {
			return dynamic_cast<TMemberNumber*>(_buff)->getValue();
		} else {
			return _typeValue;
		}
	}

	void setTextureSlot(GLuint newSlot, TextureMaterialStructComponents::Data _index) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[_index];
		dynamic_cast<TMemberTexture*>(_buff)->setTextureSlot(newSlot);
	}

	void setTextureUnit(int newUnit, TextureMaterialStructComponents::Data _index) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[_index];
		dynamic_cast<TMemberTexture*>(_buff)->setTextureUnit(newUnit);
	}
};

/* The manual storage for in-shader struct that represents material by texture maps (diffuse and specular).
*  Struct template definition: TextureMaterialManualStorage
*/
template < class TTexture = Texture, class TNumber = float, class TShader = Shader >
using TextureMaterialManualStorage = TextureMaterialStorage<StructManualContainer,
															UniformManualInteface,
															TextureManualStorage<TTexture, TShader>,
															NumberManualStorage<TNumber, TShader>,
															TTexture,
															TNumber,
															TShader >;

/* The automatic storage for in-shader struct that represents material by texture maps (diffuse and specular).
*  Struct template definition: TextureMaterialAutomaticStorage
*/
template < class TTexture = Texture, class TNumber = float, class TShader = Shader >
using TextureMaterialAutomaticStorage = TextureMaterialStorage<	StructAutomaticContainer,
																UniformAutomaticInteface, 
																TextureAutomaticStorage<TTexture, TShader>,
																NumberAutomaticStorage<TNumber, TShader>,
																TTexture,
																TNumber,
																TShader >;
#endif