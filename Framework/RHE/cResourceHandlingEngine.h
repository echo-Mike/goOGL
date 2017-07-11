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
//OUR
#include "general\vs2013tweaks.h"
#include "general\mConcepts.hpp"
#include "general\CIndexPool.h"
#include "RHE\vResourceGeneral.h"
#include "RHE\cResource.h"
#include "RHE\cResourceHandler.h"
//DEBUG
#if defined(DEBUG_RHE) && !defined(OTHER_DEBUG)
	#include "general\mDebug.h"		
#elif defined(DEBUG_RHE) && defined(OTHER_DEBUG)
	#include OTHER_DEBUG
#endif

namespace resources {

	/**
	*	Main singletone object of resource handling for all engine.
	*	Class definition: ResourceHandlingEngine
	**/
	class ResourceHandlingEngine : private Resource {

		using HandlersStorage = std::map<Resource*const, std::unique_ptr<ResourceHandler>>;

		HandlersStorage handlers;
		
		SmartSimpleIndexPool<ResourceID> indexPool;
	public:

		ResourceHandlingEngine() = delete;

		ResourceHandlingEngine(	ResourceID _maxId = RHE_GLOBAL_MAX_RESOURCES, 
								unsigned int _bandwidth = RHE_ALLOCATION_BANDWIDTH) : indexPool(1, _maxId, _bandwidth)
		{
			handlers[this] = std::make_unique<ResourceHandler>(ResourceHandler::ResourceHandlerStatus::PUBLIC, this);
		}

		~ResourceHandlingEngine() = default;

		ResourceHandlingEngine(const ResourceHandlingEngine&) = delete;

		ResourceHandlingEngine& operator= (const ResourceHandlingEngine&) = delete;

		ResourceHandlingEngine(ResourceHandlingEngine&& other)  NOEXCEPT :  status(std::move(other.status))
		{
			other.owner = nullptr;
		}

		ResourceHandlingEngine& operator= (ResourceHandlingEngine&& other) NOEXCEPT
		{
			owner = other.owner;
			other.owner = nullptr;
			status = std::move(other.status);
			Base::operator=(std::move(other));
			return *this;
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

		void secureRemove(const ResourceID _Id, ResourceHandler* const _owner) NOEXCEPT {
			for (const auto& v : handlers) {
				if (v.second.get() == _owner) {
					indexPool.deleteIndex(_Id);
					return;
				}
			}
		}
	};
}
#endif