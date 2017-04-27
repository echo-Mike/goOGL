#ifndef UNIFORMTEXTURE_H
#define UNIFORMTEXTURE_H "[0.0.3@CUniformTexture.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of in-shader uniform samplaer2D 
*		handling classes and templates based on uniform handling. Implementation of multiple texture handling.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <string>
#include <vector>
#include <utility>
//GLEW
#include <GL/glew.h>
//OUR
#include "assets/shader/CUniforms.h"
#include "CTexture.h"
//DEBUG
#ifdef DEBUG_UNIFORMTEXTURE
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#ifndef MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND
	/*Max texture count that can be loaded simultaneously
	* Platform dependent
	* For OpenGL 3.3 32 is guaranteed
	* For OpenGL 4.5 80 is guaranteed
	* FOR FUTURE: this define must be bounded with GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS OpenGL define
	* Somewhat good explanation of situation: 
	* http://stackoverflow.com/questions/8866904/differences-and-relationship-between-glactivetexture-and-glbindtexture
	*/
	#define MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND 32
#endif

#ifndef TEXTURE_STD_SHADER_VARIABLE_NAME
	//In shader variable name of texture
	#define TEXTURE_STD_SHADER_VARIABLE_NAME "Texture"
#endif

/* The storage for uniform samplaer2D in-shader value.
*  Class template definition: TextureStorage
*/
template <	class TBase, 
			class TTexture = Texture, class TShader = Shader,
			void (TTexture::* TextureLoader)(void)	= &TTexture::LoadToGL >
class TextureStorage : public TBase {
	typedef TBase Base;
protected:
	GLuint textureSlot;
	int textureUnit;
public:

	TextureStorage() :	Base(TTexture(), nullptr, TEXTURE_STD_SHADER_VARIABLE_NAME),
						textureSlot(GL_TEXTURE0), 
						textureUnit(0) {}

	TextureStorage(	TTexture *_texture, TShader *_shader,
					GLuint _textureSlot = GL_TEXTURE0, int _textureUnit = 0,
					const char* _name = TEXTURE_STD_SHADER_VARIABLE_NAME) :
					Base(_texture, _shader, _name), 
					textureSlot(_textureSlot), textureUnit(_textureUnit) {}

	TextureStorage(	TTexture *_texture, TShader *_shader,
					GLuint _textureSlot = GL_TEXTURE0, int _textureUnit = 0,
					std::string _name = std::string(TEXTURE_STD_SHADER_VARIABLE_NAME)) :
					Base(_texture, _shader, _name),
					textureSlot(_textureSlot), textureUnit(_textureUnit) {}
	
	TextureStorage(const TextureStorage& other) :	Base(other),
													textureSlot(other.textureSlot), 
													textureUnit(other.textureUnit) {}

	TextureStorage(TextureStorage &&other) :	Base(other),
												textureSlot(std::move(other.textureSlot)), 
												textureUnit(std::move(other.textureUnit)) {}

	~TextureStorage() = default;

	TextureStorage& operator=(TextureStorage other) {
		if (&other == this)
			return *this;
		std::swap(textureSlot, other.textureSlot);
		std::swap(textureUnit, other.textureUnit);
		Base::operator=(other);
		return *this;
	}

	TextureStorage& operator=(TextureStorage &&other) {
		textureSlot = std::move(other.textureSlot);
		textureUnit = std::move(other.textureUnit);
		Base::operator=(other);
		return *this;
	}

	//Load texture from disk to opengl
	void LoadTexture() { (value.*TextureLoader)(); }

	//Set texture slot of current texture handler (with bound check)
	void setTextureSlot(GLuint newSlot) {
		if (newSlot >= GL_TEXTURE0 && newSlot < GL_TEXTURE0 + MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND) {
			textureSlot = newSlot;
		} else {
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::TEXTURE_AUTOMATIC_STORAGE::setTextureSlot::INVALID_TEXTURE_SLOT" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tSlot: " << newSlot << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tRange: " << GL_TEXTURE0 << " : " << GL_TEXTURE0 + MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND << DEBUG_NEXT_LINE;
			#endif
		}
	}

	//Set texture unit of current texture handler (with bound check)
	void setTextureUnit(int newUnit) {
		if (newUnit >= 0 && newUnit < MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND) {
			textureUnit = newUnit;
		} else {
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::TEXTURE_AUTOMATIC_STORAGE::setTextureUnit::INVALID_TEXTURE_UNIT" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tUnit: " << newUnit << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tRange: 0 : " << MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND - 1 << DEBUG_NEXT_LINE;
			#endif
		}
	}
};

