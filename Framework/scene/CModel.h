#ifndef MODEL_H
#define MODEL_H "[0.0.5@CModel.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of model class.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <vector>
#include <string>
//GLEW
#include <GL/glew.h>
//ASSIMP
#include "ASSIMP\Importer.hpp"
#include "ASSIMP\scene.h"
#include "ASSIMP\postprocess.h"
//OUR
#include "assets/shader/CShader.h"
#include "assets/textures/CTexture.h"
#include "assets/textures/CUniformTexture.h"
#include "ogl/CGLBufferHandler.h"
#include "assets\mesh\CSimpleMesh.h"
#include "assets\mesh\CCombinedMesh.h"
//DEBUG
#ifdef DEBUG_MODEL
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

//Main ASSIMP importer of all framework
namespace our {
	static Assimp::Importer __mainAssimpImporter;
};

/* DESCRIPTION
*  Class definition: Model
*/
class Model {
	std::string filePath;
public:
	
	Model(std::string _modelFilePath) : filePath(std::move(_modelFilePath)) {

	}

	Model(const Model& other) {

	}

	Model(Model&& other) {

	}

	void drawInstance(int _index) {

	}

	void drawInstances(int _index, int _count) {

	}
};

#endif