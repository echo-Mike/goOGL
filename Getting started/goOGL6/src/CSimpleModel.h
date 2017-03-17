#ifndef SIMPLEMODEL_H
#define SIMPLEMODEL_H
//STD
#include <vector>
//GLEW
#include <GL/glew.h>
//GLM
#include <GLM/glm.hpp>
#include <GLM/GTC/matrix_transform.hpp>
#include <GLM/GTC/type_ptr.hpp>
//OUR
#include "CShader.h"
#include "Ctexture.h"
//DEBUG
#ifdef GEBUG_SIMPLEMODEL
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#ifndef SIMPLEMODEL_TEXTURE_NAMES
#define SIMPLEMODEL_TEXTURE_NAMES textureNames
	//Static array for texture slot names
	static const char *SIMPLEMODEL_TEXTURE_NAMES[] = {
		"texture00", "texture01", "texture02", "texture03",
		"texture04", "texture05", "texture06", "texture07",
		"texture08", "texture09", "texture10", "texture11",
		"texture12", "texture13", "texture14", "texture15",
		"texture16", "texture17", "texture18", "texture19",
		"texture20", "texture21", "texture22", "texture23",
		"texture24", "texture25", "texture26", "texture27",
		"texture28", "texture29", "texture30", "texture31",
	};
#endif 

#ifndef SIMPLEMODEL_TRANSFORM_MATRIX_NAME
	//In shader variable name of transform matrix
	#define SIMPLEMODEL_TRANSFORM_MATRIX_NAME "transform"
#endif

//Class definition: SimpleModel
class SimpleModel {
	//OpenGL internal
	struct {
		GLuint vertex;			//VBO
		GLuint color;
		GLuint textureCoord;
		GLuint normal;
		GLuint element;			//EBO
		GLuint vertexArray;		//VAO
	} GLbuffers;
	//Model internal
	const GLfloat *vertices;
	const GLfloat *texCoords;
	const GLfloat *colors;
	const GLfloat *normals;
	const GLfloat *elements;
	GLsizei size;
	//Our internal
	Shader *shader;
	//Texture array, max count: 32
	std::vector<Texture> textures;
	//Transform matrix array
	std::vector<glm::mat4> transformations;

	//Internal funcion for shader applying
	void useShader() {
		shader->Use();
		int _size = textures.size();
		GLint _location = 0;
		for (int _index = 0; _index < _size; _index++) {
			textures[_index].Use();
			_location = glGetUniformLocation(shader->Program, SIMPLEMODEL_TEXTURE_NAMES[_index]);
			if (_location == -1) {
				#ifdef GEBUG_SIMPLEMODEL
					DEBUG_OUT << "ERROR::SIMPLEMODEL::useShader::TEXTURE_NAME_MISSING\n\tName: " << SIMPLEMODEL_TEXTURE_NAMES[_index] << DEBUG_NEXT_LINE;
				#endif
				continue;
			}
			glUniform1i(_location, _index);
		}
	}

public:
	SimpleModel(GLsizei meshSize,							const GLfloat *verticesArray = nullptr, 
				const GLfloat *texCoordsArray = nullptr,	const GLfloat *colorsArray = nullptr,
				const GLfloat *normalsArray = nullptr,		const GLfloat *indexesArray = nullptr) :	vertices(verticesArray),	texCoords(texCoordsArray),
																										colors(colorsArray),		normals(normalsArray), 
																										elements(indexesArray),		shader(nullptr),
																										size(meshSize)
	{
		if (vertices != nullptr) {
			glGenBuffers(1, &GLbuffers.vertex);
		} else {
			throw std::exception("Vertices array must be defined");
		}
		if (colors != nullptr)
			glGenBuffers(1, &GLbuffers.color);
		if (texCoords != nullptr)
			glGenBuffers(1, &GLbuffers.textureCoord);
		if (normals != nullptr)
			glGenBuffers(1, &GLbuffers.normal);
		if (vertices != nullptr) {
			glGenBuffers(1, &GLbuffers.element);
		} else {
			throw std::exception("Element array must be defined");
		}
		glGenVertexArrays(1, &GLbuffers.vertexArray);
	};
	
	//Setup shader
	void setShader(Shader *s) { shader = s; }
	
	//Push texture to texture stack
	void pushTexture(Texture &t) {
		if (textures.size() == 32)
			popTexture();
		try {
			textures.push_back(std::move(t));
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLEMODEL::pushTexture::OUT_OF_RANGE\n\tCan't push more textures" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLEMODEL::pushTexture::UNKONWN" << DEBUG_NEXT_LINE;
			#endif
		}
		//Setup texture slot
		textures.end()->TextureSlot = GL_TEXTURE0 + (GLuint)(textures.size() - 1);
	}

