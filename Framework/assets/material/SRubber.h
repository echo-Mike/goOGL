#ifndef RUBBER_H
#define RUBBER_H "[0.0.2@SRubber.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of colorful rubber materials 
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
	MaterialPOD BlackRubber (	glm::vec3(0.02, 0.02, 0.02),
								glm::vec3(0.01, 0.01, 0.01),
								glm::vec3(0.4, 0.4, 0.4),
								128 * 0.078125f );

	MaterialPOD CyanRubber (	glm::vec3(0.0, 0.05, 0.05),
								glm::vec3(0.4, 0.5, 0.5),
								glm::vec3(0.04, 0.7, 0.7),
								128 * 0.078125f );

	MaterialPOD GreenRubber (	glm::vec3(0.0, 0.05, 0.0),
								glm::vec3(0.4, 0.5, 0.4),
								glm::vec3(0.04, 0.7, 0.04),
								128 * 0.078125f );

	MaterialPOD RedRubber (		glm::vec3(0.05, 0.0, 0.0),
								glm::vec3(0.5, 0.4, 0.4),
								glm::vec3(0.7, 0.04, 0.04),
								128 * 0.078125f );

	MaterialPOD WhiteRubber (	glm::vec3(0.05, 0.05, 0.05),
								glm::vec3(0.5, 0.5, 0.5),
								glm::vec3(0.7, 0.7, 0.7),
								128 * 0.078125f );

	MaterialPOD YellowRubber (	glm::vec3(0.05, 0.05, 0.0),
								glm::vec3(0.5, 0.5, 0.4),
								glm::vec3(0.7, 0.7, 0.04),
								128 * 0.078125f );
}
#endif