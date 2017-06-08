#ifdef OLD_H

struct InstanceData : public CommonInstance {
	TextureMaterialManualStorage<> material;

	InstanceData() : CommonInstance(), material() {}

	InstanceData(	our::mat4 _matrix, TextureMaterialPOD<> _material,  Shader *_shader, 
		const char* _matName, const char* _structName) : 
		CommonInstance(_matrix, _shader, _matName),
		material(_material, _shader, _structName) {}

	InstanceData(const InstanceData& other) : CommonInstance(other), material(other.material) {}

	InstanceData(InstanceData&& other) : CommonInstance(std::move(other)), material(std::move(other.material)) {}

	void bindData() { 
		modelMatrix.bindData(); 
		material.bindData();
	}

	void setShader(Shader *_shader) { 
		material(_shader);
		modelMatrix.setShader(_shader); 
	}

	void operator() (	our::mat4 _matrix, TextureMaterialPOD<> _material, Shader *_shader, 
		const char* _matName, const char* _structName) 
	{
		CommonInstance::operator()(_matrix, _shader, _matName);
		material(_material, _shader, _structName);
	}

	void operator() (TextureMaterialPOD<> _material) { material(_material); }
};


struct LightsourceData : public CommonInstance {
	LightsourceAutomaticStorage<> lightsource;

	LightsourceData() : CommonInstance(), lightsource() {}

	LightsourceData(our::mat4 _matrix, LightsourcePOD _light, Shader *_shader,
					const char* _matName, const char* _structName) : 
					CommonInstance(_matrix, _shader, _matName),
					lightsource(_light, _shader, _structName) {}

	LightsourceData(const LightsourceData& other) : CommonInstance(other), lightsource(other.lightsource) {}

	void bindData() { modelMatrix.bindData(); }
	
	void setShader(Shader *_shader) {
		lightsource(_shader);
		modelMatrix.setShader(_shader);
	}

	void operator() (	our::mat4 _matrix, LightsourcePOD _light, Shader *_shader, 
						const char* _matName, const char* _structName) 
	{
		CommonInstance::operator()(_matrix, _shader, _matName);
		lightsource(_light, _shader, _structName);
	}

	void operator() (LightsourcePOD _light) { lightsource(_light); }

	void push(Shader* _shader) {
		LightsourceAutomaticStorage<>::MemberInterface* _buff;
		for (auto &v : lightsource.data) {
			_buff = v;
			dynamic_cast<LightsourceAutomaticStorage<>::Member*>(_buff)->push(_shader);
		}
	}
};
#endif