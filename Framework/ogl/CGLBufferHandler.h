#ifndef GLBUFFERHANDLER_H
#define GLBUFFERHANDLER_H "[0.0.5@CGLBufferHandler.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of OpenGL buffer handler class.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//GLEW
#include <GL/glew.h>

/* The manual allocator of OpenGL buffers.
*  Class definition: GLBufferHandler
*/
class GLBufferHandler {
public: 
	enum BufferType : int {
		VERTEX		= 0x001,
		COLOR		= 0x002,
		TEXCOORD	= 0x004,
		NORMAL		= 0x008,
		TANGENT		= 0x010,
		BITANGENT	= 0x020,
		ELEMENT		= 0x040,
		VERTEXARRAY	= 0x080,
		COMBINED	= 0x100
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
		GLuint tangent		= 0;
		GLuint bitangent	= 0;
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
				case TANGENT:
					return tangent;
					break;
				case BITANGENT:
					return bitangent;
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
		if (allocator & TANGENT)
			glGenBuffers(1, &GLbuffers.tangent);
		if (allocator & BITANGENT)
			glGenBuffers(1, &GLbuffers.bitangent);
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
		glDeleteBuffers(1, &GLbuffers.tangent);
		glDeleteBuffers(1, &GLbuffers.bitangent);
		glDeleteBuffers(1, &GLbuffers.element);
		glDeleteBuffers(1, &GLbuffers.combined);
		glDeleteVertexArrays(1, &GLbuffers.vertexArray);
	}
};
#endif