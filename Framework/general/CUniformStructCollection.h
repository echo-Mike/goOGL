#ifndef UNIFORMSTRUCTCOLLECTION_H
#define UNIFORMSTRUCTCOLLECTION_H "[0.0.3@CUniformStructCollection.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of common structures handlers 
*		for in-shader structs of uniform values.
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
#include "CUniformMatrix.h"
#include "CUniformVec3.h"
#include "CUniformNumber.h"
#include "assets\textures\CTexture.h"
#include "assets\textures\CUniformTexture.h"
#include "assets\shader\CShader.h"
//DEBUG
#ifdef DEBUG_UNIFORMSTRUCTCOLLECTION
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/* Plain-old-data material structure.
*  Struct definition: MaterialPOD
*/
struct MaterialPOD {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	MaterialPOD(glm::vec3 _ambient, glm::vec3 _diffuse, 
				glm::vec3 _specular, float _shininess ) : 
				ambient(_ambient), diffuse(_diffuse),
				specular(_specular), shininess(_shininess) {}

	MaterialPOD(const MaterialPOD& other) : 
				ambient(other.ambient), diffuse(other.diffuse),
				specular(other.specular), shininess(other.shininess) {}
	
	enum Data : int {
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		SHININESS
	};
};

/* The manual storage for in-shader struct that represents material.
*  Struct definition: MaterialManualStorage
*/
struct MaterialManualStorage : public StructManualContainer {
	MaterialManualStorage(	glm::vec3 ambient, glm::vec3 diffuse, 
							glm::vec3 specular, float shininess, 
							Shader* _shader, std::string _structName) 
	{
		structName = std::move(_structName);
		Vec3ManualStorage<> _ambient(&ambient, _shader, structName.substr().append(".ambient"));
		newElement<Vec3ManualStorage<>>(_ambient);

		Vec3ManualStorage<> _diffuse(&diffuse, _shader, structName.substr().append(".diffuse"));
		newElement<Vec3ManualStorage<>>(_diffuse);

		Vec3ManualStorage<> _specular(&specular, _shader, structName.substr().append(".specular"));
		newElement<Vec3ManualStorage<>>(_specular);

		NumberManualStorage<> _shininess(&shininess, _shader, structName.substr().append(".shininess"));
		newElement<NumberManualStorage<>>(_shininess);
	}

	MaterialManualStorage(	MaterialPOD _material, Shader* _shader, std::string _structName) :	
							MaterialManualStorage(	_material.ambient, _material.diffuse, 
													_material.specular, _material.shininess,
													_shader, _structName) {}

	MaterialManualStorage() : MaterialManualStorage(glm::vec3(), glm::vec3(), glm::vec3(), 0, nullptr, "material") {}

	MaterialManualStorage(const MaterialManualStorage& other) {
		structName = other.structName.substr();
		UniformManualInteface* _buff = nullptr;
		_buff = other.data[0];
		newElement<Vec3ManualStorage<>>(dynamic_cast<Vec3ManualStorage<>*>(_buff));
		_buff = other.data[1];
		newElement<Vec3ManualStorage<>>(dynamic_cast<Vec3ManualStorage<>*>(_buff));
		_buff = other.data[2];
		newElement<Vec3ManualStorage<>>(dynamic_cast<Vec3ManualStorage<>*>(_buff));
		_buff = other.data[3];
		newElement<NumberManualStorage<>>(dynamic_cast<NumberManualStorage<>*>(_buff));
	}

	MaterialManualStorage& operator=(MaterialManualStorage other) {
		if (&other == this)
			return *this;
		StructManualContainer::operator=(other);
		return *this;
	}

