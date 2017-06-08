#ifndef MESHINSTANCELOADER_H
#define MESHINSTANCELOADER_H "[0.0.5@CMeshInstanceLoader.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of mesh instance loader class 
*		and general mesh instance interface.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <vector>
#include <type_traits>
//OUR
#include "assets\shader\CShader.h"
//DEBUG
#ifdef DEBUG_MESHINSTANCELOADER
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/* Common interface class for mesh instance data declaration.
*  Class abstract template definition: MeshInstanceDataInterface
*/
template< class TShader = Shader >
class MeshInstanceDataInterface {
public:
	//Bind instance data to shader
	virtual void bindData() = 0;
	//Set shader of instance
	virtual void setShader(TShader *_shader) = 0;
	//RTTI
	virtual ~MeshInstanceDataInterface() {}
};

/* Load controller for mesh instance data.
*  Class definition: MultipleMeshInstanceLoader
*/
template < class TShader = Shader >
class MultipleMeshInstanceLoader {
public:
	//The base interface class of handled instances
	typedef MeshInstanceDataInterface<TShader> InstanceInterface;

	//The type of instance container used
	typedef std::vector<InstanceInterface*> InstanceContainer;

	//Container of instance data
	InstanceContainer instances;

	//Push instance data to instance array
	void pushInstance(InstanceInterface* _data) {
		try {
			instances.push_back(_data);
		}
		catch (std::length_error e) {
			#ifdef DEBUG_MESHINSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_MESH_INSTANCE_LOADER::pushInstance::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't push more instance data." << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef DEBUG_MESHINSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_MESH_INSTANCE_LOADER::pushInstance::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}
	
	//Pop last instance data from instance array
	void popInstance() {
		if (instances.empty()) {
			#ifdef DEBUG_MESHINSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_MESH_INSTANCE_LOADER::popInstance::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tMessege: Can't pop from empty instance array." << DEBUG_NEXT_LINE;
			#endif
		} else {
			instances.pop_back();
		}
	}

	//Create new instance of type T by move-constructing from pointer
	template < class T >
	void newInstance(T* _valueptr) {
		if (std::is_base_of<InstanceInterface, T>::value) {
			if (std::is_copy_constructible<T>::value) {
				instances.push_back(nullptr);
				instances.back() = new T(std::move(*_valueptr));
			} else {
				#ifdef DEBUG_MESHINSTANCELOADER
					DEBUG_OUT << "ERROR::MULTIPLE_MESH_INSTANCE_LOADER::newInstance::INVALID_TYPE" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessege: Type T isn't copy constructible." << DEBUG_NEXT_LINE;
				#endif
			}
		} else {
			#ifdef DEBUG_MESHINSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_MESH_INSTANCE_LOADER::newInstance::INVALID_TYPE" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessege: InstanceDataInterface must be the base class of T." << DEBUG_NEXT_LINE;
			#endif
		}
	}

	//Create new instance of type T by move-constructing
	template < class T >
	void newInstance(T&& _value) {
		if (std::is_base_of<InstanceInterface, T>::value) {
			if (std::is_copy_constructible<T>::value) {
				instances.push_back(nullptr);
				instances.back() = new T(std::move(_value));
			} else {
				#ifdef DEBUG_MESHINSTANCELOADER
					DEBUG_OUT << "ERROR::MULTIPLE_MESH_INSTANCE_LOADER::newInstance::INVALID_TYPE" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessege: Type T isn't copy constructible." << DEBUG_NEXT_LINE;
				#endif
			}
		} else {
			#ifdef DEBUG_MESHINSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_MESH_INSTANCE_LOADER::newInstance::INVALID_TYPE" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessege: InstanceDataInterface must be the base class of T." << DEBUG_NEXT_LINE;
			#endif
		}
	}

	InstanceInterface* operator[] (const int _index) {
		return instances.at(_index);
	}

protected:
	/* Load instance data
	*  Instance data expected to be derived from InstanceDataInterface
	*/
	void bindInstance(int index) {
		try {
			instances.at(index)->bindData();
		}
		catch (std::out_of_range e) {
			#ifdef DEBUG_MESHINSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_MESH_INSTANCE_LOADER::bindInstance::OUT_OF_RANGE" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tIn index: " << index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		catch (...) {
			#ifdef DEBUG_MESHINSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_MESH_INSTANCE_LOADER::bindInstance::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
			throw;
		}
	}
public:

	/*
	* Practically now the first thing i don't understand is 
	* "Why on Earth i created this set of virtual functions?"
	* The second is "How compiler distinguish calls between 
	* MultipleInstanceLoader::drawInstances() and SimpleModel::drawInstances() of same object?"
	* This functions must be total abstract stuff, but object of this class must be constructible.
	* This section must be redone properly.
	*/

	//Draw one instance of model using instance data
	virtual void drawInstance(int index) { return; }

	//Draw multiple instances of model using their data
	virtual void drawInstances(int start_index = 0, int count = 1) {
		if (count < 1) {
			#ifdef DEBUG_MESHINSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_MESH_INSTANCE_LOADER::drawInstances::INVALID_COUNT: " << count << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		for (int _index = start_index; _index < start_index + count; _index++)
			this->drawInstance(_index);
	}

	//Set shader for all handled instances
	virtual void setShader(TShader *_shader) {
		for (auto &_value : instances)
			_value->setShader(_shader);
	}
};
#endif