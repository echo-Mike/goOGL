#ifdef OLD_H
	/* The automatic storage for uniform in-shader value.
*  Class template definition: UniformAutomaticStorage
*/
template <	class T, class TShader = Shader,
			int (TShader::* NewUniform)(const char*, UniformAutomaticInteface*) = &TShader::newUniform,
			void (TShader::* DeleteUniform)(int) = &TShader::deleteUniform>
class UniformAutomaticStorage : public UniformAutomaticInteface, public UniformStorage<T, TShader> {
	int uniformId;
public:
	//Push to shader uniform handle queue of current saved shader
	void push() {
		if (uniformId >= 0) {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "WARNING::UNIFORM_HANDLER::push::ALREADY_BINDED" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: Uniform handler alredy helds binding with other shader." << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tLast Id: " << uniformId << DEBUG_NEXT_LINE;
			#endif	
		}
		if (shader) {
			uniformId = (shader->*NewUniform)(uniformName.c_str(), this);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::push::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	/*Push to uniform handle queue of shader defined by pointer
	* Returns uniforId from _shader
	*/
	int push(TShader *_shader) {
		if (_shader) {
			return (_shader->*NewUniform)(uniformName.c_str(), this);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::push::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif
			return -1;
		}
	}

	//Pull from shader uniform handle queue of current saved shader
	void pull() {
		if (shader) {
			(shader->*DeleteUniform)(uniformId);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::pull::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	UniformAutomaticStorage() : UniformStorage(), uniformId(-1) {}

	UniformAutomaticStorage(T &_value, TShader *_shader = nullptr,
							std::string _uniformName = std::string(UNIFORM_STD_SHADER_VARIABLE_NAME)) :
							UniformStorage(_value, _shader, _uniformName), uniformId(-1)
	{
		push();
	}
	
	UniformAutomaticStorage(T &_value, TShader *_shader = nullptr,
							const char* _uniformName = UNIFORM_STD_SHADER_VARIABLE_NAME) :
							UniformStorage(_value, _shader, _uniformName), uniformId(-1)
	{
		push();
	}

	UniformAutomaticStorage(const UniformAutomaticStorage &other) : UniformStorage(other), uniformId(-1)
	{
		push();
	}

	UniformAutomaticStorage(UniformAutomaticStorage &&other) :	UniformStorage(other),
																uniformId(std::move(other.uniformId)) 
	{
		//Negate the shader resource acquisition in "other" that will be deleted soon
		other.uniformId = -1;
	}

	~UniformAutomaticStorage() {
		//Clear shader callback for this uniform
		if (uniformId >= 0)
			pull();
	}

	UniformAutomaticStorage& operator=(UniformAutomaticStorage other) {
		if (&other == this)
			return *this;
		UniformStorage::operator=(other);
		uniformId = -1;
		std::swap(uniformId, other.uniformId);
		return *this;
	}

	UniformAutomaticStorage& operator=(UniformAutomaticStorage &&other) {
		UniformStorage::operator=(other);
		uniformId = std::move(other.uniformId);
		other.uniformId = -1;
		return *this;
	}
};

/* The automatic observer for uniform in-shader value.
*  Class template definition: UniformAutomaticObserver
*/
template <	class T, class TShader = Shader,
			int (TShader::* NewUniform)(const char*, UniformAutomaticInteface*) = &TShader::newUniform,
			void (TShader::* DeleteUniform)(int) = &TShader::deleteUniform>
class UniformAutomaticObserver : public UniformAutomaticInteface, public UniformObserver<T, TShader> {
	int uniformId;
public:
	//Push to shader uniform handle queue of current saved shader
	void push() {
		if (uniformId >= 0) {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "WARNING::UNIFORM_HANDLER::push::ALREADY_BINDED" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: Uniform handler alredy helds binding with other shader." << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tLast Id: " << uniformId << DEBUG_NEXT_LINE;
			#endif	
		}
		if (shader) {
			uniformId = (shader->*NewUniform)(uniformName.c_str(), this);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::push::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	/*Push to uniform handle queue of shader defined by pointer
	* Returns uniforId from _shader
	*/
	int push(TShader *_shader) {
		if (_shader) {
			return (_shader->*NewUniform)(uniformName.c_str(), this);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::push::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif
			return -1;
		}
	}

	//Pull from shader uniform handle queue of current saved shader
	void pull() {
		if (shader) {
			(shader->*DeleteUniform)(uniformId);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::pull::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	UniformAutomaticObserver() : UniformObserver(), uniformId(-1) {}

	UniformAutomaticObserver(T &_value, TShader *_shader = nullptr,
							std::string _uniformName = std::string(UNIFORM_STD_SHADER_VARIABLE_NAME)) :
							UniformObserver(_value, _shader, _uniformName), uniformId(-1)
	{
		push();
	}
	
	UniformAutomaticObserver(T &_value, TShader *_shader = nullptr,
							const char* _uniformName = UNIFORM_STD_SHADER_VARIABLE_NAME) :
							UniformObserver(_value, _shader, _uniformName), uniformId(-1)
	{
		push();
	}

	UniformAutomaticObserver(const UniformAutomaticObserver &other) : UniformObserver(other), uniformId(-1)
	{
		push();
	}

	UniformAutomaticObserver(UniformAutomaticObserver &&other) : UniformObserver(other),
																uniformId(std::move(other.uniformId)) 
	{
		//Negate the shader resource acquisition in "other" that will be deleted soon
		other.uniformId = -1;
	}

	~UniformAutomaticObserver() {
		//Clear shader callback for this uniform
		if (uniformId >= 0)
			pull();
	}

	UniformAutomaticObserver& operator=(UniformAutomaticObserver other) {
		if (&other == this)
			return *this;
		UniformObserver::operator=(other);
		uniformId = -1;
		std::swap(uniformId, other.uniformId);
		return *this;
	}

	UniformAutomaticObserver& operator=(UniformAutomaticObserver &&other) {
		UniformObserver::operator=(other);
		uniformId = std::move(other.uniformId);
		other.uniformId = -1;
		return *this;
	}
};


/* The manual storage for uniform in-shader value.
*  Class template definition: UniformLoader
*/
template< class T, class TShader = Shader>
class UniformLoader : public UniformLoaderInteface, public UniformValue<T, TShader> {
public:
	UniformLoader() : UniformValue() {}

	UniformLoader(	T &_value, TShader *_shader = nullptr,
					std::string _dataName = std::string(UNIFORM_STD_SHADER_VARIABLE_NAME)) :
					UniformValue(_value, _shader, _dataName) {}

	UniformLoader(	T &_value, TShader *_shader = nullptr, 
					const char* _dataName = UNIFORM_STD_SHADER_VARIABLE_NAME) :
					UniformValue(_value, _shader, _dataName) {}
	
	UniformLoader(const UniformLoader &other) : UniformLoader(other) {}

	UniformLoader(UniformLoader &&other) : UniformLoader(other) {}

	~UniformLoader() {}
};
#endif