	void operator() (MaterialPOD _material, Shader* _shader, std::string _structName) {
		structName = std::move(_structName);
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_material.ambient);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setShader(_shader);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setName(structName.substr().append(".ambient"));
		_buff = data[1];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_material.diffuse);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setShader(_shader);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setName(structName.substr().append(".diffuse"));
		_buff = data[2];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_material.specular);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setShader(_shader);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setName(structName.substr().append(".specular"));
		_buff = data[3];
		dynamic_cast<NumberManualStorage<>*>(_buff)->setValue(_material.shininess);
		dynamic_cast<NumberManualStorage<>*>(_buff)->setShader(_shader);
		dynamic_cast<NumberManualStorage<>*>(_buff)->setName(structName.substr().append(".shininess"));
	}

	void operator() (MaterialPOD _material) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_material.ambient);
		_buff = data[1];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_material.diffuse);
		_buff = data[2];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_material.specular);
		_buff = data[3];
		dynamic_cast<NumberManualStorage<>*>(_buff)->setValue(_material.shininess);
	}

	void operator() (Shader* _shader) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setShader(_shader);
		_buff = data[3];
		dynamic_cast<NumberManualStorage<>*>(_buff)->setShader(_shader);
	}

	//Set values of glm::vec3 type
	void operator() (glm::vec3 _vector, MaterialPOD::Data _index) {
		UniformManualInteface* _buff = nullptr;
		switch (_index) {
			case MaterialPOD::AMBIENT:
				_buff = data[0];
				break;
			case MaterialPOD::DIFFUSE:
				_buff = data[1];
				break;
			case MaterialPOD::SPECULAR:
				_buff = data[2];
				break;
		}
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_vector);
	}

	//Set values of float type
	void operator() (float _value, MaterialPOD::Data _index) {
		UniformManualInteface* _buff = nullptr;
		if (_index == MaterialPOD::SHININESS) {
			_buff = data[3];
			dynamic_cast<NumberManualStorage<>*>(_buff)->setValue(_value);
		}
	}

	//Get values of any type (for float use "new float" as argument)
	template < class T >
	T operator() (MaterialPOD::Data _index, T &_typeValue) {
		UniformManualInteface* _buff = nullptr;
		if (std::is_same<T, glm::vec3>::value) {
			switch (_index) {
				case MaterialPOD::AMBIENT:
					_buff = data[0];
					break;
				case MaterialPOD::DIFFUSE:
					_buff = data[1];
					break;
				case MaterialPOD::SPECULAR:
					_buff = data[2];
					break;
			}
			return dynamic_cast<Vec3ManualStorage<>*>(_buff)->getValue();
		} else if (std::is_same<T, float>::value) {
			_buff = data[3];
			return dynamic_cast<NumberManualStorage<>*>(_buff)->getValue();
		} else {
			return _typeValue;
		}
	}
};

/* Plain-old-data material structure with texture maps.
*  Struct definition: TextureMaterialPOD
*/
struct TextureMaterialPOD {
	std::string diffuseMap;
	std::string specularMap;
	float shininess;

	/*
	* _diffuseMap : path to diffuse texture
	* _specularMap : path to specular texture
	* _shininess : the shininess amount of object
	*/
	TextureMaterialPOD(	std::string &_diffuseMap, 
						std::string &_specularMap,
						float _shininess) :
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
						float _shininess) :
						diffuseMap(_diffuseMap),
						specularMap(_specularMap),
						shininess(_shininess) {}


	TextureMaterialPOD(	const TextureMaterialPOD& other) :
						diffuseMap(other.diffuseMap), 
						specularMap(other.specularMap),
						shininess(other.shininess) {}

	enum Data : int {
		DIFFUSE,
		SPECULAR,
		SHININESS
	};
};

/* The manual storage for in-shader struct that represents material by texture maps (diffuse and specular).
*  Struct template definition: TextureMaterialManualStorage
*/
template < class TTexture = Texture >
struct TextureMaterialManualStorage : public StructManualContainer {
private:
	typedef TextureManualStorage<TTexture> TextureHandler;
public:
	TextureMaterialManualStorage(	TTexture* diffuseMap,
									TTexture* specularMap, 
									float shininess, 
									Shader* _shader, 
									std::string _structName) 
	{
		structName = std::move(_structName);

		TextureHandler _diffuse(diffuseMap, _shader, 0, 0, structName.substr().append(".diffuse"));
		newElement<TextureHandler>(_diffuse);
		dynamic_cast<TextureHandler*>(data[0])->LoadTexture();

		TextureHandler _specular(specularMap, _shader, 0, 0, structName.substr().append(".specular"));
		newElement<TextureHandler>(_specular);
		dynamic_cast<TextureHandler*>(data[1])->LoadTexture();

		NumberManualStorage<> _shininess(&shininess, _shader, structName.substr().append(".shininess"));
		newElement<NumberManualStorage<>>(_shininess);
	}

