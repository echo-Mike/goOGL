#ifndef SCENE_H
#define SCENE_H "[0.0.5@CScene.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of scene class.
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

//OUR
#include "assets\mesh\CSimpleMesh.h"
//DEBUG
#ifdef DEBUG_SCENE
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/* DESCRIPTION
*  Class definition: Scene
*/
class Scene {
public:
	std::string name;
	unsigned int id;
	
	//std::vector<Animation*> mAnimations;
	//std::vector<Camera*> mCameras;
	//std::vector<Light*> mLights;
	//std::vector<Mesh*> mMeshes;
	//std::vector<Material*> mMaterials;
	
	//SceneResourceDatabase resourceDatabase;
	
	//ResourceHandler* privateResources;
	//ResourceHandler* publicResources;
	
	//Node rootNode;
	Scene() {

	}

	Scene(const Scene& other) {

	}

	Scene(Scene&& other) {

	}
};

#endif