/* The automatic storage for uniform samplaer2D in-shader value.
*  Class template definition: TextureAutomaticStorage
*/
template <	class TTexture = Texture, class TShader = Shader,
			void (TTexture::* TextureLoader)(void) = &TTexture::LoadToGL,
			void (TTexture::* BindTexture)(void) = &TTexture::Use >
class TextureAutomaticStorage : public TextureStorage<UniformAutomaticStorage<TTexture, TShader>, TTexture, TShader, TextureLoader> {
	typedef TextureStorage<UniformAutomaticStorage<TTexture, TShader>, TTexture, TShader, TextureLoader> Base;
public:

	TextureAutomaticStorage() : Base() {}

	TextureAutomaticStorage(TTexture *_texture, TShader *_shader,
							GLuint _textureSlot = GL_TEXTURE0, int _textureUnit = 0,
							const char* _name = TEXTURE_STD_SHADER_VARIABLE_NAME) :
							Base(_texture, _shader, _textureSlot, _textureUnit, _name) {}

	TextureAutomaticStorage(TTexture *_texture, TShader *_shader,
							GLuint _textureSlot = GL_TEXTURE0, int _textureUnit = 0,
							std::string _name = std::string(TEXTURE_STD_SHADER_VARIABLE_NAME)) :
							Base(_texture, _shader, _textureSlot, _textureUnit, _name) {}
	
	TextureAutomaticStorage(const TextureAutomaticStorage& other) : Base(other) {}

	TextureAutomaticStorage(TextureAutomaticStorage &&other) : Base(other) {}

	~TextureAutomaticStorage() = default;

	TextureAutomaticStorage& operator=(TextureAutomaticStorage other) {
		Base::operator=(other);
		return *this;
	}

	TextureAutomaticStorage& operator=(TextureAutomaticStorage &&other) {
		Base::operator=(other);
		return *this;
	}

	void bindUniform(GLint location) {
		glActiveTexture(textureSlot);
		(value.*BindTexture)();
		glUniform1i(location, textureUnit);
	}
};

/* The manual storage for uniform samplaer2D in-shader value.
*  Class template definition: TextureManualStorage
*/
template <	class TTexture = Texture, class TShader = Shader,
			void (TTexture::* TextureLoader)(void) = &TTexture::LoadToGL,
			void (TTexture::* BindTexture)(void) = &TTexture::Use,
			GLint(TShader::* _getUniformLocation)(const char*) = &TShader::getUniformLocation >
class TextureManualStorage : public TextureStorage<UniformManualStorage<TTexture, TShader>, TTexture, TShader, TextureLoader> {
	typedef TextureStorage<UniformManualStorage<TTexture, TShader>, TTexture, TShader, TextureLoader> Base;
public:
	TextureManualStorage() : Base() {}

	TextureManualStorage(	TTexture *_texture, TShader *_shader,
							GLuint _textureSlot = GL_TEXTURE0, int _textureUnit = 0,
							const char* _name = TEXTURE_STD_SHADER_VARIABLE_NAME) :
							Base(_texture, _shader, _textureSlot, _textureUnit, _name) {}

	TextureManualStorage(	TTexture *_texture, TShader *_shader,
							GLuint _textureSlot = GL_TEXTURE0, int _textureUnit = 0,
							std::string _name = std::string(TEXTURE_STD_SHADER_VARIABLE_NAME)) :
							Base(_texture, _shader, _textureSlot, _textureUnit, _name) {}
	
	TextureManualStorage(const TextureManualStorage& other) : Base(other) {}

	TextureManualStorage(TextureManualStorage &&other) : Base(other) {}

	~TextureManualStorage() = default;

	TextureManualStorage& operator=(TextureManualStorage other) {
		Base::operator=(other);
		return *this;
	}

	TextureManualStorage& operator=(TextureManualStorage &&other) {
		Base::operator=(other);
		return *this;
	}

