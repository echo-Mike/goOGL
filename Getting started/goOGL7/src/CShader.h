#ifndef SHADER_H
#define SHADER_H
//STD
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//std::vector
#include <vector>
//std::map
#include <map>
//std::tuple, std::make_tuple
#include <tuple>
//GLEW
//Include glew to get all the required OpenGL headers
#include <GL/glew.h>
//DEBUG
#ifdef GEBUG_SHADER
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#ifndef UNIFORM_HANDLER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define UNIFORM_HANDLER_STD_SHADER_VARIABLE_NAME "value"
#endif

//Class template definition: UniformHandler
template <	class T, class TShader,
			int (TShader::* NewUniform)(const char*, void(*)(GLuint)) = &TShader::newUniform,
			void (TShader::* DeleteUniform)(int) = &TShader::deleteUniform>
class UniformHandler {
	int uniformId;
protected:
	T value;
	TShader *shader;
	std::string uniformName;
public:
	//Bind uniform to shader
	virtual void bindUniform(GLint _location) { return; }
protected:
	void Build() {
		if (shader) {
			uniformId = (shader->*NewUniform)(uniformName.c_str(), &(this->bindUniform));
		} else {
			#ifdef GEBUG_SHADER
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::Build::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}
public:
	UniformHandler() : value(), uniformId(-1), shader(nullptr), uniformName(UNIFORM_HANDLER_STD_SHADER_VARIABLE_NAME) {}

	UniformHandler(	T _value, TShader *_shader = nullptr, 
					std::string _uniformName = std::string(UNIFORM_HANDLER_STD_SHADER_VARIABLE_NAME)) :
					value(std::move(_value)), shader(_shader), 
					uniformName(std::move(_uniformName)), uniformId(-1) 
	{
		Build();
	}

	UniformHandler(T _value, TShader *_shader, const char* _uniformName) : UniformHandler(_value, _shader, std::string(_uniformName)) { }

	~UniformHandler() {
		//Clear shader callback for this uniform
		(shader->*DeleteUniform)(uniformId);
		//Protect shader from destructor call
		shader = nullptr; 
	}

	//Set new value to handle
	void newValue(T _value) { value = std::move(_value); }
};

//Class definition: Shader

class Shader {
public:
	//Type for uniform update function
	typedef void(*const UpdateFunction)(GLuint);
private:

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
	
	//NO STDCONSTRUCT, NO MOVECONSTRUCT, NO COPYCONSTRUCT
	Shader() = delete;
	Shader(const Shader& s) = delete;
	Shader(const Shader&& s) = delete;

	//Vertex and Fragment shader paths
	std::string vpath, fpath;
	//Uniform container
	std::map<int, std::pair<std::string, UpdateFunction>> uniforms;
	// The program ID //SPO ID
	GLuint Program;
	//NOT SAFE SOLUTION: OVERFLOW
	//Id counter for current shader
	int incrementId = 0;
public:
	//Returns SPO Id
	GLuint getShaderId() { return Program; }

	//Returns result of uniform search: location detemined by name
	GLint getUniformLocation(const char* _uniformName) {
		GLint _location;
		_location = glGetUniformLocation(Program, _uniformName);
		if (_location == -1) { //Check if uniform not found
			#ifdef GEBUG_SHADER
				DEBUG_OUT << "ERROR::SHADER::getUniformLocation::UNIFORM_NAME_MISSING"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << _uniformName << DEBUG_NEXT_LINE;
			#endif
		}
		return _location;
	}

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath) : vpath(vertexPath), fpath(fragmentPath), Program(0) { Reload(); }
	
	~Shader() {
		//Prevent external values from destructor calls
		//for (auto v : uniforms)
			//v.second = nullptr;
		glDeleteProgram(this->Program); 
	}

	int newUniform(const char* _uniformName, UpdateFunction _update) {
		uniforms[incrementId] = std::move(std::make_pair(std::string(_uniformName), _update));
		return incrementId++;
	}

	void deleteUniform(int _index) {
		uniforms.erase(_index);
	}

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

	void Use();

	void Reload();

};

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

