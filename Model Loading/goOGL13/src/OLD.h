#ifdef OLD_H
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