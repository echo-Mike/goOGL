#ifndef SEPARATEMESH_H
#define SEPARATEMESH_H "[0.0.5@CSeparateMesh.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of mesh storage.
*		Mesh is represented by multiple arrays of data.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//GLEW
#include <GL/glew.h>
//OUR
#include "CSimpleMesh.h"

#ifndef SEPARATEMESH_VERTEX_SIZE 
	//Length of vertex position vector
	#define SEPARATEMESH_VERTEX_SIZE 3
#endif
 
#ifndef SEPARATEMESH_COLOR_SIZE
	//Length of vertex color vector
	#define SEPARATEMESH_COLOR_SIZE 3
#endif

#ifndef SEPARATEMESH_TEXCOORD_SIZE
	//Length of vertex texture coordinates vector
	#define SEPARATEMESH_TEXCOORD_SIZE 2
#endif

#ifndef SEPARATEMESH_NORMAL_SIZE
	//Length of vertex normal vector
	#define SEPARATEMESH_NORMAL_SIZE 3
#endif

/* Implementation of storage of mesh represented by multiple arrays of data.
*  Class template definition: SeparateModel
*/
template<	class TShader = Shader, class TTexture = Texture, 
			void (TShader::* ApplyShader)(void) = &TShader::Use>
class SeparateMesh : public SimpleMesh<TShader, TTexture> {
	const GLfloat *vertices;
	const GLfloat *colors;
	const GLfloat *texCoords;
	const GLfloat *normals;
	const GLuint  *elements;
	int vertices_count, indexes_count;
public:
	SeparateMesh(	int verticesCount,							int indexesCount,
					const GLfloat *verticesArray = nullptr,		const GLfloat *colorsArray = nullptr,
					const GLfloat *texCoordsArray = nullptr,	const GLfloat *normalsArray = nullptr,
					const GLuint  *indexesArray = nullptr) :	vertices_count(verticesCount),	indexes_count(indexesCount),
																vertices(verticesArray),		colors(colorsArray),
																texCoords(texCoordsArray),		normals(normalsArray), 
																elements(indexesArray)
	{
		int bufferAlocator = 0;
		if (vertices != nullptr) {
			bufferAlocator |= VERTEX;
		}
		else {
			throw std::invalid_argument("ERROR::SEPARATE_MESH::Constructor::Vertices array must be defined");
		}
		if (colors != nullptr)
			bufferAlocator |= COLOR;
		if (texCoords != nullptr)
			bufferAlocator |= TEXCOORD;
		if (normals != nullptr)
			bufferAlocator |= NORMAL;
		if (elements != nullptr) {
			bufferAlocator |= ELEMENT;
		}
		else {
			throw std::invalid_argument("ERROR::SEPARATE_MESH::Constructor::Element array must be defined");
		}
		bufferAlocator |= VERTEXARRAY;
		SimpleMesh::allocate(bufferAlocator);
	};

	void drawInstance(int index = 0, GLboolean applay_shader = GL_TRUE) {
		if (applay_shader == GL_TRUE)
			(shader->*ApplyShader)();
		bindInstance(index);
		bindBuffer(VERTEXARRAY);
		glDrawElements(GL_TRIANGLES, indexes_count, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	/*	Fix current model state in VAO
	*	Attribute layout:
	*	0 : vec3f() : position
	*	1 : vec3f() : color
	*	2 : vec2f() : texture coordinates
	*	3 : vec3f() : normals
	*/
	void Build() {
		bindBuffer(VERTEXARRAY);
		{
			// 1. Copy our vertices array in a vertex buffer for OpenGL to use
			//Position attribute
			bindBuffer(VERTEX);
			glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(GLfloat)* SEPARATEMESH_VERTEX_SIZE, vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, SEPARATEMESH_VERTEX_SIZE, GL_FLOAT, GL_FALSE, SEPARATEMESH_VERTEX_SIZE * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			//Color attribute
			if (colors != nullptr) {
				bindBuffer(COLOR);
				glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(GLfloat)* SEPARATEMESH_COLOR_SIZE, colors, GL_STATIC_DRAW);
				glVertexAttribPointer(1, SEPARATEMESH_COLOR_SIZE, GL_FLOAT, GL_FALSE, SEPARATEMESH_COLOR_SIZE * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(1);
			}
			//Texture coord attribute
			if (texCoords != nullptr) {
				bindBuffer(TEXCOORD);
				glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(GLfloat)* SEPARATEMESH_TEXCOORD_SIZE, texCoords, GL_STATIC_DRAW);
				glVertexAttribPointer(2, SEPARATEMESH_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE, SEPARATEMESH_TEXCOORD_SIZE * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(2);
			}
			//Normal attribute
			if (normals != nullptr) {
				bindBuffer(NORMAL);
				glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(GLfloat)* SEPARATEMESH_NORMAL_SIZE, normals, GL_STATIC_DRAW);
				glVertexAttribPointer(3, SEPARATEMESH_NORMAL_SIZE, GL_FLOAT, GL_FALSE, SEPARATEMESH_NORMAL_SIZE * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(3);
			}
			// 2. Copy our index array in a element buffer for OpenGL to use
			bindBuffer(ELEMENT, GL_ELEMENT_ARRAY_BUFFER);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes_count * sizeof(GLuint)* 3, elements, GL_STATIC_DRAW);
		}
		glBindVertexArray(0);
	}
};
#endif