#ifndef CAMERA_H
#define CAMERA_H
//GLEW
#include <GL/glew.h>
//GLM
#include <GLM/glm.hpp>
#include <GLM/GTC/matrix_transform.hpp>
#include <GLM/GTC/type_ptr.hpp>
#include <GLM/GTC/quaternion.hpp>
//DEBUG
#ifdef DEBUG_CAMERA
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/*
#ifndef CAMERA_STD_MATRIX_TYPE
	//In shader type name of matrix
	#define CAMERA_STD_MATRIX_TYPE Shader::UNIFORMMATRIX4FV
#endif
*/


#ifndef CAMERA_STD_PROJECTION_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define CAMERA_STD_PROJECTION_SHADER_VARIABLE_NAME "projection"
#endif

/**
//Class definition: MatrixHandler
template<	class TMatrix = glm::mat4, class TShader = Shader,
			int (TShader::* NewUniform)(const char*, void*, int)	= &TShader::pushUniform,
			void (TShader::* UpdateUniform)(void*, int)				= &TShader::setUniform>
class MatrixHandler {
	TMatrix matrix;
	TShader *shader;
	std::string shaderVariableName;
	int uniformId, uniformType;
protected:
	/*
	//Helper function for derived classes
	void allocate(	TMatrix _matrix, TShader *_shader, 
					std::string _svn, int _uniformType) 
	{
		matrix = std::move(_matrix);
		shader = _shader;
		shaderVariableName = std::move(_svn);
		uniformType = _uniformType;
	}
	/
	//Bind healded matrix to shader
	void Build() {
		if (shader) {
			uniformId = (shader->*NewUniform)(shaderVariableName.c_str(), &matrix, uniformType);
		} else { 
			#ifdef DEBUG_CAMERA
				DEBUG_OUT << "ERROR::MATRIX_HANDLER::Build::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif		
		}
	}

public:
	MatrixHandler() :	matrix(), shader(nullptr), shaderVariableName(CAMERA_STD_SHADER_VARIABLE_NAME), 
						uniformId(-1), uniformType(CAMERA_STD_MATRIX_TYPE) { }

	MatrixHandler(	TMatrix _matrix, TShader* _shader = nullptr, 
					std::string name = std::string(CAMERA_STD_SHADER_VARIABLE_NAME), 
					int matType = CAMERA_STD_MATRIX_TYPE) : matrix(std::move(_matrix)), shader(_shader),
															shaderVariableName(name),	uniformId(-1),
															uniformType(matType) { Build(); }

	MatrixHandler(	TMatrix _matrix, TShader* _shader, const char* name, int matType) : MatrixHandler(_matrix, _shader, std::string(name), matType) { }

	~MatrixHandler() { /*Protect shader from destructor call/	shader = nullptr; }

	//Update content of matrix in shader
	virtual void loadToShader() {
		if (shader) {
			(shader->*UpdateUniform)(&matrix, uniformId);
		} else { 
			#ifdef DEBUG_CAMERA
				DEBUG_OUT << "ERROR::MATRIX_HANDLER::Update::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif		
		}	
	}

	//Update matrix with new value
	void updateMatrix(TMatrix _matrix) { matrix = std::move(_matrix); }
	
};
*/

//Class definition: ProjectionHandler
//class ProjectionHandler : virtual public MatrixHandler<> {
class ProjectionHandler : virtual public MatrixHandler {
public:
	enum ProjectionType : int {
		UNINITIALISED	= 0x00,
		INITIALISED		= 0x01,
		ORTHO			= 0x03,
		PERSPECTIVE		= 0x05,
		BOTH			= 0x07
	};

	enum ProjectionMode {
		MODE_ORTHO,
		MODE_PERSPECTIVE
	};

	struct PerspectiveData {
		float fov		= 0;
		float aspect	= 0;
		float near		= 0;
		float far		= 0;

		PerspectiveData() {};

		PerspectiveData(float _fov,		float _aspect, 
						float _near,	float _far) : 
						fov(_fov),		aspect(_aspect), 
						near(_near),	far(_far) {}
	};

	struct OrthoData {
		float left		= 0;
		float right		= 0;
		float bottom	= 0;
		float top		= 0;
		float near		= 0;
		float far		= 0;

		OrthoData() {};

		OrthoData(	float _left,	float _right, 
					float _bottom,	float _top, 
					float _near,	float _far) :
					left(_left),	right(_right),
					bottom(_bottom), top(_top),
					near(_near),	far(_far) {}

	};
private:
	//State data
	int type;
	ProjectionMode mode;
	//Data for orthographical projection
	float left;
	float right;
	float bottom;
	float top;
	//Data for perspective projection
	float fov;
	float aspect;
	//Clip frustum data
	float near, far;
protected:
	//MatrixHandler::loadToShader;
public:
	
	ProjectionHandler() : type(ProjectionType::UNINITIALISED) {}

