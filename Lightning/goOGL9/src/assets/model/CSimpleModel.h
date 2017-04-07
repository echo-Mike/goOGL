#ifndef SIMPLEMODEL_H
#define SIMPLEMODEL_H "[0.0.1@CSimpleModel.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of model base class.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//GLEW
#include <GL/glew.h>
//OUR
#include "assets/shader/CShader.h"
#include "assets/textures/CTexture.h"
#include "assets/textures/CUniformTexture.h"
#include "ogl/CGLBufferHandler.h"
#include "CInstanceLoader.h"
//DEBUG
#ifdef DEBUG_SIMPLEMODEL
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/* The base class for models, contains buffer allocator, instance loader and multiple texture handler.
*  Class template definition: SimpleModel
*/
template<	class TShader = Shader, class TTexture = Texture,
			void (TShader::* ApplyShader)(void) = &TShader::Use>
class SimpleModel : public GLBufferHandler, public MultipleInstanceLoader, public MultipleTextureHandler<TTexture, TShader> {
protected:
	//Pointer to shader
	TShader *shader;
public:
	SimpleModel() : GLBufferHandler(), MultipleTextureHandler(), shader(nullptr) {}

	~SimpleModel() { shader = nullptr; }

	//Setup pointer to shader
	void setShader(TShader *_shader) {
		shader = _shader;
		MultipleTextureHandler::setShader(_shader);
	}

	//Draw one instance of model using instance data
	virtual void drawInstance(int index = 0, GLboolean applay_shader = GL_TRUE) { return; };

	//Draw multiple instances of model using their transform matrices
	virtual void drawInstances(int start_index = 0, int count = 1, GLboolean applay_shader = GL_TRUE) {
		if (applay_shader == GL_TRUE)
			(shader->*ApplyShader)();
		if (count < 1) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLE_MODEL::drawInstances::INVALID_COUNT: " << count << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		for (int _index = start_index; _index < start_index + count; _index++)
			this->drawInstance(_index, GL_FALSE);
	}

	virtual void Build() { return; };
};
#endif