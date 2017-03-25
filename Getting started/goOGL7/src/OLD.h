#ifdef OLD_H
template < class TUniformLoader >
class MultipleInstanceDataContainer : public std::vector<TUniformLoader> {
public:
	MultipleInstanceDataContainer();
	~MultipleInstanceDataContainer();

	void genData(int count = 1) {

	}

	void pushData(TUniformLoader _loader) {

	}

	void popData() {

	}

	void setData(TUniformLoader _loader, int _index = 0) {

	}

	//Remove data from start index to end index
	void eraseData(int start = 0, int end = 0) {
		auto _start = data.begin(), _end = data.begin();
		_start += start;
		_end += end;
		data.erase(_start, _end);
	}
};

//Helper function for glm::mat4 matrix binding to OpenGL uniform Mat4
static void stdBindMatrix(GLint location, glm::mat4 &m) {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

//Class template definition: InstanceHandler
template <	class Matrix = glm::mat4, class TShader = Shader,
			GLint (TShader::* _getUniformLocation)(const char*) = &TShader::getUniformLocation>
class MultipleInstanceLoader {
	//Transform matrix array
	std::vector<Matrix> transformation_data;
protected:
	//Load instance data
	void bindInstance(int index, TShader *shader, void(*BindMatrix)(GLint, Matrix&) = &stdBindMatrix) {
		if (transformation_data.size() > 0) {
			GLint _location = (shader->*_getUniformLocation)(SIMPLEMODEL_TRANSFORM_MATRIX_NAME);
				if (_location == -1) { //Check if uniform not found
					#ifdef GEBUG_SIMPLEMODEL
						DEBUG_OUT << "ERROR::INSTANCE_HANDLER::bindInstance::UNIFORM_NAME_MISSING" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tName: " << SIMPLEMODEL_TRANSFORM_MATRIX_NAME << DEBUG_NEXT_LINE;
					#endif
					return;
				}
			try {
				(*BindMatrix)(_location, transformation_data.at(index));
			}
			catch (std::out_of_range e) {
				#ifdef GEBUG_SIMPLEMODEL
					DEBUG_OUT << "ERROR::SEPARATE_MODEL::drawInstance::OUT_OF_RANGE\n\tIn index: " << index << DEBUG_NEXT_LINE;
					DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
				#endif
				return;
			}
		}
	}
public:
	//Draw one instance of model using instance data
	virtual void drawInstance(int index) { return; }

	//Draw multiple instances of model using their transform matrices
	virtual void drawInstances(int start_index = 0, int count = 1) {
		if (count < 1) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::drawInstances::INVALID_COUNT: " << count << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		for (int _index = start_index; _index < start_index + count; _index++)
			this->drawInstance(_index);
	}

	//Generate place for "count" instances of model
	void genInstances(int count = 1) {
		if (count > 0) {
			for (int _index = 0; _index < count; _index++) {
				try {
					pushTransformation(Matrix());
				}
				catch (...) {
					throw;
				}
			}
		} else {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::genInstances::INVALID_COUNT" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't generate count: " << count << " instances." << DEBUG_NEXT_LINE;
			#endif
		}
	}

	//Push transformation to transformation array
	void pushTransformation(Matrix transform) {
		try {
			transformation_data.push_back(std::move(transform));
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::pushTransformation::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't push more transformations " << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::pushTransformations::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Pop last transformation from transformation array
	void popTransformation() {
		if (transformation_data.empty()) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::popTransformation::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't pop from empty transform array." << DEBUG_NEXT_LINE;
			#endif
		} else {
			transformation_data.pop_back();
		}
	}

	//Setup transformation for "index" model instance
	void setTransformation(Matrix transform, int index = 0) {
		try {
			transformation_data.at(index) = std::move(transform);
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::setTransformation::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tCan't access transformation by index: " << index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::setTransformation::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Remove transformations from start index to end index
	void eraseTransformations(int start = 0, int end = 0) {
		auto _start = transformation_data.begin(), _end = transformation_data.begin();
		_start += start;
		_end += end;
		transformation_data.erase(_start, _end);
	}

	//Return curent helded instances count
	int instanceCount() { return transformation_data.size(); }
};

/*
//Class template definition: InstanceHandler
class InstanceHandler {
	//Transform matrix array
	std::vector<MatrixHandler> transformation_data;
protected:
	//Load instance data
	void bindInstance(int index) {
		if (transformation_data.size() > 0) {
			try {
				
			}
			catch (std::out_of_range e) {
				#ifdef GEBUG_SIMPLEMODEL
					DEBUG_OUT << "ERROR::SEPARATE_MODEL::drawInstance::OUT_OF_RANGE\n\tIn index: " << index << DEBUG_NEXT_LINE;
					DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
				#endif
				return;
			}
		}
	}
public:
	//Draw one instance of model using instance data
	virtual void drawInstance(int index) { return; }

	//Draw multiple instances of model using their transform matrices
	virtual void drawInstances(int start_index = 0, int count = 1) {
		if (count < 1) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::drawInstances::INVALID_COUNT: " << count << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		for (int _index = start_index; _index < start_index + count; _index++)
			this->drawInstance(_index);
	}

	//Generate place for "count" instances of model
	void genInstances(Shader* _shader, int count = 1) {
		if (count > 0) {
			for (int _index = 0; _index < count; _index++) {
				try {
					pushTransformation(MatrixHandler(glm::mat4(), _shader, SIMPLEMODEL_TRANSFORM_MATRIX_NAME));
				}
				catch (...) {
					throw;
				}
			}

		} else {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::genInstances::INVALID_COUNT" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't generate count: " << count << " instances." << DEBUG_NEXT_LINE;
			#endif
		}
	}

	//Push transformation to transformation array
	void pushTransformation(MatrixHandler transform) {
		try {
			transformation_data.push_back(std::move(transform));
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::pushTransformation::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't push more transformations " << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::pushTransformations::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Push transformation to transformation array
	void pushTransformation(glm::mat4 _matrix, Shader* _shader) {
		try {
			pushTransformation(MatrixHandler(_matrix, _shader, SIMPLEMODEL_TRANSFORM_MATRIX_NAME));
		}
		catch (...) {
			throw;
		}
	}

	//Pop last transformation from transformation array
	void popTransformation() {
		if (transformation_data.empty()) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::popTransformation::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't pop from empty transform array." << DEBUG_NEXT_LINE;
			#endif
		} else {
			transformation_data.pop_back();
		}
	}

	//Setup transformation for "index" model instance
	void setTransformation(MatrixHandler transform, int index = 0) {
		try {
			transformation_data.at(index) = std::move(transform);
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::setTransformation::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tCan't access transformation by index: " << index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::setTransformation::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Setup transformation for "index" model instance
	void setTransformation(glm::mat4 _matrix, Shader* _shader, int _index = 0) {
		try {
			setTransformation(MatrixHandler(_matrix, _shader, SIMPLEMODEL_TRANSFORM_MATRIX_NAME), _index);
		}
		catch (...) {
			throw;
		}
	}

	//Remove transformations from start index to end index
	void eraseTransformations(int start = 0, int end = 0) {
		auto _start = transformation_data.begin(), _end = transformation_data.begin();
		_start += start;
		_end += end;
		transformation_data.erase(_start, _end);
	}

	//Return curent helded instances count
	int instanceCount() { return transformation_data.size(); }
};
*/

/*
//Class template definition: TextureHandler
template <	class TTexture = Texture, class TShader = Shader,
			void (TTexture::* TextureLoader)(void)	= &TTexture::LoadToGL,
			void (TTexture::* BindTexture)(void)	= &TTexture::Use,
			void (TShader::* BindShader)(void)		= &TShader::Use,
			void (TShader::* ReloadShader)(void)	= &TShader::Reload,
			GLuint (TShader::* GetShaderID)(void)	= &TShader::getShaderId>
class TextureHandler {
	//Texture array, max count: 32
	std::vector<TTexture> textures;
protected:
	//Pointer to shader
	TShader *shader;

	//Returns shader object id
	GLuint getShaderId() { return (shader->*GetShaderID)(); }

	//Internal funcion for shader applying
	void useShader() {
		(shader->*BindShader)();
		int _size = textures.size();
		GLint _location = 0;
		for (int _index = 0; _index < _size; _index++) {
			(textures[_index].*BindTexture)();
			_location = glGetUniformLocation((shader->*GetShaderID)(), SIMPLEMODEL_TEXTURE_NAMES[_index]);
			if (_location == -1) {
				#ifdef GEBUG_SIMPLEMODEL
					DEBUG_OUT << "ERROR::SIMPLE_MODEL::useShader::TEXTURE_NAME_MISSING\n\tName: " << SIMPLEMODEL_TEXTURE_NAMES[_index] << DEBUG_NEXT_LINE;
				#endif
				continue;
			}
			glUniform1i(_location, _index);
		}
	}
public:

	TextureHandler() : shader(nullptr) {}
	
	~TextureHandler() { shader = nullptr; }

	//Setup shader
	void setShader(TShader *s) { shader = s; }

	//Reload shader from disk
	void reloadShader() { (shader->*ReloadShader)(); }

	//Push texture to texture stack
	void pushTexture(TTexture &t) {
		if (textures.size() == 32)
			popTexture();
		try {
			textures.push_back(std::move(t));
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::pushTexture::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't push more textures" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::pushTexture::UNKONWN" << DEBUG_NEXT_LINE;
			#endif
		}
		//Setup texture slot
		textures.back().TextureSlot = GL_TEXTURE0 + (GLuint)(textures.size() - 1);
	}

	//Pop texture from texture stack
	void popTexture() {
		if (textures.empty()) {
			#ifdef GEBUG_SIMPLEMODEL
			DEBUG_OUT << "ERROR::TEXTURE_HANDLER::popTexture::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
			DEBUG_OUT << "\tCan't pop from empty texture stack." << DEBUG_NEXT_LINE;
			#endif
		} else {
			textures.back().TextureSlot = GL_TEXTURE0;
			textures.pop_back();
		}
	}

	//Load texture from disk to opengl
	void loadTexture(int index = 0) {
		try {
			(textures.at(index).*TextureLoader)();
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::loadTexture::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tCan't access texture by index: " << index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::loadTexture::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Load textures from range [start,end] from disk to opengl
	void loadTextures(int start = 0, int end = 0) {
		for (int _index = start; _index <= end; _index++)
			loadTexture(_index);
	}

	//Load all textures from disk to opengl
	void loadAllTextures() {
		loadTextures(0, textures.size() - 1);
	}

	//Return curent textures count
	int textureCount() { return textures.size(); }
};

*/

/*
//Bind shader program to OpenGL and update all uniforms
void Shader::Use() {
	glUseProgram(this->Program);
	GLint _location = 0;
	for (auto &_value : uniforms) {
		_location = glGetUniformLocation(Program, std::get<0>(_value).c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef GEBUG_SHADER
				DEBUG_OUT << "ERROR::SHADER::Use::UNIFORM_NAME_MISSING"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << std::get<0>(_value) << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tUniform type: " << std::get<2>(_value) << DEBUG_NEXT_LINE;
			#endif
			continue;
		}
		switch (std::get<2>(_value)) {
		case UNIFORMMATRIX4FV:
			glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(*((glm::mat4 *)std::get<1>(_value))));
			break;
		default:
			break;
		}
	}
}*/

	/*
public:
	enum uniformTypes : int {
		UNIFORM1F,
		UNIFORM1FV,
		UNIFORM1I,
		UNIFORM1IV,
		UNIFORM1UI,
		UNIFORM1UIV,
		UNIFORM2F,
		UNIFORM2FV,
		UNIFORM2I,
		UNIFORM2IV,
		UNIFORM2UI,
		UNIFORM2UIV,
		UNIFORM3F,
		UNIFORM3FV,
		UNIFORM3I,
		UNIFORM3IV,
		UNIFORM3UI,
		UNIFORM3UIV,
		UNIFORM4F,
		UNIFORM4FV,
		UNIFORM4I,
		UNIFORM4IV,
		UNIFORM4UI,
		UNIFORM4UIV,
		UNIFORMMATRIX2FV,
		UNIFORMMATRIX2X3FV,
		UNIFORMMATRIX2X4FV,
		UNIFORMMATRIX3FV,
		UNIFORMMATRIX3X2FV,
		UNIFORMMATRIX3X4FV,
		UNIFORMMATRIX4FV,
		UNIFORMMATRIX4X2FV,
		UNIFORMMATRIX4X3FV
	};
private:
*/
#endif