#ifndef JEWELRY_H
#define JEWELRY_H "[0.0.5@SJewelry.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of jewelry materials 
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
	MaterialPOD<> Emerald(	glm::vec3(0.0215, 0.1745, 0.0215),
							glm::vec3(0.07568, 0.61424, 0.07568),
							glm::vec3(0.633, 0.727811, 0.633),
							128 * 0.6f );

	MaterialPOD<> Jade(		glm::vec3(0.135, 0.2225, 0.1575),
							glm::vec3(0.54, 0.89, 0.63),
							glm::vec3(0.316228, 0.316228, 0.316228),
							128 * 0.1f );

	MaterialPOD<> Obsidian(	glm::vec3(0.05375, 0.05, 0.06625),
							glm::vec3(0.18275, 0.17, 0.22525),
							glm::vec3(0.332741, 0.328634, 0.346435),
							128 * 0.3f );

	MaterialPOD<> Pearl(	glm::vec3(0.25, 0.20725, 0.20725),
							glm::vec3(1.0, 0.829, 0.829),
							glm::vec3(0.296648, 0.296648, 0.296648),
							128 * 0.088f);

	MaterialPOD<> Ruby(		glm::vec3(0.1745, 0.01175, 0.01175),
							glm::vec3(0.61424, 0.04136, 0.04136),
							glm::vec3(0.727811, 0.626959, 0.626959),
							128 * 0.6f );

	MaterialPOD<> Turquoise(glm::vec3(0.1, 0.18725, 0.1745),
							glm::vec3(0.396, 0.74151, 0.69102),
							glm::vec3(0.297254, 0.30829, 0.306678),
							128 * 0.1f );
}
#endif