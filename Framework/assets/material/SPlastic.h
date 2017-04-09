#ifndef PLASTIC_H
#define PLASTIC_H "[0.0.2@SPlastic.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of colorful plastic materials 
*		as part of material collection based on http://devernay.free.fr/cours/opengl/materials.html 
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//GLM
#include <GLM/glm.hpp>
//OUR
#include "general\CUniformStructCollection.h"
namespace material {
	MaterialPOD BlackPlastic (	glm::vec3(0.0, 0.0, 0.0),
								glm::vec3(0.01, 0.01, 0.01),
								glm::vec3(0.5, 0.5, 0.5),
								128 * 0.25f );

	MaterialPOD CyanPlastic (	glm::vec3(0.0, 0.1, 0.06),
								glm::vec3(0.0, 0.50980392, 0.50980392),
								glm::vec3(0.50196078, 0.50196078, 0.50196078),
								128 * 0.25f );

	MaterialPOD GreenPlastic (	glm::vec3(0.0, 0.0, 0.0),
								glm::vec3(0.1, 0.35, 0.1),
								glm::vec3(0.45, 0.55, 0.45),
								128 * 0.25f );

	MaterialPOD RedPlastic (	glm::vec3(0.0, 0.0, 0.0),
								glm::vec3(0.5, 0.0, 0.0),
								glm::vec3(0.7, 0.6, 0.6),
								128 * 0.25f );

	MaterialPOD WhitePlastic (	glm::vec3(0.0, 0.0, 0.0),
								glm::vec3(0.55, 0.55, 0.55),
								glm::vec3(0.7, 0.7, 0.7),
								128 * 0.25f );

	MaterialPOD YellowPlastic (	glm::vec3(0.0, 0.0, 0.0),
								glm::vec3(0.5, 0.5, 0.0),
								glm::vec3(0.6, 0.6, 0.5),
								128 * 0.25f );
}
#endif