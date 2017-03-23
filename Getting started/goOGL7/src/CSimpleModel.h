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

#ifndef SIMPLEMODEL_TRANSFORM_MATRIX_NAME
	//In shader variable name of transform matrix
	#define SIMPLEMODEL_TRANSFORM_MATRIX_NAME "transform"
#endif

#ifndef SIMPLEMODEL_VERTEX_SIZE 
	//Length of vertex position vector
	#define SIMPLEMODEL_VERTEX_SIZE 3
#endif

#ifndef SIMPLEMODEL_COLOR_SIZE
	//Length of vertex color vector
	#define SIMPLEMODEL_COLOR_SIZE 3
#endif

#ifndef SIMPLEMODEL_TEXCOORD_SIZE
	//Length of vertex texture coordinates vector
	#define SIMPLEMODEL_TEXCOORD_SIZE 2
#endif

#ifndef SIMPLEMODEL_NORMAL_SIZE
	//Length of vertex normal vector
	#define SIMPLEMODEL_NORMAL_SIZE 3
#endif

//Class definition: GLBufferHandler
class GLBufferHandler {
public: 
	enum BufferType : int {
		VERTEX		= 0x01,
		COLOR		= 0x02,
		TEXCOORD	= 0x04,
		NORMAL		= 0x08,
		ELEMENT		= 0x10,
		VERTEXARRAY	= 0x20,
		COMBINED	= 0x40
	};
private:
	//Allocator sorage
	int allocator;

	//OpenGL internal
	struct {
		GLuint vertex		= 0;	//VBO
		GLuint color		= 0;
		GLuint textureCoord	= 0;
		GLuint normal		= 0;
		GLuint element		= 0;	//EBO
		GLuint vertexArray	= 0;	//VAO
		GLuint combined		= 0;	//VBO

		GLuint operator[](BufferType index) {
			switch (index) {
				case VERTEX:
					return vertex;
					break;
				case COLOR:
					return color;
					break;
				case TEXCOORD:
					return textureCoord;
					break;
				case NORMAL:
					return normal;
					break;
				case ELEMENT:
					return element;
					break;
				case VERTEXARRAY:
					return vertexArray;
					break;
				case COMBINED:
					return combined;
					break;
				default:
					return 0;
					break;
			}
		}
	} GLbuffers;
protected:
	//Allocate buffers in OpenGL by "alloc" allocator 
	void allocate(int alloc) {
		allocator = alloc;
		if (allocator & VERTEX)
			glGenBuffers(1, &GLbuffers.vertex);
		if (allocator & COLOR)
			glGenBuffers(1, &GLbuffers.color);
		if (allocator & TEXCOORD)
			glGenBuffers(1, &GLbuffers.textureCoord);
		if (allocator & NORMAL)
			glGenBuffers(1, &GLbuffers.normal);
		if (allocator & COMBINED)
			glGenBuffers(1, &GLbuffers.combined);
		if (allocator & ELEMENT)
			glGenBuffers(1, &GLbuffers.element);
		if (allocator & VERTEXARRAY)
			glGenVertexArrays(1, &GLbuffers.vertexArray);
	}

	//Bind buffer by type "type" to "target" OpenGL target(std : GL_ARRAY_BUFFER)
	void bindBuffer(BufferType type, GLuint target = GL_ARRAY_BUFFER) {
		if (allocator & type) {
			if (type == VERTEXARRAY) {
				glBindVertexArray(GLbuffers[VERTEXARRAY]);
			} else {
				glBindBuffer(target, GLbuffers[type]);
			}
		}
	}
public:
	GLBufferHandler() : allocator(0) {};

	GLBufferHandler(int alloc) : allocator(alloc) {	allocate(alloc); }

	~GLBufferHandler() {
		glDeleteBuffers(1, &GLbuffers.vertex);
		glDeleteBuffers(1, &GLbuffers.color);
		glDeleteBuffers(1, &GLbuffers.textureCoord);
		glDeleteBuffers(1, &GLbuffers.normal);
		glDeleteBuffers(1, &GLbuffers.element);
		glDeleteBuffers(1, &GLbuffers.combined);
		glDeleteVertexArrays(1, &GLbuffers.vertexArray);
	}
};

