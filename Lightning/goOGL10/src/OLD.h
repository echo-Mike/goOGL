#ifdef OLD_H
/* The manual storage for in-shader struct that represents lightsource.
*  Struct definition: LightsourceManualStorage
*/
struct LightsourceManualStorage : public StructManualContainer {
	LightsourceManualStorage(	glm::vec3 position, glm::vec3 ambient,
								glm::vec3 diffuse,	glm::vec3 specular, 
								Shader* _shader, std::string _structName) 
	{
		structName = std::move(_structName);
		Vec3ManualStorage<> _position(&position, _shader, structName.substr().append(".position"));
		newElement<Vec3ManualStorage<>>(_position);

		Vec3ManualStorage<> _ambient(&ambient, _shader, structName.substr().append(".ambient"));
		newElement<Vec3ManualStorage<>>(_ambient);

		Vec3ManualStorage<> _diffuse(&diffuse, _shader, structName.substr().append(".diffuse"));
		newElement<Vec3ManualStorage<>>(_diffuse);

		Vec3ManualStorage<> _specular(&specular, _shader, structName.substr().append(".specular"));
		newElement<Vec3ManualStorage<>>(_specular);
	}

	LightsourceManualStorage(	LightsourcePOD _light, Shader* _shader, std::string _structName) :
								LightsourceManualStorage(	_light.position, _light.ambient,
															_light.diffuse, _light.specular, 
															_shader, _structName) {}

	LightsourceManualStorage() : LightsourceManualStorage(glm::vec3(), glm::vec3(), glm::vec3(), glm::vec3(), nullptr, "light") {}

	LightsourceManualStorage(const LightsourceManualStorage& other) {
		structName = other.structName.substr();
		UniformManualInteface* _buff = nullptr;
		_buff = other.data[0];
		newElement<Vec3ManualStorage<>>(dynamic_cast<Vec3ManualStorage<>*>(_buff));
		_buff = other.data[1];
		newElement<Vec3ManualStorage<>>(dynamic_cast<Vec3ManualStorage<>*>(_buff));
		_buff = other.data[2];
		newElement<Vec3ManualStorage<>>(dynamic_cast<Vec3ManualStorage<>*>(_buff));
		_buff = other.data[3];
		newElement<Vec3ManualStorage<>>(dynamic_cast<Vec3ManualStorage<>*>(_buff));
	}

	LightsourceManualStorage& operator=(LightsourceManualStorage other) {
		if (&other == this)
			return *this;
		StructManualContainer::operator=(other);
		return *this;
	}

	void operator() (LightsourcePOD _light, Shader* _shader, std::string _structName) {
		structName = std::move(_structName);
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_light.position);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setShader(_shader);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setName(structName.substr().append(".position"));
		_buff = data[1];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_light.ambient);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setShader(_shader);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setName(structName.substr().append(".ambient"));
		_buff = data[2];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_light.diffuse);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setShader(_shader);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setName(structName.substr().append(".diffuse"));
		_buff = data[3];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_light.specular);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setShader(_shader);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setName(structName.substr().append(".specular"));
		
	}

	void operator() (LightsourcePOD _light) {
		UniformManualInteface* _buff = nullptr;
		_buff = data[0];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_light.position);
		_buff = data[1];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_light.ambient);
		_buff = data[2];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_light.diffuse);
		_buff = data[3];
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_light.specular);
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
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setShader(_shader);
	}

	void operator() (glm::vec3 _vector, int _index = 0) {
		UniformManualInteface* _buff = nullptr;
		_buff = data.at(_index);
		dynamic_cast<Vec3ManualStorage<>*>(_buff)->setValue(_vector);
	}
};

struct LightsourceAutomaticStorage : public StructAutomaticContainer {

};
#endif