	//Pop texture from texture stack
	void popTexture() {
		if (textures.empty()) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLE_MODEL::popTexture::OUT_OF_RANGE\n\tCan't pop from empty texture stack." << DEBUG_NEXT_LINE;
			#endif
		} else {
			textures.end()->TextureSlot = GL_TEXTURE0;
			textures.pop_back();
		}
	}
	
	/*	Fix current model state in VAO
	 *	Attribute layout:
	 *	0 : vec3f() : position 
	 *	1 : vec3f() : color
	 *	2 : vec2f() : texture coordinates
	 *	3 : vec3f() : normals
	 */
	void Build() {
		glBindVertexArray(GLbuffers.vertexArray);
		{
			// 1. Copy our vertices array in a vertex buffer for OpenGL to use
			//Position attribute
			glBindBuffer(GL_ARRAY_BUFFER, GLbuffers.vertex);
			glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			//Color attribute
			if (colors != nullptr) {
				glBindBuffer(GL_ARRAY_BUFFER, GLbuffers.color);
				glBufferData(GL_ARRAY_BUFFER, size, colors, GL_STATIC_DRAW);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(1);
			}
			//Texture coord attribute
			if (texCoords != nullptr) {
				glBindBuffer(GL_ARRAY_BUFFER, GLbuffers.textureCoord);
				glBufferData(GL_ARRAY_BUFFER, size, texCoords, GL_STATIC_DRAW);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(2);
			}
			//Normal attribute
			if (normals != nullptr) {
				glBindBuffer(GL_ARRAY_BUFFER, GLbuffers.normal);
				glBufferData(GL_ARRAY_BUFFER, size, normals, GL_STATIC_DRAW);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(3);
			}
			// 2. Copy our index array in a element buffer for OpenGL to use
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLbuffers.element);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, elements, GL_STATIC_DRAW);
		}
		glBindVertexArray(0);
	}

	//Draw one instance of model using it transform matrix
	void drawInstance(int index = 0, GLboolean applay_shader = GL_TRUE) {
		if (applay_shader == GL_TRUE)
			useShader();
		if (transformations.size() > 0) {
			try {
				glUniformMatrix4fv( glGetUniformLocation(shader->Program, SIMPLEMODEL_TRANSFORM_MATRIX_NAME), 1,
									GL_FALSE, glm::value_ptr(transformations.at(index)));
			}
			catch (std::out_of_range e) {
				#ifdef GEBUG_SIMPLEMODEL
					DEBUG_OUT << "ERROR::SIMPLEMODEL::drawInstance::OUT_OF_RANGE\n\tIn index: " << index << DEBUG_NEXT_LINE;
					DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
				#endif
				return;
			}
		}
		glBindVertexArray(GLbuffers.vertexArray);
		glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	//Draw multiple instances of model using their transform matrices
	void drawInstances(int index = 0, int count = 1, GLboolean applay_shader = GL_TRUE) {
		if (applay_shader == GL_TRUE)
			useShader();
		if (count < 1) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLEMODEL::drawInstances::INVALID_COUNT: " << count << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		for (int _index = index; _index < index + count; _index++)
			drawInstance(_index, GL_FALSE);
	}

	//Return curent helded instances count
	int instanceCount() { return transformations.size(); }

	//Push transformation to transformation array
	void pushTransformation(glm::mat4 &transform) {
		try {
			transformations.push_back(std::move(transform));
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLEMODEL::pushTransformation::OUT_OF_RANGE\n\tCan't push more transformations " << DEBUG_NEXT_LINE;
				DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLEMODEL::pushTransformations::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Pop last transformation from transformation array
	void popTransformation() {
		if (transformations.empty()) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLE_MODEL::popTransformation::OUT_OF_RANGE\n\tCan't pop from empty transform array." << DEBUG_NEXT_LINE;
			#endif
		} else {
			transformations.pop_back();
		}
	}

	//Setup transformation for "index" model instance
	void setTransformation(glm::mat4 &transform, int index = 0) {
		try {
			transformations.at(index) = std::move(transform);
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLEMODEL::setTransformation::OUT_OF_RANGE\n\tCan't access transformation by index: "<< index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLEMODEL::setTransformation::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Remove transformations from start index to end index
	void eraseTransformations(int start = 0, int end = 0) {
		auto s = transformations.begin(), e = transformations.begin();
		s += start;
		e += end;
		transformations.erase(s, e);
	}

};

#endif