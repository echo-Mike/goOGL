#ifndef RHE_H
#define RHE_H "[0.0.5@cResourceHandlingEngine.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of resource handling engine.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <map>
#include <memory>
#include <string>
#include <iostream>
//ASSIMP

//OUR
#include "general\CIndexPool.h"
#include "RHE\vResourceGeneral.h"
#include "RHE\cResource.h"
#include "RHE\cResourceHandler.h"
//DEBUG
#ifdef DEBUG_RHE
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

namespace resources {

	#ifndef RHE_GLOBAL_MAX_RESOURCES
		/**
		*	DESCRIPTION
		**/
		#define RHE_GLOBAL_MAX_RESOURCES ((ResourceID)0xF4240)
	#endif

	#ifndef RHE_ALLOCATION_BANDWIDTH
		/**
		*	DESCRIPTION
		**/
		#define RHE_ALLOCATION_BANDWIDTH ((unsigned int)0x9C4)
	#endif

	/**
	*	Main singletone object of resource handling for all engine.
	*	Class definition: ResourceHandlingEngine
	**/
	class ResourceHandlingEngine : private Resource {
		SmartSimpleIndexPool<ResourceID> indexPool;
		
		typedef std::map<Resource*const, ResourceHandler*> HandlersStorage;
		HandlersStorage handlers;
		
	public:

		ResourceHandlingEngine(	ResourceID _maxId = RHE_GLOBAL_MAX_RESOURCES, 
								unsigned int _bandwidth = RHE_ALLOCATION_BANDWIDTH) : 
								indexPool(1, _maxId, _bandwidth)
		{
			handlers[this] = new ResourceHandler();
			handlers[this]->status = ResourceHandler::PUBLIC;
		}


		template < class T >
		bool newResource(T&& _value, Resource* _owner, std::shared_ptr<T>& _result) {
			try { 
				_result = std::move(handlers.at(_owner)->newResource<T>(std::move(_value), indexPool.newIndex())); 
				return true;
			}
			catch (const std::out_of_range& e) {
				#ifdef DEBUG_RHE
					DEBUG_OUT << "ERROR::RHE::newResource" << DEBUG_NEXT_LINE;
					if (std::string(e.what()).find("SMART_SIMPLE_INDEX_POOL") == std::string::npos) {
						DEBUG_OUT << "\tMessage: Owner not found." << DEBUG_NEXT_LINE;
					} else {
						DEBUG_OUT << "\tMessage: Allocation limit reached." << DEBUG_NEXT_LINE;
					}
				#endif
				return false;
			}
			catch (const std::logic_error& e) {
				#ifdef DEBUG_RHE
					DEBUG_OUT << "ERROR::RHE::newResource" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Error occurred during new object move-constructing." << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tException content:" << e.what() << DEBUG_NEXT_LINE;
				#endif
				return false;
			}
		}

		template < class T >
		bool newResource(T* _valueptr, Resource* _owner, std::shared_ptr<T>& _result) {
			try { 
				_result = std::move(handlers.at(_owner)->newResource<T>(_valueptr, indexPool.newIndex()));
				return true;
			}
			catch (const std::out_of_range& e) {
				#ifdef DEBUG_RHE
					DEBUG_OUT << "ERROR::RHE::newResource" << DEBUG_NEXT_LINE;
					if (std::string(e.what()).find("SMART_SIMPLE_INDEX_POOL") == std::string::npos) {
						DEBUG_OUT << "\tMessage: Owner not found." << DEBUG_NEXT_LINE;
					} else {
						DEBUG_OUT << "\tMessage: Allocation limit reached." << DEBUG_NEXT_LINE;
					}
				#endif
				return false;
			}
			catch (const std::logic_error& e) {
				#ifdef DEBUG_RHE
					DEBUG_OUT << "ERROR::RHE::newResource" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Error occurred during new object move-constructing." << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tException content:" << e.what() << DEBUG_NEXT_LINE;
				#endif
				return false;
			}
		}

		template < class T >
		bool newResource(T&& _value, std::shared_ptr<Resource> _owner, std::shared_ptr<T>& _result) {
			return newResource<T>(std::move(_value), _owner.get(), _result);
		}

		template < class T >
		std::shared_ptr<T> newResource(T* _valueptr, std::shared_ptr<Resource> _owner) {
			return std::move(newResource<T>(_valueptr, _owner.get()));
		}

		template < class T >
		std::shared_ptr<T> setResource(T&& _value, ResourceID _Id, Resource* _owner) {

		}

		template < class T >
		std::shared_ptr<T> setResource(T* _valueptr, ResourceID _Id, Resource* _owner) {

		}

		template < class T >
		std::shared_ptr<T> setResource(T&& _value, ResourceID _Id, std::shared_ptr<Resource> _owner) {
			return std::move(setResource<T>(std::move(_value), _Id, _owner.get()));
		}

		template < class T >
		std::shared_ptr<T> setResource(T* _valueptr, ResourceID _Id, std::shared_ptr<Resource> _owner) {
			return std::move(setResource<T>(_valueptr, _Id, _owner.get()));
		}

		void deleteResource(ResourceID _Id, Resource* _owner) {

		}

		void deleteResource(ResourceID _Id, std::shared_ptr<Resource> _owner) {	deleteResource(_Id, _owner.get()); }
	};
}
#endif