//Bind shader program to OpenGL and update all uniforms
void Shader::Use() {
	glUseProgram(this->Program);
	GLint _location = 0;
	for (auto &_value : uniforms) {
		_location = glGetUniformLocation(Program, _value.second.first.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef GEBUG_SHADER
				DEBUG_OUT << "ERROR::SHADER::Use::UNIFORM_NAME_MISSING"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << _value.first << DEBUG_NEXT_LINE;
			#endif
			continue;
		}
		(*_value.second.second)(_location);
	}
}

//Reload shader from disk (read, build and link)
void Shader::Reload() {
	if (Program)
		glDeleteProgram(Program);
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try {
		// Open files
		vShaderFile.open(vpath);
		fShaderFile.open(fpath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into GLchar array
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		#ifdef GEBUG_SHADER
			DEBUG_OUT << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << DEBUG_NEXT_LINE;
		#endif
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	// 2. Compile shaders
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];
	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// Print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		#ifdef GEBUG_SHADER
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			DEBUG_OUT << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << DEBUG_NEXT_LINE;
		#endif
	};
	// Similiar for Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		#ifdef GEBUG_SHADER
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			DEBUG_OUT << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << DEBUG_NEXT_LINE;
		#endif
	}
	// Shader Program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);
	// Print linking errors if any
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		#ifdef GEBUG_SHADER
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			DEBUG_OUT << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << DEBUG_NEXT_LINE;
		#endif
	}
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

#ifndef MATRIX_HANDLER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define MATRIX_HANDLER_STD_SHADER_VARIABLE_NAME "matrix"
#endif

//REDO: Make real tamplate class and one specialisation for glm::mat4
//Class definition: MatrixHandler
class MatrixHandler : public UniformHandler<glm::mat4, Shader> {
public:
	//NOT SAFE SOLUTION
	//Control over transpose operation on matrix loading
	GLuint transposeOnLoad = GL_FALSE;

	MatrixHandler() : UniformHandler(glm::mat4(), nullptr, MATRIX_HANDLER_STD_SHADER_VARIABLE_NAME) {}

	MatrixHandler(	glm::mat4 _matrix, Shader* _shader,
					const char* _name = MATRIX_HANDLER_STD_SHADER_VARIABLE_NAME) :
					UniformHandler(_matrix, _shader, _name) {}

	MatrixHandler(	glm::mat4 _matrix, Shader* _shader,
					std::string _name = std::string(MATRIX_HANDLER_STD_SHADER_VARIABLE_NAME)) :
					UniformHandler(_matrix, _shader, _name) {}

	void bindUniform(GLuint _location) {
		glUniformMatrix4fv(_location, 1, transposeOnLoad, glm::value_ptr(value));
	}
};

#ifndef TEXTURE_HANDLER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of texture
	#define TEXTURE_HANDLER_STD_SHADER_VARIABLE_NAME "Texture"
#endif

//Class template definition: TextureHandler
template <	class TTexture = Texture, class TShader = Shader,
			void (TTexture::* TextureLoader)(void)	= &TTexture::LoadToGL,
			void (TTexture::* BindTexture)(void)	= &TTexture::Use >
class TextureHandler : UniformHandler<TTexture, TShader> {
	GLuint textureSlot;
	int textureUnit;
public:

	TextureHandler() : UniformHandler(TTexture(), nullptr, TEXTURE_HANDLER_STD_SHADER_VARIABLE_NAME) {}

	TextureHandler(	TTexture _texture, TShader *_shader,
					GLuint _textureSlot = GL_TEXTURE0, int _textureUnit = 0,
					const char* _name = TEXTURE_HANDLER_STD_SHADER_VARIABLE_NAME) :
					UniformHandler(_texture, _shader, _name), 
					textureSlot(_textureSlot), textureUnit(_textureUnit) {}

	TextureHandler(	TTexture _texture, TShader *_shader,
					GLuint _textureSlot = GL_TEXTURE0, int _textureUnit = 0,
					std::string _name = std::string(TEXTURE_HANDLER_STD_SHADER_VARIABLE_NAME)) :
					UniformHandler(_texture, _shader, _name),
					textureSlot(_textureSlot), textureUnit(_textureUnit) {}

