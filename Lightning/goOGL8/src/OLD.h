#ifdef OLD_H
	/*
	//Push uniform value to uniform loader
	int pushUniform(const char *uniformName, void *value, uniformTypes type) {
		uniforms.push_back(std::make_tuple(std::string(uniformName), value, type));
		return uniforms.size()-1;
	}

	//Push uniform value to uniform loader
	int pushUniform(std::string &uniformName, void *value, uniformTypes type) {
		uniforms.push_back(std::make_tuple(uniformName, value, type));
		return uniforms.size()-1;
	}

	//Push uniform value to uniform loader
	int pushUniform(const char *uniformName, void *value, int type) {
		return pushUniform(uniformName, value, (uniformTypes)type);
	}

	//Push uniform value to uniform loader
	int pushUniform(std::string &uniformName, void *value, int type) {
		return pushUniform(uniformName, value, (uniformTypes)type);
	}

	//Pop uniform value from uniform loader
	void popUniform() {
		if (!uniforms.empty()) {
			//Prevent external value from destructor call
			std::get<1>(uniforms.back()) = nullptr;
			uniforms.pop_back();
		}
	}

	//Set pointer to current value of value
	void setUniform(void *value, int index = 0) {
		try {
			std::get<1>(uniforms.at(index)) = value;
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SHADER
				DEBUG_OUT << "ERROR::SHADER::setUniform::OUT_OF_RANGE\n\tCan't access uniform by index: "<< index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SHADER
				DEBUG_OUT << "ERROR::SHADER::setUniform::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}
	*/
	/*Generate place for "count" instances of model
	void genInstances(int count = 1) {
		if (count > 0) {
			for (int _index = 0; _index < count; _index++) {
				try {
					pushInstance(InstanceData());
				}
				catch (...) {
					throw;
				}
			}
		} else {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::genInstances::INVALID_COUNT" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't generate count: " << count << " instances." << DEBUG_NEXT_LINE;
			#endif
		}
	}
	*/
	/*Access instance data 
	InstanceDataInterface* accessInstance(int _index = 0) {
		try {
			return instances.at(_index);
		}
		catch (std::length_error e) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::accessInstance::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tCan't access instance by index: " << _index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::accessInstance::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}*/
#endif