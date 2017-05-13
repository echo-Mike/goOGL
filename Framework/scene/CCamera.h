#ifndef CAMERA_H
#define CAMERA_H "[0.0.4@CCamera.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of camera controller class.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//GLEW
#include <GL/glew.h>
//GLM
#include <GLM/glm.hpp>
#include <GLM/GTC/quaternion.hpp>
//OUR
#include "general/CUniformMatrix.h"
#include "general/ImprovedMath.h"
//DEBUG
#ifdef DEBUG_CAMERA
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#ifndef PROJECTION_HANDLER_STD_PROJECTION_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define PROJECTION_HANDLER_STD_PROJECTION_SHADER_VARIABLE_NAME "projection"
#endif

//Class definition: ProjectionHandler
class ProjectionHandler : public MatrixAutomaticStorage<> {
public:
	enum State : int {
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

		PerspectiveData(PerspectiveData& _data) : 
						fov(_data.fov), aspect(_data.aspect),
						near(_data.near), far(_data.far) {}
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

		OrthoData(	OrthoData& _data) :
					left(_data.left),	right(_data.right),
					bottom(_data.bottom), top(_data.top),
					near(_data.near), far(_data.far) {}

	};
private:
	//State data
	int state;
	ProjectionMode projmode;
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
	//Hide from SimpleCamera and other derived:
	MatrixAutomaticStorage::bindUniform;
	MatrixAutomaticStorage::transposeOnLoad;
	MatrixAutomaticStorage::setValue;
	MatrixAutomaticStorage::value;
	MatrixAutomaticStorage::uniformName;
protected:
	//Hide from other world:
	MatrixAutomaticStorage::push;
	MatrixAutomaticStorage::pull;
	MatrixAutomaticStorage::setName;
	MatrixAutomaticStorage::getName;
public:
	
	ProjectionHandler() : state(State::UNINITIALISED), MatrixAutomaticStorage<>() {
		setName(PROJECTION_HANDLER_STD_PROJECTION_SHADER_VARIABLE_NAME);
	}

	ProjectionHandler(	float _fov,			float _aspect, 
						float _near,		float _far,
						Shader *_shader) : 
						state(State::PERSPECTIVE), 
						projmode(ProjectionMode::MODE_PERSPECTIVE),
						fov(_fov),			near(_near),
						aspect(_aspect),	far(_far),
						MatrixAutomaticStorage(&our::perspective(_fov, _aspect, _near, _far),
						_shader, std::string(PROJECTION_HANDLER_STD_PROJECTION_SHADER_VARIABLE_NAME)) {}

	ProjectionHandler(	float _left,		float _right,
						float _bottom,		float _top,
						float _near,		float _far,
						Shader *_shader) : 
						state(State::ORTHO), 
						projmode(ProjectionMode::MODE_ORTHO),
						left(_left),		right(_right), 
						bottom(_bottom),	top(_top),
						near(_near),		far(_far),
						MatrixAutomaticStorage(&our::ortho(_left, _right, _bottom, _top, _near, _far),
						_shader, std::string(PROJECTION_HANDLER_STD_PROJECTION_SHADER_VARIABLE_NAME)) {}

	//Returns initialisation status
	bool isInitialised() { return (state & State::INITIALISED); }
	
	//Returns curent state
	State getState() { return (State)state; }

	//Returns perpective data if it presented
	PerspectiveData getPrespectiveData() { return (state & State::PERSPECTIVE) ? PerspectiveData(fov, aspect, near, far) : PerspectiveData(); }

	//Setup perspective data by data stucture
	void setPerspectiveData(PerspectiveData& data) {
		aspect = data.aspect;
		near = data.near;
		far = data.far;
		fov = data.fov;
		state |= State::PERSPECTIVE;
	}

	//Setup perspective data by data it self
	void setPerspectiveData(float _fov, float _aspect, float _near, float _far) {
		aspect = _aspect;
		near = _near;
		far = _far;
		fov = _fov;
		state |= State::PERSPECTIVE;
	}

	//Returns ortho data if it presented
	OrthoData getOrthoData() { return (state & State::ORTHO) ? OrthoData(left, right, bottom, top, near, far) : OrthoData(); }

	//Setup ortho data by data stucture
	void setOrthoData(OrthoData& data) {
		bottom = data.bottom;
		right = data.right;
		left = data.left;
		near = data.near;
		top = data.top;
		far = data.far;
		state |= State::ORTHO;
	}

	//Setup ortho data by data it self
	void setOrthoData(float _left, float _right, float _bottom, float _top, float _near, float _far) {
		bottom = _bottom;
		right = _right;
		left = _left;
		near = _near;
		top = _top;
		far = _far;
		state |= State::ORTHO;
	}

	//Returns current projection mode
	ProjectionMode getProjectionMode() { return projmode; }