	//Bind sampler to shader
	void bindData() {
		GLint _location = (shader->*_getUniformLocation)(uniformName.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_UNIFORMMATRIX
				DEBUG_OUT << "ERROR::VEC3_MANUAL_STORAGE::bindData::UNIFORM_NAME_MISSING" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << uniformName << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		glActiveTexture(textureSlot);
		(value.*BindTexture)();
		glUniform1i(_location, textureUnit);
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

#ifndef MULTIPLE_TEXTURE_HANDLER_RESERVED
	//STD reserved texture slots
	#define MULTIPLE_TEXTURE_HANDLER_RESERVED 10
#endif

/* The automatic stack-like storage for multiple uniform samplaer2D in-shader values.
*  Class template definition: MultipleTextureHandler
*/
template < class TTexture = Texture, class TShader = Shader >
class MultipleTextureHandler {
	typedef TextureAutomaticStorage<TTexture, TShader> THandler;
	//Texture array, max count: MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND
	std::vector<THandler> textures;
	//Pointer to shader
	TShader *shader;
	//Count of reserved texture places
	int reserved;
public:

	MultipleTextureHandler() : shader(nullptr), reserved(MULTIPLE_TEXTURE_HANDLER_RESERVED) {}
	
	MultipleTextureHandler(int _reserved) : shader(nullptr), reserved(_reserved) {
		if (_reserved < 0 || _reserved > MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND)
			reserved = MULTIPLE_TEXTURE_HANDLER_RESERVED;
	}

	~MultipleTextureHandler() { shader = nullptr; }

	//Push texture to texture stack
	void pushTexture(TTexture* t, const char* _name = TEXTURE_STD_SHADER_VARIABLE_NAME) {
		int _size = textures.size();
		if (MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND == reserved) {
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::MULTIPLE_TEXTURE_HANDLER::pushTexture::ALL_RESERVED" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't push texture, all places reserved." << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		if (_size == MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND - reserved) {
			popTexture();
			_size--;
		}
		try {
			textures.push_back(std::move(THandler(t, shader, 0, 0, MULTIPLE_TEXTURE_HANDLER_TEXTURE_NAMES[_size])));
		}
		catch (std::length_error e) {
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::MULTIPLE_TEXTURE_HANDLER::pushTexture::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't push more textures." << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		catch (...) {
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::MULTIPLE_TEXTURE_HANDLER::pushTexture::UNKONWN" << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		//Setup texture
		textures.back().push();
		textures.back().setTextureSlot(GL_TEXTURE0 + (GLuint)(textures.size() - 1));
		textures.back().setTextureUnit(textures.size() - 1);
	}

	//Pop texture from texture stack
	void popTexture() {
		if (textures.empty()) {
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::MULTIPLE_TEXTURE_HANDLER::popTexture::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
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
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::MULTIPLE_TEXTURE_HANDLER::loadTexture::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMesseg: Can't access texture by index: " << index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::MULTIPLE_TEXTURE_HANDLER::loadTexture::UNKONWN"<< DEBUG_NEXT_LINE;
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
	void loadAllTextures() { loadTextures(0, textures.size() - 1); }

	//Return curent textures count
	int textureCount() { return textures.size(); }

	//Internal function for shader pointer setup
	void setShader(TShader *_shader) { 
		for (auto &v : textures)
			v.setShader(_shader);
		shader = _shader; 
	}

	//Get copy of texture in place _index
	TTexture& getTexture(int _index = 0) {
		try {
			return textures.at(index).getValue();
		}
		catch (std::length_error e) {
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::MULTIPLE_TEXTURE_HANDLER::getTexture::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMesseg: Can't access texture by index: " << index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::MULTIPLE_TEXTURE_HANDLER::getTexture::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
			throw;
		}
	}

	//Setup count of reserved texture slots
	void setReservedCount(int _reserved = MULTIPLE_TEXTURE_HANDLER_RESERVED) {
		//Do nothing check
		if (_reserved == reserved)
			return;
		//Invalid size check
		if (_reserved < 0 || _reserved > MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND)
			_reserved = MULTIPLE_TEXTURE_HANDLER_RESERVED;
		//Do nothing check
		if (_reserved == reserved)
			return;
		//Place check
		if (_reserved > reserved && MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND - textures.size() < _reserved)
			for (int _index = _reserved - MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND + textures.size(); _index > 0; _index--)
				popTexture();
		reserved = _reserved;
	}

	//Get reserved texture slots count
	int getResrvedCount() { return reserved; }

	//Get first available texture slot
	GLuint getReservedSlot(GLuint _index = 0) { return GL_TEXTURE0 + (GLuint)(MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND - reserved) + _index; }

	//Get first available texture unit
	int getReservedUnit(int _index = 0) { return MULTIPLE_TEXTURE_HANDLER_UPPER_BOUND - reserved + _index; }
};
#endif