//Helper function for glm::mat4 matrix binding to OpenGL uniform Mat4
static void stdBindMatrix(GLint location, glm::mat4 &m) {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

//Class template definition: InstanceHandler
template <	class Matrix = glm::mat4, class TShader = Shader,
			GLint (TShader::* _getUniformLocation)(const char*) = &TShader::getUniformLocation>
class InstanceHandler {
	//Transform matrix array
	std::vector<Matrix> transformation_data;
protected:
	//Load instance data
	void bindInstance(int index, TShader *shader, void(*BindMatrix)(GLint, Matrix&) = &stdBindMatrix) {
		if (transformation_data.size() > 0) {
			GLint _location = (shader->*_getUniformLocation)(SIMPLEMODEL_TRANSFORM_MATRIX_NAME);
				if (_location == -1) { //Check if uniform not found
					#ifdef GEBUG_SIMPLEMODEL
						DEBUG_OUT << "ERROR::INSTANCE_HANDLER::bindInstance::UNIFORM_NAME_MISSING" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tName: " << SIMPLEMODEL_TRANSFORM_MATRIX_NAME << DEBUG_NEXT_LINE;
					#endif
					return;
				}
			try {
				(*BindMatrix)(_location, transformation_data.at(index));
			}
			catch (std::out_of_range e) {
				#ifdef GEBUG_SIMPLEMODEL
					DEBUG_OUT << "ERROR::SEPARATE_MODEL::drawInstance::OUT_OF_RANGE\n\tIn index: " << index << DEBUG_NEXT_LINE;
					DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
				#endif
				return;
			}
		}
	}
public:
	//Draw one instance of model using instance data
	virtual void drawInstance(int index) { return; }

	//Draw multiple instances of model using their transform matrices
	virtual void drawInstances(int start_index = 0, int count = 1) {
		if (count < 1) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::drawInstances::INVALID_COUNT: " << count << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		for (int _index = start_index; _index < start_index + count; _index++)
			this->drawInstance(_index);
	}

	//Generate place for "count" instances of model
	void genInstances(int count = 1) {
		if (count > 0) {
			for (int _index = 0; _index < count; _index++) {
				try {
					pushTransformation(Matrix());
				}
				catch (...) {
					throw;
				}
			}
		} else {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::genInstances::INVALID_COUNT" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't generate count: " << count << " instances." << DEBUG_NEXT_LINE;
			#endif
		}
	}

	//Push transformation to transformation array
	void pushTransformation(Matrix transform) {
		try {
			transformation_data.push_back(std::move(transform));
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::pushTransformation::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't push more transformations " << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::pushTransformations::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Pop last transformation from transformation array
	void popTransformation() {
		if (transformation_data.empty()) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::popTransformation::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't pop from empty transform array." << DEBUG_NEXT_LINE;
			#endif
		} else {
			transformation_data.pop_back();
		}
	}

	//Setup transformation for "index" model instance
	void setTransformation(Matrix transform, int index = 0) {
		try {
			transformation_data.at(index) = std::move(transform);
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::setTransformation::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tCan't access transformation by index: " << index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::setTransformation::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Remove transformations from start index to end index
	void eraseTransformations(int start = 0, int end = 0) {
		auto _start = transformation_data.begin(), _end = transformation_data.begin();
		_start += start;
		_end += end;
		transformation_data.erase(_start, _end);
	}

	//Return curent helded instances count
	int instanceCount() { return transformation_data.size(); }
};

/*
//Class template definition: InstanceHandler
class InstanceHandler {
	//Transform matrix array
	std::vector<MatrixHandler> transformation_data;
protected:
	//Load instance data
	void bindInstance(int index) {
		if (transformation_data.size() > 0) {
			try {
				
			}
			catch (std::out_of_range e) {
				#ifdef GEBUG_SIMPLEMODEL
					DEBUG_OUT << "ERROR::SEPARATE_MODEL::drawInstance::OUT_OF_RANGE\n\tIn index: " << index << DEBUG_NEXT_LINE;
					DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
				#endif
				return;
			}
		}
	}
public:
	//Draw one instance of model using instance data
	virtual void drawInstance(int index) { return; }

	//Draw multiple instances of model using their transform matrices
	virtual void drawInstances(int start_index = 0, int count = 1) {
		if (count < 1) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::drawInstances::INVALID_COUNT: " << count << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		for (int _index = start_index; _index < start_index + count; _index++)
			this->drawInstance(_index);
	}

	//Generate place for "count" instances of model
	void genInstances(Shader* _shader, int count = 1) {
		if (count > 0) {
			for (int _index = 0; _index < count; _index++) {
				try {
					pushTransformation(MatrixHandler(glm::mat4(), _shader, SIMPLEMODEL_TRANSFORM_MATRIX_NAME));
				}
				catch (...) {
					throw;
				}
			}

		} else {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::genInstances::INVALID_COUNT" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't generate count: " << count << " instances." << DEBUG_NEXT_LINE;
			#endif
		}
	}

	//Push transformation to transformation array
	void pushTransformation(MatrixHandler transform) {
		try {
			transformation_data.push_back(std::move(transform));
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::pushTransformation::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't push more transformations " << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::pushTransformations::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Push transformation to transformation array
	void pushTransformation(glm::mat4 _matrix, Shader* _shader) {
		try {
			pushTransformation(MatrixHandler(_matrix, _shader, SIMPLEMODEL_TRANSFORM_MATRIX_NAME));
		}
		catch (...) {
			throw;
		}
	}

	//Pop last transformation from transformation array
	void popTransformation() {
		if (transformation_data.empty()) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::popTransformation::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't pop from empty transform array." << DEBUG_NEXT_LINE;
			#endif
		} else {
			transformation_data.pop_back();
		}
	}

	//Setup transformation for "index" model instance
	void setTransformation(MatrixHandler transform, int index = 0) {
		try {
			transformation_data.at(index) = std::move(transform);
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::setTransformation::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tCan't access transformation by index: " << index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::INSTANCE_HANDLER::setTransformation::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Setup transformation for "index" model instance
	void setTransformation(glm::mat4 _matrix, Shader* _shader, int _index = 0) {
		try {
			setTransformation(MatrixHandler(_matrix, _shader, SIMPLEMODEL_TRANSFORM_MATRIX_NAME), _index);
		}
		catch (...) {
			throw;
		}
	}

	//Remove transformations from start index to end index
	void eraseTransformations(int start = 0, int end = 0) {
		auto _start = transformation_data.begin(), _end = transformation_data.begin();
		_start += start;
		_end += end;
		transformation_data.erase(_start, _end);
	}

	//Return curent helded instances count
	int instanceCount() { return transformation_data.size(); }
};
*/

/*
//Class template definition: TextureHandler
template <	class TTexture = Texture, class TShader = Shader,
			void (TTexture::* TextureLoader)(void)	= &TTexture::LoadToGL,
			void (TTexture::* BindTexture)(void)	= &TTexture::Use,
			void (TShader::* BindShader)(void)		= &TShader::Use,
			void (TShader::* ReloadShader)(void)	= &TShader::Reload,
			GLuint (TShader::* GetShaderID)(void)	= &TShader::getShaderId>
class TextureHandler {
	//Texture array, max count: 32
	std::vector<TTexture> textures;
protected:
	//Pointer to shader
	TShader *shader;

	//Returns shader object id
	GLuint getShaderId() { return (shader->*GetShaderID)(); }

	//Internal funcion for shader applying
	void useShader() {
		(shader->*BindShader)();
		int _size = textures.size();
		GLint _location = 0;
		for (int _index = 0; _index < _size; _index++) {
			(textures[_index].*BindTexture)();
			_location = glGetUniformLocation((shader->*GetShaderID)(), SIMPLEMODEL_TEXTURE_NAMES[_index]);
			if (_location == -1) {
				#ifdef GEBUG_SIMPLEMODEL
					DEBUG_OUT << "ERROR::SIMPLE_MODEL::useShader::TEXTURE_NAME_MISSING\n\tName: " << SIMPLEMODEL_TEXTURE_NAMES[_index] << DEBUG_NEXT_LINE;
				#endif
				continue;
			}
			glUniform1i(_location, _index);
		}
	}
public:

	TextureHandler() : shader(nullptr) {}
	
	~TextureHandler() { shader = nullptr; }

	//Setup shader
	void setShader(TShader *s) { shader = s; }

	//Reload shader from disk
	void reloadShader() { (shader->*ReloadShader)(); }

	//Push texture to texture stack
	void pushTexture(TTexture &t) {
		if (textures.size() == 32)
			popTexture();
		try {
			textures.push_back(std::move(t));
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::pushTexture::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't push more textures" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::pushTexture::UNKONWN" << DEBUG_NEXT_LINE;
			#endif
		}
		//Setup texture slot
		textures.back().TextureSlot = GL_TEXTURE0 + (GLuint)(textures.size() - 1);
	}

	//Pop texture from texture stack
	void popTexture() {
		if (textures.empty()) {
			#ifdef GEBUG_SIMPLEMODEL
			DEBUG_OUT << "ERROR::TEXTURE_HANDLER::popTexture::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
			DEBUG_OUT << "\tCan't pop from empty texture stack." << DEBUG_NEXT_LINE;
			#endif
		} else {
			textures.back().TextureSlot = GL_TEXTURE0;
			textures.pop_back();
		}
	}

	//Load texture from disk to opengl
	void loadTexture(int index = 0) {
		try {
			(textures.at(index).*TextureLoader)();
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::loadTexture::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tCan't access texture by index: " << index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::TEXTURE_HANDLER::loadTexture::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Load textures from range [start,end] from disk to opengl
	void loadTextures(int start = 0, int end = 0) {
		for (int _index = start; _index <= end; _index++)
			loadTexture(_index);
	}

	//Load all textures from disk to opengl
	void loadAllTextures() {
		loadTextures(0, textures.size() - 1);
	}

	//Return curent textures count
	int textureCount() { return textures.size(); }
};

*/

//Class template definition: SimpleModel
template<	class TShader = Shader, class TTexture = Texture, class TMatrix = glm::mat4 
			void (TShader::* ApplyShader)(void) = &TShader::Use>
class SimpleModel : public GLBufferHandler, public InstanceHandler<TMatrix, TShader>, public MultipleTextureHandler<TTexture, TShader> {
	//Pointer to shader
	TShader *shader;
public:
	SimpleModel() : GLBufferHandler(), MultipleTextureHandler() {}

	~SimpleModel() { shader = nullptr; }

	//Draw one instance of model using instance data
	virtual void drawInstance(int index = 0, GLboolean applay_shader = GL_TRUE) { return; };

	//Draw multiple instances of model using their transform matrices
	void drawInstances(int start_index = 0, int count = 1, GLboolean applay_shader = GL_TRUE) {
		if (applay_shader == GL_TRUE)
			(shader->*ApplyShader)();
		if (count < 1) {
			#ifdef GEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLE_MODEL::drawInstances::INVALID_COUNT: " << count << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		for (int _index = start_index; _index < start_index + count; _index++)
			this->drawInstance(_index, GL_FALSE);
	}

	virtual void Build() { return; };
};

//Class template definition: SeparateModel
template<	class TShader = Shader, class TTexture = Texture, class TMatrix = glm::mat4
			void (TShader::* ApplyShader)(void) = &TShader::Use>
class SeparateModel : public SimpleModel<TShader, TTexture, TMatrix> {
	const GLfloat *vertices;
	const GLfloat *colors;
	const GLfloat *texCoords;
	const GLfloat *normals;
	const GLuint  *elements;
	int vertices_count, indexes_count;
public:
	SeparateModel(	int verticesCount,							int indexesCount,
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
			throw std::exception("ERROR::SEPARATE_MODEL::Constructor::Vertices array must be defined");
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
			throw std::exception("ERROR::SEPARATE_MODEL::Constructor::Element array must be defined");
		}
		bufferAlocator |= VERTEXARRAY;
		allocate(bufferAlocator);
	};

	void drawInstance(int index = 0, GLboolean applay_shader = GL_TRUE) {
		if (applay_shader == GL_TRUE)
			(shader->*ApplyShader)();
		bindInstance(index, shader); //LAST PARAMETER NOT USED === BAD
		bindBuffer(VERTEXARRAY);
		glDrawElements(GL_TRIANGLES, indexes_count, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void Build();
};

/*	Fix current model state in VAO
*	Attribute layout:
*	0 : vec3f() : position
*	1 : vec3f() : color
*	2 : vec2f() : texture coordinates
*	3 : vec3f() : normals
*/
template< class TShader = Shader, class TTexture = Texture, class TMatrix = glm::mat4 >
void SeparateModel<TShader, TTexture, TMatrix>::Build() {
	bindBuffer(VERTEXARRAY);
	{
		// 1. Copy our vertices array in a vertex buffer for OpenGL to use
		//Position attribute
		bindBuffer(VERTEX);
		glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(GLfloat)* SIMPLEMODEL_VERTEX_SIZE, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, SIMPLEMODEL_VERTEX_SIZE, GL_FLOAT, GL_FALSE, SIMPLEMODEL_VERTEX_SIZE * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//Color attribute
		if (colors != nullptr) {
			bindBuffer(COLOR);
			glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(GLfloat)* SIMPLEMODEL_COLOR_SIZE, colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, SIMPLEMODEL_COLOR_SIZE, GL_FLOAT, GL_FALSE, SIMPLEMODEL_COLOR_SIZE * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
		}
		//Texture coord attribute
		if (texCoords != nullptr) {
			bindBuffer(TEXCOORD);
			glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(GLfloat)* SIMPLEMODEL_TEXCOORD_SIZE, texCoords, GL_STATIC_DRAW);
			glVertexAttribPointer(2, SIMPLEMODEL_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE, SIMPLEMODEL_TEXCOORD_SIZE * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(2);
		}
		//Normal attribute
		if (normals != nullptr) {
			bindBuffer(NORMAL);
			glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(GLfloat)* SIMPLEMODEL_NORMAL_SIZE, normals, GL_STATIC_DRAW);
			glVertexAttribPointer(3, SIMPLEMODEL_NORMAL_SIZE, GL_FLOAT, GL_FALSE, SIMPLEMODEL_NORMAL_SIZE * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(3);
		}
		// 2. Copy our index array in a element buffer for OpenGL to use
		bindBuffer(ELEMENT, GL_ELEMENT_ARRAY_BUFFER);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes_count * sizeof(GLuint)* 3, elements, GL_STATIC_DRAW);
	}
	glBindVertexArray(0);
}

//Class template definition: CombinedModel
template<	class TShader = Shader, class TTexture = Texture, class TMatrix = glm::mat4
			void (TShader::* ApplyShader)(void) = &TShader::Use>
class CombinedModel : public SimpleModel<TShader, TTexture, TMatrix> {
public:
	struct Layout {
		int vertex_offset,		vertex_length;
		int color_offset,		color_length;
		int texCoord_offset,	texCoord_length;
		int normal_offset,		normal_length;
		int vertices_count,		indexes_count;
		bool indexed;

		Layout() :  vertex_offset(0),		vertex_length(SIMPLEMODEL_VERTEX_SIZE),
					color_offset(-1),		color_length(SIMPLEMODEL_COLOR_SIZE),
					texCoord_offset(-1),	texCoord_length(SIMPLEMODEL_TEXCOORD_SIZE),
					normal_offset(-1),		normal_length(SIMPLEMODEL_NORMAL_SIZE),
					vertices_count(0),		indexes_count(0),
					indexed(true) {}

		Layout( int vo, int vl, int co, int cl, 
				int to, int tl, int no, int nl, 
				int vc, int ic, bool ind) : vertex_offset(vo),		vertex_length(vl),
											color_offset(co),		color_length(cl),
											texCoord_offset(to),	texCoord_length(tl),
											normal_offset(no),		normal_length(nl),
											vertices_count(vc),		indexes_count(ic),
											indexed(ind) {}
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
			throw std::exception("ERROR::COMBINED_MODEL::Constructor::Vertices array must be defined");
		}
		if (indexesArray != nullptr) {
			bufferAlocator |= ELEMENT;
		} else if (layout.indexed) {
			throw std::exception("ERROR::COMBINED_MODEL::Constructor::Element array must be defined for indexed layout");
		}
		bufferAlocator |= VERTEXARRAY;
		allocate(bufferAlocator);
	}

	void drawInstance(int index = 0, GLboolean applay_shader = GL_TRUE) {
		if (applay_shader == GL_TRUE)
			(shader->*ApplyShader)();
		bindInstance(index, shader); //LAST PARAMETER NOT USED === BAD
		bindBuffer(VERTEXARRAY);
		if (layout.indexed) {
			glDrawElements(GL_TRIANGLES, layout.indexes_count, GL_UNSIGNED_INT, 0);
		} else {
			glDrawArrays(GL_TRIANGLES, 0, layout.vertices_count);
		}
		glBindVertexArray(0);
	}

	void Build();
};

/*	Fix current model state in VAO
*	Attribute layout:
*	0 : vec3f() : position
*	1 : vec3f() : color
*	2 : vec2f() : texture coordinates
*	3 : vec3f() : normals
*/
template< class TShader = Shader, class TTexture = Texture, class TMatrix = glm::mat4 >
void CombinedModel<TShader, TTexture, TMatrix>::Build() {
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

#endif