#ifndef UNIFORMTEXTURE_H
#define UNIFORMTEXTURE_H "[0.0.1@CUniformTexture.h]"
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
//GLEW
#include <GL/glew.h>
//OUR
#include "CUniforms.h"
#include "Ctexture.h"
//DEBUG
#ifdef DEBUG_UNIFORMTEXTURE
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#ifndef TEXTURE_HANDLER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of texture
	#define TEXTURE_HANDLER_STD_SHADER_VARIABLE_NAME "Texture"
#endif

/* The automatic storage for uniform samplaer2D in-shader value.
*  Class template definition: TextureHandler
*/
template <	class TTexture = Texture, class TShader = Shader,
			void (TTexture::* TextureLoader)(void)	= &TTexture::LoadToGL,
			void (TTexture::* BindTexture)(void)	= &TTexture::Use >
class TextureHandler : public UniformHandler<TTexture, TShader> {
	GLuint textureSlot;
	int textureUnit;
public:

	TextureHandler() :	UniformHandler(TTexture(), nullptr, TEXTURE_HANDLER_STD_SHADER_VARIABLE_NAME), 
						textureSlot(GL_TEXTURE0), 
						textureUnit(0) {}

	TextureHandler(	TTexture &_texture, TShader *_shader,
					GLuint _textureSlot = GL_TEXTURE0, int _textureUnit = 0,
					const char* _name = TEXTURE_HANDLER_STD_SHADER_VARIABLE_NAME) :
					UniformHandler(_texture, _shader, _name), 
					textureSlot(_textureSlot), textureUnit(_textureUnit) {}

	TextureHandler(	TTexture &_texture, TShader *_shader,
					GLuint _textureSlot = GL_TEXTURE0, int _textureUnit = 0,
					std::string _name = std::string(TEXTURE_HANDLER_STD_SHADER_VARIABLE_NAME)) :
					UniformHandler(_texture, _shader, _name),
					textureSlot(_textureSlot), textureUnit(_textureUnit) {}
	
	TextureHandler(const TextureHandler& other) :	UniformHandler(other), 
													textureSlot(other.textureSlot), 
													textureUnit(other.textureUnit) {}

	TextureHandler(TextureHandler &&other) :	UniformHandler(other),
												textureSlot(std::move(other.textureSlot)), 
												textureUnit(std::move(other.textureUnit)) {}

	~TextureHandler() = default;

	TextureHandler& operator=(TextureHandler other) {
		if (&other == this)
			return *this;
		std::swap(textureSlot, other.textureSlot);
		std::swap(textureUnit, other.textureUnit);
		UniformHandler<TTexture, TShader>::operator=(other);
		return *this;
	}

	TextureHandler& operator=(TextureHandler &&other) {
		textureSlot = std::move(other.textureSlot);
		textureUnit = std::move(other.textureUnit);
		UniformHandler<TTexture, TShader>::operator=(other);
		return *this;
	}

	//Load texture from disk to opengl
	void LoadTexture() { (value.*TextureLoader)(); }

	void bindUniform(GLint location) {
		glActiveTexture(textureSlot);
		(value.*BindTexture)();
		glUniform1i(location, textureUnit);
	}

	//Set texture slot of current texture handler (with bound check)
	void setTextureSlot(GLuint newSlot) {
		if (newSlot >= GL_TEXTURE0 && newSlot <= GL_TEXTURE31) {
			textureSlot = newSlot;
		} else {
			#ifdef DEBUG_UNIFORMTEXTURE
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
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::setTextureUnit::INVALID_TEXTURE_UNIT" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tUnit: " << newUnit << DEBUG_NEXT_LINE;
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

/* The automatic storage stack-like for multiple uniform samplaer2D in-shader value.
*  Class template definition: MultipleTextureHandler
*/
template < class TTexture = Texture, class TShader = Shader >
class MultipleTextureHandler {
	//Texture array, max count: 32
	std::vector<TextureHandler<TTexture, TShader>> textures;
	//Pointer to shader
	TShader *shader;
public:

	MultipleTextureHandler() : shader(nullptr) {}
	
	~MultipleTextureHandler() { shader = nullptr; }

	//Push texture to texture stack
	void pushTexture(TTexture &t) {
		int _size = textures.size();
		if (_size == 32) {
			popTexture();
			_size--;
		}
		TextureHandler<TTexture, TShader> _handler(t, shader, 0, 0, MULTIPLE_TEXTURE_HANDLER_TEXTURE_NAMES[_size]);
		try {
			textures.push_back(_handler);
		}
		catch (std::length_error e) {
			#ifdef DEBUG_UNIFORMTEXTURE
				DEBUG_OUT << "ERROR::MULTIPLE_TEXTURE_HANDLER::pushTexture::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't push more textures" << DEBUG_NEXT_LINE;
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
		textures.back().setTextureSlot( GL_TEXTURE0 + (GLuint)(textures.size() - 1));
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
	void setShader(TShader *_shader) { shader = _shader; }

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
};
#endif