	TextureMaterialManualStorage(	std::string diffuseMap,
									std::string specularMap,
									float shininess,
									Shader* _shader,
									std::string _structName) :
									TextureMaterialManualStorage(&TTexture(diffuseMap), &TTexture(specularMap), shininess, _shader, _structName) {}

	TextureMaterialManualStorage(	TextureMaterialPOD _material, Shader* _shader, std::string _structName) :
									TextureMaterialManualStorage( _material.diffuseMap, _material.specularMap, _material.shininess,	_shader, _structName) {}

	TextureMaterialManualStorage() : TextureMaterialManualStorage(nullptr, nullptr, 0, nullptr, "material") {}

	TextureMaterialManualStorage(const TextureMaterialManualStorage& other) {
		structName = other.structName.substr();
		UniformManualInteface* _buff = nullptr;
		_buff = other.data[0];
		newElement<TextureHandler>(dynamic_cast<TextureHandler*>(_buff));
		dynamic_cast<TextureHandler*>(data[0])->LoadTexture();
		_buff = other.data[1];
		newElement<TextureHandler>(dynamic_cast<TextureHandler*>(_buff));
		dynamic_cast<TextureHandler*>(data[1])->LoadTexture();
		_buff = other.data[2];
		newElement<NumberManualStorage<>>(dynamic_cast<NumberManualStorage<>*>(_buff));
	}

	MaterialManualStorage& operator=(MaterialManualStorage other) {
		if (&other == this)
			return *this;
		StructManualContainer::operator=(other);
		dynamic_cast<TextureHandler*>(data[0])->LoadTexture();
		dynamic_cast<TextureHandler*>(data[1])->LoadTexture();
		return *this;
	}

	void operator() (TextureMaterialPOD _material, Shader* _shader, std::string _structName) {
		structName = std::move(_structName);
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TextureHandler*>(_buff)->setValue(TTexture(_material.diffuseMap));
		dynamic_cast<TextureHandler*>(_buff)->setShader(_shader);
		dynamic_cast<TextureHandler*>(_buff)->setName(structName.substr().append(".diffuse"));
		dynamic_cast<TextureHandler*>(data[0])->LoadTexture();
		_buff = data[1];
		dynamic_cast<TextureHandler*>(_buff)->setValue(TTexture(_material.specularMap));
		dynamic_cast<TextureHandler*>(_buff)->setShader(_shader);
		dynamic_cast<TextureHandler*>(_buff)->setName(structName.substr().append(".specular"));
		dynamic_cast<TextureHandler*>(data[1])->LoadTexture();
		_buff = data[2];
		dynamic_cast<NumberManualStorage<>*>(_buff)->setValue(_material.shininess);
		dynamic_cast<NumberManualStorage<>*>(_buff)->setShader(_shader);
		dynamic_cast<NumberManualStorage<>*>(_buff)->setName(structName.substr().append(".shininess"));
	}

