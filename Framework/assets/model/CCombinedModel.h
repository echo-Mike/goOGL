#ifndef COMBINEDMODEL_H
#define COMBINEDMODEL_H "[0.0.3@CCombinedModel.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of model storage.
*		Model is represented by single array of data and layout structure.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//GLEW
#include <GL/glew.h>
//OUR
#include "CSimpleModel.h"

/* Implementation of storage of model represented by arrays of data and layout structure.
*  Class template definition: CombinedModel
*/
template<	class TShader = Shader, class TTexture = Texture,
			void (TShader::* ApplyShader)(void) = &TShader::Use>
class CombinedModel : public SimpleModel<TShader, TTexture> {
public:
	struct Layout {
		int vertex_offset,		vertex_length;
		int color_offset,		color_length;
		int texCoord_offset,	texCoord_length;
		int normal_offset,		normal_length;
		int vertices_count,		indexes_count;
		bool indexed;
		int stride;

		Layout() :  vertex_offset(0),		vertex_length(SIMPLEMODEL_VERTEX_SIZE),
					color_offset(-1),		color_length(SIMPLEMODEL_COLOR_SIZE),
					texCoord_offset(-1),	texCoord_length(SIMPLEMODEL_TEXCOORD_SIZE),
					normal_offset(-1),		normal_length(SIMPLEMODEL_NORMAL_SIZE),
					vertices_count(0),		indexes_count(0),
					indexed(true),			stride(-1) {}
		/*
		vertex_offset,   vertex_length,
		color_offset,	 color_length,
		texCoord_offset, texCoord_length,
		normal_offset,   normal_length,
		vertices_count,  indexes_count,
		indexed,		 stride
		*/
		Layout( int vo, int vl, int co, int cl, 
				int to, int tl, int no, int nl, 
				int vc, int ic, bool ind, 
				int st = -1 ) : vertex_offset(vo),		vertex_length(vl),
								color_offset(co),		color_length(cl),
								texCoord_offset(to),	texCoord_length(tl),
								normal_offset(no),		normal_length(nl),
								vertices_count(vc),		indexes_count(ic),
								indexed(ind),			stride(st) {}
	};
private:
	Layout layout;
	const GLfloat *data;
	const GLuint *elements;
public:
	CombinedModel(	const Layout _layout, 
					const GLfloat *dataArray = nullptr,
					const GLuint *indexesArray = nullptr) : data(dataArray), elements(indexesArray), layout(_layout)
	{
		int bufferAlocator = 0;
		if (layout.vertex_offset > -1) {
			bufferAlocator |= COMBINED;
		} else {
			throw std::invalid_argument("ERROR::COMBINED_MODEL::Constructor::Vertices array must be defined");
		}
		if (indexesArray != nullptr) {
			bufferAlocator |= ELEMENT;
		} else if (layout.indexed) {
			throw std::invalid_argument("ERROR::COMBINED_MODEL::Constructor::Element array must be defined for indexed layout");
		}
		bufferAlocator |= VERTEXARRAY;
		allocate(bufferAlocator);
	}

	void drawInstance(int index = 0, GLboolean applay_shader = GL_TRUE) {
		if (applay_shader == GL_TRUE)
			(shader->*ApplyShader)();
		bindInstance(index);
		bindBuffer(VERTEXARRAY);
		if (layout.indexed) {
			glDrawElements(GL_TRIANGLES, layout.indexes_count, GL_UNSIGNED_INT, 0);
		} else {
			glDrawArrays(GL_TRIANGLES, 0, layout.vertices_count);
		}
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
			bindBuffer(COMBINED);
			int stride = layout.vertex_length;
			if (layout.color_offset > -1)
				stride += layout.color_length;
			if (layout.texCoord_offset > -1)
				stride += layout.texCoord_length;
			if (layout.normal_offset > -1)
				stride += layout.normal_length;
			
			/* This adds ability to use continuous arrays of data partially:
			* |   DATA SLOT 1   ||   DATA SLOT 2   |
			* |X|Y|Z|___|S|T|___||X|Y|Z|___|S|T|___|
			* |1|2|3|_|_|4|5|_|_||1|2|3|_|_|4|5|_|_|...
			* |----------------->|stride = 9
			*           |----------------->|stride = 9
			* Custom stride must be equal or higher than sum of length of data.
			*/
			if (layout.stride > stride)
				stride = layout.stride;

			glBufferData(GL_ARRAY_BUFFER, layout.vertices_count * sizeof(GLfloat) * stride, data, GL_STATIC_DRAW);

			glVertexAttribPointer(0, layout.vertex_length, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(layout.vertex_offset * sizeof(GLfloat)));
			glEnableVertexAttribArray(0);
			//Color attribute
			if (layout.color_offset > -1) {
				glVertexAttribPointer(1, layout.color_length, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(layout.color_offset * sizeof(GLfloat)));
				glEnableVertexAttribArray(1);
			}
			//Texture coord attribute
			if (layout.texCoord_offset > -1) {
				glVertexAttribPointer(2, layout.texCoord_length, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(layout.texCoord_offset * sizeof(GLfloat)));
				glEnableVertexAttribArray(2);
			}
			//Normal attribute
			if (layout.normal_offset > -1) {
				glVertexAttribPointer(3, layout.normal_length, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(layout.normal_offset * sizeof(GLfloat)));
				glEnableVertexAttribArray(3);
			}
			// 2. Copy our index array in a element buffer for OpenGL to use
			if (layout.indexed) {
				bindBuffer(ELEMENT, GL_ELEMENT_ARRAY_BUFFER);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, layout.indexes_count * sizeof(GLuint) * 3, elements, GL_STATIC_DRAW);
			}
		}
		glBindVertexArray(0);
	}
};
#endif