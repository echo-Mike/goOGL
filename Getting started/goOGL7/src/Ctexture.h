#ifndef TEXTURE_H
#define TEXTURE_H
//STD
#include <string>
#include <iostream>
//GLEW
#include <GL/glew.h>
//SOIL
#include <SOIL/SOIL.h>
//DEBUG
#ifdef DEBUG_TEXTURE
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

//Class definition: Texture
class Texture {
	std::string path;
	unsigned char* image_data;
	int load_status;
public:
	enum {
		EMPTY,
		IN_MEMORY,
		IN_OPENGL,
		IN_BOTH
	};
	int width, height, depth;
	GLuint GLId;
	GLuint GLTarget;
	GLuint GLStoreFormat;
	GLuint PixelDataFormat;
	GLuint PixelDataType;
	/*
	GLuint WarpingS;
	GLuint WarpingT;
	GLuint FilterMin;
	GLuint FilterMag;
	*/
	GLboolean HaveMimpmap;
	int SOILLoadType;

	Texture() {}

	Texture(const char* texture_path,			int load_type = SOIL_LOAD_RGB, 
			GLuint target = GL_TEXTURE_2D,		GLuint how_to_store = GL_RGB, 
			GLuint pixel_load_format = GL_RGB,	GLuint pixel_load_type = GL_UNSIGNED_BYTE,
			GLuint texture_slot = GL_TEXTURE0,	GLboolean applay_mipmap = GL_TRUE) :	path(texture_path),				width(-1), 
																						height(-1),						depth(-1),
																						GLId(0),						GLTarget(target),
																						GLStoreFormat(how_to_store),	PixelDataFormat(pixel_load_format),
																						PixelDataType(pixel_load_type),	HaveMimpmap(applay_mipmap),	
																						SOILLoadType(load_type),		load_status(EMPTY) {}
	
	Texture(const Texture& t) : path(t.path),					width(t.width),
								height(t.height),				depth(t.depth),
								GLId(0),						GLTarget(t.GLTarget),
								GLStoreFormat(t.GLStoreFormat),	PixelDataFormat(t.PixelDataFormat),
								PixelDataType(t.PixelDataType), HaveMimpmap(t.HaveMimpmap),		
								SOILLoadType(t.SOILLoadType),	load_status(EMPTY) {}

	~Texture() {
		if (load_status == IN_MEMORY || load_status == IN_BOTH)
			SOIL_free_image_data(image_data);
		if (GLId)
			glDeleteTextures(1, &GLId);
	}

	Texture& operator=(Texture other) {
		if (&other == this)
			return *this;
		std::swap(path, other.path);
		std::swap(width, other.width);
		std::swap(height, other.height);
		std::swap(depth, other.depth);
		std::swap(GLStoreFormat, other.GLStoreFormat);
		std::swap(PixelDataFormat, other.PixelDataFormat);
		std::swap(PixelDataType, other.PixelDataType);
		std::swap(HaveMimpmap, other.HaveMimpmap);
		std::swap(SOILLoadType, other.SOILLoadType);
		load_status = EMPTY;
		GLId = 0;
		return *this;
	}

	//Load image data from disk to memory
	void LoadToMemory() {
		if (load_status == IN_MEMORY || load_status == IN_BOTH) {
			SOIL_free_image_data(image_data);
		} else {
			//IN_OPENGL to IN_BOTH, EMPTY to IN_MEMORY
			load_status++;
		}
		image_data = SOIL_load_image(path.c_str(), &width, &height, 0, SOILLoadType);
		if (!image_data) {
			#ifdef DEBUG_TEXTURE
				DEBUG_OUT << "ERROR::TEXTURE::SOIL\n\tCan't load image by path: \n" << path << DEBUG_NEXT_LINE;
				DEBUG_OUT << "SOIL ERROR STRING:\n" << SOIL_last_result() << DEBUG_NEXT_LINE;
			#endif
		} else {
			#ifdef DEBUG_TEXTURE
				DEBUG_OUT << "Texture loaded to Memory by path: \n" << path << DEBUG_NEXT_LINE;
			#endif
		}
	}
	
	//Load image data from memory to OpenGL
	void LoadFromMemoryToGL() {
		switch (load_status) {
			case EMPTY:
			case IN_OPENGL:
				#ifdef DEBUG_TEXTURE
					DEBUG_OUT << "ERROR::TEXTURE::LoadFromMemoryToGL\n\tAttempt to load empty memory to OpenGL" << DEBUG_NEXT_LINE;
				#endif
				break;
			case IN_BOTH:
				if (GLId)
					glDeleteTextures(1, &GLId);
			case IN_MEMORY:
				glGenTextures(1, &GLId);
				Warping();
				SamplingFilter();
				glBindTexture(GLTarget, GLId);
				switch (GLTarget) {
					case GL_TEXTURE_1D:
						glTexImage1D(GLTarget, 0, GLStoreFormat, width, 0, PixelDataFormat, PixelDataType, image_data);
						break;
					case GL_TEXTURE_2D:
						glTexImage2D(GLTarget, 0, GLStoreFormat, width, height, 0, PixelDataFormat, PixelDataType, image_data);
						break;
					case GL_TEXTURE_3D:
						glTexImage3D(GLTarget, 0, GLStoreFormat, width, height, depth, 0, PixelDataFormat, PixelDataType, image_data);
						break;
					default:
						break;
				}
				if (HaveMimpmap == GL_TRUE)
					glGenerateMipmap(GLTarget);
				glBindTexture(GLTarget, 0);
				load_status = IN_BOTH;
				#ifdef DEBUG_TEXTURE
					DEBUG_OUT << "Texture loaded from Memory to OpenGL" << DEBUG_NEXT_LINE;
				#endif
				break;
		}
	}

	//Load image data from disk to OpenGL
	void LoadToGL() {
		LoadToMemory();
		LoadFromMemoryToGL();
		SOIL_free_image_data(image_data);
		load_status = IN_OPENGL;
	}

	//Bind texture to OpenGL
	void Use() { 
		if (load_status == IN_OPENGL || load_status == IN_BOTH || !GLId) {
			glBindTexture(GLTarget, GLId);
		} else {
			#ifdef DEBUG_TEXTURE
				DEBUG_OUT << "ERROR::TEXTURE::Use\n\tAttempt to use unloaded texture" << DEBUG_NEXT_LINE;
			#endif
		}
	}

	//Setup warping parameters
	void Warping(GLuint s_warp = GL_REPEAT, GLuint t_warp = GL_REPEAT) {
		glBindTexture(GLTarget, GLId);
		glTexParameteri(GLTarget, GL_TEXTURE_WRAP_S, s_warp);
		glTexParameteri(GLTarget, GL_TEXTURE_WRAP_T, t_warp);
		glBindTexture(GLTarget, 0);
	}

	//Setup sampling filter parameters
	void SamplingFilter(GLuint min = GL_LINEAR, GLuint mag = GL_LINEAR) {
		glBindTexture(GLTarget, GLId);
		glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, min);
		glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, mag);
		glBindTexture(GLTarget, 0);
	}

	//Setup new texture path
	void NewTexturePath(const char* texture_path) {
		path = std::string(texture_path);
		#ifdef DEBUG_TEXTURE
			DEBUG_OUT << "Texture path updated, new path:\n" << path << DEBUG_NEXT_LINE;
		#endif
	}
};
#endif