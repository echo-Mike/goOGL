#ifndef METALS_H
#define METALS_H "[0.0.5@SMetals.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of metal-like materials 
*		as part of material collection based on http://devernay.free.fr/cours/opengl/materials.html 
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//GLM
#include <GLM/glm.hpp>
//OUR
#include "CMaterial.h"
namespace material {
	MaterialPOD<> Gold(		glm::vec3(0.24725f, 0.1995f, 0.0745f), 
							glm::vec3(0.75164f, 0.60648f, 0.22648f),
							glm::vec3(0.628281f, 0.555802f, 0.366065f),
							128 * 0.4f );

	MaterialPOD<> Brass(	glm::vec3(0.329412, 0.223529, 0.027451),
							glm::vec3(0.780392, 0.568627, 0.113725),
							glm::vec3(0.992157,	0.941176, 0.807843),
							128 * 0.21794872f );

	MaterialPOD<> Bronze(	glm::vec3(0.2125, 0.1275, 0.054),
							glm::vec3(0.714, 0.4284, 0.18144),
							glm::vec3(0.393548, 0.271906, 0.166721),
							128 * 0.2f );

	MaterialPOD<> Chrome(	glm::vec3(0.25, 0.25, 0.25),
							glm::vec3(0.4, 0.4, 0.4),
							glm::vec3(0.774597, 0.774597, 0.774597),
							128 * 0.6f );

	MaterialPOD<> Copper(	glm::vec3(0.19125, 0.0735, 0.0225),
							glm::vec3(0.7038, 0.27048, 0.0828),
							glm::vec3(0.256777, 0.137622, 0.086014),
							128 * 0.1f );

	MaterialPOD<> Silver(	glm::vec3(0.19225, 0.19225, 0.19225),
							glm::vec3(0.50754, 0.50754, 0.50754),
							glm::vec3(0.508273, 0.508273, 0.508273),
							128 * 0.4f );
}
#endif