	//Setup projection mode
	void setProjectionMode(ProjectionMode _mode) {
		switch (_mode) {
			case ProjectionMode::MODE_ORTHO:
				if (state & State::ORTHO) {
					projmode = _mode;
				} else {
					#ifdef DEBUG_CAMERA
						DEBUG_OUT << "ERROR::PROJECTION_HANDLER::setMode" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Can't set mode ORTHO on current state." << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tState: " << state << DEBUG_NEXT_LINE;
					#endif	
				}
				break;
			case ProjectionMode::MODE_PERSPECTIVE:
				if (state & State::PERSPECTIVE){
					projmode = _mode;
				} else {
					#ifdef DEBUG_CAMERA
						DEBUG_OUT << "ERROR::PROJECTION_HANDLER::setMode" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Can't set mode PERSPECTIVE on current state." << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tState: " << state << DEBUG_NEXT_LINE;
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

	//Switch projection mode Ortho<->Perspective
	void switchMode() { 
		if (state & State::BOTH) {
			projmode = (projmode == ProjectionMode::MODE_ORTHO) ? ProjectionMode::MODE_PERSPECTIVE : ProjectionMode::MODE_ORTHO;
		} else {
			#ifdef DEBUG_CAMERA
				DEBUG_OUT << "ERROR::PROJECTION_HANDLER::switchMode::CANT_SWITCH_MODE" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	//Load current mode projection matrix to shader
	void updateProjection() { 
		if (state & State::INITIALISED) {
			setValue((projmode == ProjectionMode::MODE_ORTHO) ? our::ortho(left, right, bottom, top, near, far) : our::perspective(fov, aspect, near, far));
		} else {
			#ifdef DEBUG_CAMERA
				DEBUG_OUT << "ERROR::PROJECTION_HANDLER::updateProjection::NOT_INITIALISED" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: Projection matrix isn't initialised." << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tState: " << state << DEBUG_NEXT_LINE;
			#endif	
		}
	}
};

#ifndef SIMPLE_CAMERA_STD_VIEW_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define SIMPLE_CAMERA_STD_VIEW_SHADER_VARIABLE_NAME "view"
#endif

//Class definition: SimpleCamera
class SimpleCamera : public ProjectionHandler {
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	MatrixAutomaticStorage<> view;
public:
	glm::vec3 position;
	GLfloat speed;
	glm::vec2 sensitivity;
	enum CameraMode : int {
		FPS,
		PLANE
	};
	CameraMode mode;
	bool lockUp;

	SimpleCamera() :	ProjectionHandler(), position(glm::vec3(0.0f, 0.0f, 0.0f)), 
						front(glm::vec3(0.0f, 0.0f, -1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)),
						right(glm::vec3(-1.0f, 0.0f, 0.0f)), view(), mode(CameraMode::FPS), lockUp(false)
	{
		view.setName(SIMPLE_CAMERA_STD_VIEW_SHADER_VARIABLE_NAME);
	}

	//Setup new main shader
	void setShader(Shader *_shader) {
		ProjectionHandler::setShader(_shader);
		view.setShader(_shader);
	}

	/*Setup all parameters for OpenGL draw calls excluding shader
	* setShader need to be called first
	*/
	void Setup() {
		updateProjection();
		view.setValue(our::lookAt(position, position + front, up));
	}

	/*Setup all parameters for OpenGL draw calls including shader
	* No need to call setShader first
	*/
	void Setup(Shader *_shader) {
		updateProjection();
		view.setValue(our::lookAt(position, position + front, up));
		ProjectionHandler::setShader(_shader);
		view.setShader(_shader);
	}

	//Push camera data to _shader handle queue
	void push(Shader *_shader) {
		ProjectionHandler::push(_shader);
		view.push(_shader);
	}

	//Periodic update function
	void Update() {
		view.setValue(our::lookAt(position, position + front, up));
	}

	/* Control camera view by deltaX and deltaY parameters.
	* In FPS mode: deltaX - turn around, deltaY - look up and down
	* In PLANE mode: deltaX - roll angle, deltaY - pitch angle
	*/
	void controlView(GLfloat &deltaTime, float &deltaX, float &deltaY) {
		right = glm::cross(front, up);
		glm::quat temp = glm::angleAxis(sensitivity.y * deltaY * deltaTime, right) * glm::angleAxis(sensitivity.x * deltaX * deltaTime, (mode) ? front : -up);
		front = glm::normalize(temp * front);
		if (!lockUp)
			up = glm::normalize(temp * up);
	}

	//TODO: redo this function by accepting only general direction vector 
	//Control camera movement by 6-way derection
	void controlMovement(GLfloat &deltaTime, bool &forward, bool &backward, bool &left, bool &right, bool &upward, bool &down) {
		GLfloat realSpeed = speed * deltaTime;
		if (forward)
			position += realSpeed * front;
		if (backward)
			position -= realSpeed * front;
		if (left)
			position -= glm::normalize(glm::cross(front, up)) * realSpeed;
		if (right)
			position += glm::normalize(glm::cross(front, up)) * realSpeed;
		if (upward)
			position += realSpeed * up;
		if (down)
			position -= realSpeed * up;
	}
};
#endif