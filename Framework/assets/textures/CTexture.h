#ifndef TEXTURE_H
#define TEXTURE_H "[0.0.4@CTexture.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of texture class.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
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

struct TextureDataStructure {
	int width, height, depth;
	int SOILLoadType;
	GLuint GLId;
	GLuint GLTarget;
	GLuint GLStoreFormat;
	GLuint PixelDataFormat;
	GLuint PixelDataType;
	GLboolean HaveMimpmap;
	/*
	GLuint WarpingS;
	GLuint WarpingT;
	GLuint FilterMin;
	GLuint FilterMag;
	*/

	TextureDataStructure() : width(-1), height(-1),	depth(-1), GLId(0),						
							 GLTarget(GL_TEXTURE_2D),	GLStoreFormat(GL_RGB),	
							 PixelDataFormat(GL_RGB),	PixelDataType(GL_UNSIGNED_BYTE),
							 HaveMimpmap(GL_TRUE),		SOILLoadType(SOIL_LOAD_RGB) {}

	TextureDataStructure(const TextureDataStructure&) = default;

	//VS2013 does not support implicit generation of move constructors and move assignment operators
	TextureDataStructure(TextureDataStructure&& other) : width(std::move(other.width)), height(std::move(other.height)), depth(std::move(other.depth)), GLId(std::move(other.GLId)),
														 GLTarget(std::move(other.GLTarget)), GLStoreFormat(std::move(other.GLStoreFormat)),
														 PixelDataFormat(std::move(other.PixelDataFormat)), PixelDataType(std::move(other.PixelDataType)),
														 HaveMimpmap(std::move(other.HaveMimpmap)), SOILLoadType(std::move(other.SOILLoadType)) {}

	TextureDataStructure& operator=(const TextureDataStructure&) = default;
};

//Class definition: Texture
class Texture : public TextureDataStructure {
	std::string path;
	unsigned char* image_data;
	int load_status;
public:
	enum Load_Status : int {
		EMPTY		= 0x0,
		IN_MEMORY	= 0x1,
		IN_OPENGL	= 0x2,
		IN_BOTH		= 0x3,
		UNSTABLE	= 0x4
	};

	Texture() : TextureDataStructure(), path(""), image_data(nullptr), load_status(UNSTABLE) {}

	Texture(const char* texture_path, TextureDataStructure& texture_data) : TextureDataStructure(std::move(texture_data)),
																			path(texture_path), 
																			image_data(nullptr),
																			load_status(EMPTY) {}

	Texture(std::string &texture_path, TextureDataStructure& texture_data) : TextureDataStructure(std::move(texture_data)),
																			 path(std::move(texture_path)), 
																			 image_data(nullptr),
																			 load_status(EMPTY) {}

	Texture(const char* texture_path,			int load_type = SOIL_LOAD_RGB, 
			GLuint target = GL_TEXTURE_2D,		GLuint how_to_store = GL_RGB, 
			GLuint pixel_load_format = GL_RGB,	GLuint pixel_load_type = GL_UNSIGNED_BYTE,
			GLboolean applay_mipmap = GL_TRUE) :TextureDataStructure(), path(texture_path), load_status(EMPTY)
	{
		GLTarget = target;
		GLStoreFormat = how_to_store;
		PixelDataFormat = pixel_load_format;
		PixelDataType = pixel_load_type;
		HaveMimpmap = applay_mipmap;
		SOILLoadType = load_type;
	}

	Texture(std::string &texture_path,			int load_type = SOIL_LOAD_RGB, 
			GLuint target = GL_TEXTURE_2D,		GLuint how_to_store = GL_RGB, 
			GLuint pixel_load_format = GL_RGB,	GLuint pixel_load_type = GL_UNSIGNED_BYTE,
			GLboolean applay_mipmap = GL_TRUE) :TextureDataStructure(), path(std::move(texture_path)), load_status(EMPTY)
	{
		GLTarget = target;
		GLStoreFormat = how_to_store;
		PixelDataFormat = pixel_load_format;
		PixelDataType = pixel_load_type;
		HaveMimpmap = applay_mipmap;
		SOILLoadType = load_type;
	}

	Texture(const Texture& other) : TextureDataStructure(other), path(other.path), image_data(nullptr), load_status(EMPTY) {}

	Texture(Texture&& other) :	TextureDataStructure(std::move(other)),
								path(std::move(other.path)), 
								image_data(std::move(other.image_data)),
								load_status(std::move(other.load_status))
	{
		if (load_status & IN_MEMORY)
			other.image_data = nullptr;
	}

	~Texture() {
		if (load_status & IN_MEMORY)
			SOIL_free_image_data(image_data);
		if (GLId)
			glDeleteTextures(1, &GLId);
	}

	Texture& operator=(Texture other) {
		if (&other == this)
			return *this;
		std::swap(path, other.path);
		TextureDataStructure::operator=(other);
		GLId = 0;
		load_status = EMPTY;
		return *this;
	}

	//Load image data from disk to memory
	void LoadToMemory() {
		if (load_status & IN_MEMORY) {
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
			case UNSTABLE:
			case EMPTY:
			case IN_OPENGL:
				#ifdef DEBUG_TEXTURE
					DEBUG_OUT << "ERROR::TEXTURE::LoadFromMemoryToGL" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMeesage: Attempt to load empty memory to OpenGL." << DEBUG_NEXT_LINE;
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
		if (load_status & IN_MEMORY && !GLId) {
			glBindTexture(GLTarget, GLId);
		} else {
			#ifdef DEBUG_TEXTURE
				#ifdef WARNINGS_TEXTURE
					DEBUG_OUT << "WARNING::TEXTURE::Use" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Attempt to use unloaded texture." << DEBUG_NEXT_LINE;
				#endif
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

	//Setup new texture path
	void NewTexturePath(std::string texture_path) {
		path = std::move(texture_path);
		#ifdef DEBUG_TEXTURE
			DEBUG_OUT << "Texture path updated, new path:\n" << path << DEBUG_NEXT_LINE;
		#endif
	}
};
#endif