	//Load texture from disk to opengl
	void LoadTexture() { (value.*TextureLoader)(); }

	void bindUniform(GLuint location) {
		glActiveTexture(TextureSlot);
		(value.*BindTexture)();
		glUniform1i(location, textureUnit);
	}

	//Set texture slot of current texture handler (with bound check)
	void setTextureSlot(GLuint newSlot) {
		if (newSlot >= GL_TEXTURE0 && newSlot <= GL_TEXTURE31) {
			textureSlot = newSlot;
		} else {
			#ifdef GEBUG_SHADER
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::setTextureSlot::INVALID_TEXTURE_SLOT" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tSlot: " << newSlot << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tRange: " << GL_TEXTURE0 << " : " << GL_TEXTURE31 << DEBUG_NEXT_LINE;
			#endif
		}
	}

	//Set texture unit of current texture handler (with bound check)
	void setTextureUnit(int newUnit) {
		if (newUnit >= 0 && newUnit <= 31) {
			textureUnit = newUnit;
		} else {
			#ifdef GEBUG_SHADER
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::setTextureUnit::INVALID_TEXTURE_UNIT" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tSlot: " << newUnit << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tRange: 0 : 31" << DEBUG_NEXT_LINE;
			#endif
		}
	}
};

#ifndef MULTIPLE_TEXTURE_HANDLER_TEXTURE_NAMES
#define MULTIPLE_TEXTURE_HANDLER_TEXTURE_NAMES textureNames
	//Static array for texture slot names
	static const char* MULTIPLE_TEXTURE_HANDLER_TEXTURE_NAMES[] = {
		"texture00", "texture01", "texture02", "texture03",
		"texture04", "texture05", "texture06", "texture07",
		"texture08", "texture09", "texture10", "texture11",
		"texture12", "texture13", "texture14", "texture15",
		"texture16", "texture17", "texture18", "texture19",
		"texture20", "texture21", "texture22", "texture23",
		"texture24", "texture25", "texture26", "texture27",
		"texture28", "texture29", "texture30", "texture31"
	};
#endif 

//Class template definition: MultipleTextureHandler
template < class TTexture = Texture, class TShader = Shader >
class MultipleTextureHandler {
	//Texture array, max count: 32
	std::vector<TextureHandler<TTexture, TShader>> textures;
	//Pointer to shader
	TShader *shader;
protected:
	//Internal function for shader pointer setup
	void setShader(TShader *_shader) { shader = _shader; }
public:

	MultipleTextureHandler() : shader(nullptr) {}
	
	~MultipleTextureHandler() { shader = nullptr; }

	//Push texture to texture stack
	void pushTexture(TTexture t) {
		int _size = textures.size();
		if (_size == 32) {
			popTexture();
			size--;
		}
		try {
			textures.push_back(TextureHandler<TTexture, TShader>(t, shader, 0, 0, MULTIPLE_TEXTURE_HANDLER_TEXTURE_NAMES[_size]));
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::pushTexture::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't push more textures" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
				return;
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::pushTexture::UNKONWN" << DEBUG_NEXT_LINE;
			#endif
				return;
		}
		//Setup texture slot and unit
		textures.back().setTextureSlot( GL_TEXTURE0 + (GLuint)(textures.size() - 1));
		textures.back().setTextureUnit(textures.size() - 1);
	}

	//Pop texture from texture stack
	void popTexture() {
		if (textures.empty()) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::popTexture::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't pop from empty texture stack." << DEBUG_NEXT_LINE;
			#endif
		} else {
			textures.pop_back();
		}
	}

	//Load texture from disk to opengl
	void loadTexture(int index = 0) {
		try {
			textures.at(index).LoadTexture();
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::loadTexture::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMesseg: Can't access texture by index: " << index << DEBUG_NEXT_LINE;
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
		try {
			for (int _index = start; _index <= end; _index++)
				loadTexture(_index);
		}
		catch (...) { throw; }
	}

	//Load all textures from disk to opengl
	void loadAllTextures() {
		loadTextures(0, textures.size() - 1);
	}

	//Return curent textures count
	int textureCount() { return textures.size(); }
};

#endif