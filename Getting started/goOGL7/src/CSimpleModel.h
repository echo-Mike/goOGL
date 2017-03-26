#ifndef SIMPLEMODEL_H
#define SIMPLEMODEL_H
//STD
//std::vector
#include <vector>
//GLEW
#include <GL/glew.h>
//OUR
#include "CShader.h"
#include "Ctexture.h"
#include "CUniforms.h"
#include "CUniformMatrix.h"
#include "CUniformTexture.h"
//DEBUG
#ifdef DEBUG_SIMPLEMODEL
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
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

//Class variadic template definition: SimpleModel
template< typename ... Types>
class MultipleInstanceLoader {
public:
	typedef std::tuple<Types ...> InstanceData;
private:
	//Instannce data container
	std::vector<InstanceData> instances;
public:
	const int dataPerInstance = std::tuple_size<InstanceData>;
	
	//Generate place for "count" instances of model
	void genInstances(int count = 1) {
		if (count > 0) {
			for (int _index = 0; _index < count; _index++) {
				try {
					pushInstance(std::make_tuple(Types() ...));
				}
				catch (...) {
					throw;
				}
			}
		} else {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::genInstances::INVALID_COUNT" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't generate count: " << count << " instances." << DEBUG_NEXT_LINE;
			#endif
		}
	}

	//Push instance data to instance array
	void pushInstance(InstanceData _tuple) {
		try {
			instances.push_back(std::move(_tuple));
		}
		catch (std::length_error e) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::pushInstance::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't push more instance data." << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::pushInstance::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}
	
	//Push instance data to instance array
	void pushInstance(Types ... _data) { 
		try { pushInstance(std::make_tuple(_data ...)); }
		catch (...) { throw; }
	}
	
	//Pop last instance data from instance array
	void popInstance() {
		if (instances.empty()) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::popInstance::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't pop from empty instance array." << DEBUG_NEXT_LINE;
			#endif
		} else {
			instances.pop_back();
		}
	}

	//Setup instance data for "index" model instance
	void setInstance(int _index, InstanceData _tuple) {
		try {
			instances.at(_index) = std::move(_tuple);
		}
		catch (std::length_error e) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::setInstance::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tCan't access transformation by index: " << _index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::setInstance::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Setup instance data for "index" model instance
	void setInstance(int _index, Types ... _data) {
		try { setInstance(_index, std::make_tuple(_data ...)); }
		catch (...) { throw; }
	}

	//Remove instances from start index to end index
	void eraseInstance(int start = 0, int end = 0) {
		auto _start = data.begin(), _end = data.begin();
		_start += start;
		_end += end;
		data.erase(_start, _end);
	}
protected:
	/* Load instance data
	* All instance data expected to have loader function "bindData" 
	* a.e. to be UniformLoader or derived classes 
	*/
	void bindInstance(int index) {
		try {
			for (int _index = 0; _index < dataPerInstance; _index++)
				std::get<_index>(instances.at(index)).bindData();
		}
		catch (std::out_of_range e) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::bindInstance::OUT_OF_RANGE" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tIn index: " << index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		catch (...) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::bindInstance::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
			throw;
		}
	}
public:
	//Draw one instance of model using instance data
	virtual void drawInstance(int index) { return; }

	//Draw multiple instances of model using their data
	virtual void drawInstances(int start_index = 0, int count = 1) {
		if (count < 1) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::drawInstances::INVALID_COUNT: " << count << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		for (int _index = start_index; _index < start_index + count; _index++)
			this->drawInstance(_index);
	}
};

//Class template definition: SimpleModel
template<	class TShader = Shader, class TTexture = Texture, class TMatrix = MatrixLoader<our::mat4, TShader>, 
			void (TShader::* ApplyShader)(void) = &TShader::Use>
class SimpleModel : public GLBufferHandler, public MultipleInstanceLoader<TMatrix>, public MultipleTextureHandler<TTexture, TShader> {
	//Pointer to shader
	TShader *shader;
public:
	SimpleModel() : GLBufferHandler(), MultipleTextureHandler(), shader(nullptr) {}

	~SimpleModel() { shader = nullptr; }

	//Setup pointer to shader
	void setShader(TShader *_shader) { shader = _shader; }

	//Draw one instance of model using instance data
	virtual void drawInstance(int index = 0, GLboolean applay_shader = GL_TRUE) { return; };

	//Draw multiple instances of model using their transform matrices
	void drawInstances(int start_index = 0, int count = 1, GLboolean applay_shader = GL_TRUE) {
		if (applay_shader == GL_TRUE)
			(shader->*ApplyShader)();
		if (count < 1) {
			#ifdef DEBUG_SIMPLEMODEL
				DEBUG_OUT << "ERROR::SIMPLE_MODEL::drawInstances::INVALID_COUNT: " << count << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		for (int _index = start_index; _index < start_index + count; _index++)
			this->drawInstance(_index, GL_FALSE);
	}

	virtual void Build() { return; };
};

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

//Class template definition: SeparateModel
template<	class TShader = Shader, class TTexture = Texture, class TMatrix = MatrixLoader<our::mat4, TShader>,
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
		bindInstance(index);
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
template< class TShader = Shader, class TTexture = Texture, class TMatrix = MatrixLoader<our::mat4, TShader> >
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
template<	class TShader = Shader, class TTexture = Texture, class TMatrix = MatrixLoader<our::mat4, TShader>,
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
		bindInstance(index);
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
template< class TShader = Shader, class TTexture = Texture, class TMatrix = MatrixLoader<our::mat4, TShader> >
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