	void operator() (TextureMaterialPOD _material) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TextureHandler*>(_buff)->setValue(_material.diffuseMap);
		dynamic_cast<TextureHandler*>(data[0])->LoadTexture();
		_buff = data[1];
		dynamic_cast<TextureHandler*>(_buff)->setValue(_material.specularMap);
		dynamic_cast<TextureHandler*>(data[1])->LoadTexture();
		_buff = data[2];
		dynamic_cast<NumberManualStorage<>*>(_buff)->setValue(_material.shininess);
	}

	void operator() (Shader* _shader) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TextureHandler*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TextureHandler*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<NumberManualStorage<>*>(_buff)->setShader(_shader);
	}

	//Set values of TTexture type
	void operator() (TTexture& _texture, TextureMaterialPOD::Data _index) {
		UniformManualInteface* _buff = nullptr;
		switch (_index) {
			case TextureMaterialPOD::DIFFUSE:
				_buff = data[0];
				dynamic_cast<TextureHandler*>(_buff)->setValue(_texture);
				dynamic_cast<TextureHandler*>(data[0])->LoadTexture();
				break;
			case TextureMaterialPOD::SPECULAR:
				_buff = data[1];
				dynamic_cast<TextureHandler*>(_buff)->setValue(_texture);
				dynamic_cast<TextureHandler*>(data[1])->LoadTexture();
				break;
		}
	}

	//Set values of float type
	void operator() (float _value, TextureMaterialPOD::Data _index) {
		UniformManualInteface* _buff = nullptr;
		if (_index == MaterialPOD::SHININESS) {
			_buff = data[2];
			dynamic_cast<NumberManualStorage<>*>(_buff)->setValue(_value);
		}
	}

	//Get values of any type (for float use "new float" as argument)
	template < class T >
	T operator() (TextureMaterialPOD::Data _index, T &_typeValue) {
		UniformManualInteface* _buff = nullptr;
		if (std::is_same<T, TTexture>::value) {
			switch (_index) {
				case TextureMaterialPOD::DIFFUSE:
					_buff = data[0];
					break;
				case TextureMaterialPOD::SPECULAR:
					_buff = data[1];
					break;
			}
			return dynamic_cast<TextureHandler*>(_buff)->getValue();
		} else if (std::is_same<T, float>::value) {
			_buff = data[2];
			return dynamic_cast<NumberManualStorage<>*>(_buff)->getValue();
		} else {
			return _typeValue;
		}
	}

	void setTextureSlot(GLuint newSlot, TextureMaterialPOD::Data _index) {
		UniformManualInteface* _buff = nullptr;
		switch (_index) {
			case TextureMaterialPOD::DIFFUSE:
				_buff = data[0];
				break;
			case TextureMaterialPOD::SPECULAR:
				_buff = data[1];
				break;
		}
		dynamic_cast<TextureHandler*>(_buff)->setTextureSlot(newSlot);
	}

	void setTextureUnit(int newUnit, TextureMaterialPOD::Data _index) {
		UniformManualInteface* _buff = nullptr;
		switch (_index) {
			case TextureMaterialPOD::DIFFUSE:
				_buff = data[0];
				break;
			case TextureMaterialPOD::SPECULAR:
				_buff = data[1];
				break;
		}
		dynamic_cast<TextureHandler*>(_buff)->setTextureUnit(newUnit);
	}
};

/* Plain-old-data lightsource properties structure.
*  Struct definition: LightPOD
*/
struct LightsourcePOD {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	LightsourcePOD(	glm::vec3 _position, glm::vec3 _ambient,
					glm::vec3 _diffuse,	glm::vec3 _specular ) : 
					ambient(_ambient), diffuse(_diffuse),
					specular(_specular), position(_position) {}

	LightsourcePOD(	const LightsourcePOD& other) :
					ambient(other.ambient), diffuse(other.diffuse),
					specular(other.specular), position(other.position) {}
	
	enum Data : int {
		POSITION,
		AMBIENT,
		DIFFUSE,
		SPECULAR
	};
};

/* The storage for in-shader struct that represents lightsource.
*  Struct template definition: LightsourceStorage
*/
template < class TBase, class TMember, class TMemberInterface, class TVector3 = glm::vec3, class TShader = Shader>
struct LightsourceStorage : public TBase {

	LightsourceStorage(	TVector3 position, TVector3 ambient,
						TVector3 diffuse, TVector3 specular,
						TShader* _shader, std::string _structName)
	{
		structName = std::move(_structName);
		TMember _position(&position, _shader, structName.substr().append(".position"));
		newElement<TMember>(_position);

		TMember _ambient(&ambient, _shader, structName.substr().append(".ambient"));
		newElement<TMember>(_ambient);

		TMember _diffuse(&diffuse, _shader, structName.substr().append(".diffuse"));
		newElement<TMember>(_diffuse);

		TMember _specular(&specular, _shader, structName.substr().append(".specular"));
		newElement<TMember>(_specular);
	}

	LightsourceStorage(	LightsourcePOD _light, TShader* _shader, std::string _structName) :
						LightsourceStorage(	_light.position, _light.ambient,
											_light.diffuse, _light.specular,
											_shader, _structName) {}