	/*
	ProjectionHandler(	float _fov,			float _aspect, 
						float _near,		float _far,
						Shader *_shader, int matType = CAMERA_STD_MATRIX_TYPE) : 
						type(ProjectionType::PERSPECTIVE), 
						mode(ProjectionMode::MODE_PERSPECTIVE),
						fov(_fov),			near(_near),
						aspect(_aspect),	far(_far),
						MatrixHandler(glm::perspective(_fov, _aspect, _near, _far), 
						_shader, std::string(CAMERA_STD_PROJECTION_SHADER_VARIABLE_NAME), matType) { }

	ProjectionHandler(	float _left,		float _right,
						float _bottom,		float _top,
						float _near,		float _far,
						Shader *_shader,	int matType = CAMERA_STD_MATRIX_TYPE) : 
						type(ProjectionType::ORTHO), 
						mode(ProjectionMode::MODE_ORTHO),
						left(_left),		right(_right), 
						bottom(_bottom),	top(_top),
						near(_near),		far(_far),
						MatrixHandler(glm::ortho(_left, _right, _bottom, _top, _near, _far), 
						_shader, std::string(CAMERA_STD_PROJECTION_SHADER_VARIABLE_NAME), matType) { }
	*/

	ProjectionHandler(	float _fov,			float _aspect, 
						float _near,		float _far,
						Shader *_shader) : 
						type(ProjectionType::PERSPECTIVE), 
						mode(ProjectionMode::MODE_PERSPECTIVE),
						fov(_fov),			near(_near),
						aspect(_aspect),	far(_far),
						MatrixHandler(glm::perspective(_fov, _aspect, _near, _far), 
						_shader, std::string(CAMERA_STD_PROJECTION_SHADER_VARIABLE_NAME)) { }

	ProjectionHandler(	float _left,		float _right,
						float _bottom,		float _top,
						float _near,		float _far,
						Shader *_shader) : 
						type(ProjectionType::ORTHO), 
						mode(ProjectionMode::MODE_ORTHO),
						left(_left),		right(_right), 
						bottom(_bottom),	top(_top),
						near(_near),		far(_far),
						MatrixHandler(glm::ortho(_left, _right, _bottom, _top, _near, _far), 
						_shader, std::string(CAMERA_STD_PROJECTION_SHADER_VARIABLE_NAME)) { }

	bool isInitialised() { return (type & ProjectionType::INITIALISED); }
	
	ProjectionType getType() { return (ProjectionType)type; }

	PerspectiveData getPrespectiveData() { return (type & ProjectionType::PERSPECTIVE) ? PerspectiveData(fov, aspect, near, far) : PerspectiveData(); }

	void setPerspectiveData(PerspectiveData& data) {
		aspect = data.aspect;
		near = data.near;
		far = data.far;
		fov = data.fov;
		type |= ProjectionType::PERSPECTIVE;
	}

	OrthoData getOrthoData() { return (type & ProjectionType::ORTHO) ? OrthoData(left, right, bottom, top, near, far) : OrthoData(); }

	void setOrthoData(OrthoData& data) {
		bottom = data.bottom;
		right = data.right;
		left = data.left;
		near = data.near;
		top = data.top;
		far = data.far;
		type |= ProjectionType::ORTHO;
	}

	ProjectionMode getMode() { return mode; }

	void setMode(ProjectionMode _mode) { 
		switch (_mode) {
			case ProjectionMode::MODE_ORTHO:
				if (type & ProjectionType::ORTHO) {
					mode = _mode;
				} else {
					#ifdef DEBUG_CAMERA
						DEBUG_OUT << "ERROR::PROJECTION_HANDLER::setMode" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Can't set mode ORTHO on current state." << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tState: " << type << DEBUG_NEXT_LINE;
					#endif	
				}
				break;
			case ProjectionMode::MODE_PERSPECTIVE:
				if (type & ProjectionType::PERSPECTIVE){
					mode = _mode;
				} else {
					#ifdef DEBUG_CAMERA
						DEBUG_OUT << "ERROR::PROJECTION_HANDLER::setMode" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Can't set mode PERSPECTIVE on current state." << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tState: " << type << DEBUG_NEXT_LINE;
					#endif	
				}
				break;
			default:
				#ifdef DEBUG_CAMERA
					DEBUG_OUT << "ERROR::PROJECTION_HANDLER::setMode::UNRECOGNISABLE_MODE" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMode: "<< _mode << DEBUG_NEXT_LINE;
				#endif	
				break;
		}
	}

	void switchMode() { 
		if (type & ProjectionType::BOTH) {
			mode = (mode == ProjectionMode::MODE_ORTHO) ? ProjectionMode::MODE_PERSPECTIVE : ProjectionMode::MODE_ORTHO;
		} else {
			#ifdef DEBUG_CAMERA
				DEBUG_OUT << "ERROR::PROJECTION_HANDLER::switchMode::CANT_SWITCH_MODE" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	//Loade current mode projection matrix to shader
	void updateProjection() { 
		if (type & ProjectionType::INITIALISED) {
			this->newValue((mode == ProjectionMode::MODE_ORTHO) ? glm::ortho(left, right, bottom, top, near, far) : glm::perspective(fov, aspect, near, far));
		} else {
			#ifdef DEBUG_CAMERA
				DEBUG_OUT << "ERROR::PROJECTION_HANDLER::updateProjection::NOT_INITIALISED" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: Projection patrix isn't initialised." << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tState: " << type << DEBUG_NEXT_LINE;
			#endif	
		}
	}
};


//Class definition: SimpleCamera
class SimpleCamera : public ProjectionHandler {
	glm::vec3 position		= glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 front			= glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up			= glm::vec3(0.0f, 1.0f, 0.0f);
	MatrixHandler view;
public:
	SimpleCamera() {}

	SimpleCamera() {}

};

#endif