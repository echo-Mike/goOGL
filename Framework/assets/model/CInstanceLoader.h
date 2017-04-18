#ifndef INSTANCELOADER_H
#define INSTANCELOADER_H "[0.0.2@CInstanceLoader.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of model instance loader class 
*		and general model instance interface.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <vector>
//DEBUG
#ifdef DEBUG_INSTANCELOADER
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/* Common interface class for InstanceData declaration.
*  Class abstract definition: InstanceDataInterface
*/
class InstanceDataInterface {
public:
	//Bind instance data to shader
	virtual void bindData() = 0;
};

/* Load controller for instance data.
*  Class definition: MultipleInstanceLoader
*/
class MultipleInstanceLoader {
	//Container of pointers to instance data 
	std::vector<InstanceDataInterface*> instances;
public:
	//Push instance data to instance array
	void pushInstance(InstanceDataInterface* _data) {
		try {
			instances.push_back(_data);
		}
		catch (std::length_error e) {
			#ifdef DEBUG_INSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::pushInstance::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't push more instance data." << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef DEBUG_INSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::pushInstance::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}
	
	//Pop last instance data from instance array
	void popInstance() {
		if (instances.empty()) {
			#ifdef DEBUG_INSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::popInstance::OUT_OF_RANGE" << DEBUG_NEXT_LINE; 
				DEBUG_OUT << "\tCan't pop from empty instance array." << DEBUG_NEXT_LINE;
			#endif
		} else {
			instances.pop_back();
		}
	}

	//Setup instance data for "index" model instance
	void setInstance(int _index, InstanceDataInterface *_data) {
		try {
			instances.at(_index) = _data;
		}
		catch (std::length_error e) {
			#ifdef DEBUG_INSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::setInstance::OUT_OF_RANGE"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tCan't access instance by index: " << _index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tError string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef DEBUG_INSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::setInstance::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	//Remove instances from start index to end index
	void eraseInstance(int start = 0, int end = 0) {
		auto _start = instances.begin(), _end = instances.begin();
		_start += start;
		_end += end;
		instances.erase(_start, _end);
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
			#ifdef DEBUG_INSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::bindInstance::OUT_OF_RANGE" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tIn index: " << index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		catch (...) {
			#ifdef DEBUG_INSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::bindInstance::UNKONWN"<< DEBUG_NEXT_LINE;
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
			#ifdef DEBUG_INSTANCELOADER
				DEBUG_OUT << "ERROR::MULTIPLE_INSTANCE_LOADER::drawInstances::INVALID_COUNT: " << count << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		for (int _index = start_index; _index < start_index + count; _index++)
			this->drawInstance(_index);
	}
};
#endif