	LightsourceStorage() : LightsourceStorage(TVector3(), TVector3(), TVector3(), TVector3(), nullptr, "light") {}

	LightsourceStorage(const LightsourceStorage& other) {
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

	LightsourceStorage& operator=(LightsourceStorage other) {
		if (&other == this)
			return *this;
		TBase::operator=(other);
		return *this;
	}

	void operator() (LightsourcePOD _light, TShader* _shader, std::string _structName) {
		structName = std::move(_structName);
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMember*>(_buff)->setValue(_light.position);
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(".position"));
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
		_buff = data[1];
		dynamic_cast<TMember*>(_buff)->setValue(_light.ambient);
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(".ambient"));
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
		_buff = data[2];
		dynamic_cast<TMember*>(_buff)->setValue(_light.diffuse);
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(".diffuse"));
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
		_buff = data[3];
		dynamic_cast<TMember*>(_buff)->setValue(_light.specular);
		dynamic_cast<TMember*>(_buff)->setName(structName.substr().append(".specular"));
		dynamic_cast<TMember*>(_buff)->setShader(_shader);
	}

	void operator() (LightsourcePOD _light) {
		TMemberInterface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<TMember*>(_buff)->setValue(_light.position);
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

	void operator() (TVector3 _vector, int _index = 0) {
		TMemberInterface* _buff = nullptr;
		try {
			_buff = data.at(_index);
		}
		catch (std::out_of_range e) {
			#ifdef DEBUG_UNIFORMSTRUCTCOLLECTION
				DEBUG_OUT << "ERROR::LIGHTSOURCE_STORAGE::operator()::OUT_OF_RANGE" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tIn index: " << _index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		catch (...) {
			#ifdef DEBUG_UNIFORMSTRUCTCOLLECTION
				DEBUG_OUT << "ERROR::LIGHTSOURCE_STORAGE::operator()::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
			throw;
		}
		dynamic_cast<TMember*>(_buff)->setValue(_vector);
	}

	void operator() (TVector3 _vector, LightsourcePOD::Data _index) {
		TMemberInterface* _buff = nullptr;
		switch (_index) {
			case LightsourcePOD::POSITION:
				_buff = data[0];
				break;
			case LightsourcePOD::AMBIENT:
				_buff = data[1];
				break;
			case LightsourcePOD::DIFFUSE:
				_buff = data[2];
				break;
			case LightsourcePOD::SPECULAR:
				_buff = data[3];
				break;
		}
		dynamic_cast<TMember*>(_buff)->setValue(_vector);
	}

	TVector3 operator()(LightsourcePOD::Data _index, TVector3& _typeValue = glm::vec3()) {
		TMemberInterface* _buff = nullptr;
		switch (_index) {
			case LightsourcePOD::POSITION:
				_buff = data[0];
				break;
			case LightsourcePOD::AMBIENT:
				_buff = data[1];
				break;
			case LightsourcePOD::DIFFUSE:
				_buff = data[2];
				break;
			case LightsourcePOD::SPECULAR:
				_buff = data[3];
				break;
			default:
				return _typeValue;
		}
		return dynamic_cast<TMember*>(_buff)->getValue();
	}

	typedef TBase Base;
	typedef TMember Member;
	typedef TMemberInterface MemberInterface;
};

/* The manual storage for in-shader struct that represents material.
*  Struct template definition: LightsourceManualStorage
*/
template < class TVector3 = glm::vec3, class TShader = Shader >
using LightsourceManualStorage	=	LightsourceStorage<	StructManualContainer, 
														Vec3ManualStorage<TVector3, TShader>, 
														UniformManualInteface, 
														TVector3, 
														TShader >;

/* The automatic storage for in-shader struct that represents material.
*  Struct template definition: LightsourceAutomaticStorage
*/
template < class TVector3 = glm::vec3, class TShader = Shader >
using LightsourceAutomaticStorage = LightsourceStorage<	StructAutomaticContainer, 
														Vec3AutomaticStorage<TVector3, TShader>, 
														UniformAutomaticInteface, 
														TVector3, 
														TShader >;
#endif