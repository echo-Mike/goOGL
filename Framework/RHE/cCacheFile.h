#ifndef CACHEFILE_H
#define CACHEFILE_H "[0.0.5@cCacheFile.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of cache file handler calss.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <map>
#include <memory>
#include <iostream>
#include <fstream>
#include <cstdio>
//ASSIMP

//OUR
#include "RHE\vResourceGeneral.h"
//DEBUG
#ifdef DEBUG_CACHEFILE
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

namespace resources {

	#ifndef RHE_CACHE_FILE_SIZE_LIMIT
		/**
		*	Cache file size limit, 250MB by default.
		**/
		#define	RHE_CACHE_FILE_SIZE_LIMIT ((unsigned int)0xFA00000)
	#endif

	/**
	*	Represents temporary cache file for storing resources.
	*	Work cycle: 
	*	CacheFile() -> [ Open(mode) -> getStream() -> 
	*	-> [ registerResource() -> I/O operation (-> unregisterResource()) ] ->
	*	-> Close() ] -> ~CacheFile()
	*	Class definition: CacheFile
	**/
	class CacheFile {
	public:
		//Cached resource information storage: map(Id,size)
		typedef std::map<ResourceID, unsigned int> CachedResourceData;
		//Unsigned integral type for sizes
		typedef unsigned int CachedSize;
	private:
		//Pointer to file stream
		std::shared_ptr<std::fstream> stream;
		//State of cache file
		int state;
		//Path to cache file
		std::string filePath;
		//Map between Ids and sizes
		CachedResourceData resources;
		//Total size of current file content
		CachedSize size;
	public:
		//Cache file state masks
		enum CacheFileState : int {
			CLOSED		= 0x0,
			OPEND		= 0x1,
			READ		= 0x2,
			OPENTOREAD	= 0x3,
			WRITE		= 0x4,
			OPENTOWRITE	= 0x5,
			READORWRITE	= 0x6,
			CREATED		= 0x8,
			INVALID		= 0x10
		};
		
		CacheFile() : state(CacheFileState::CLOSED), size(0)
		{ 
			//Get temp file name
			filePath = std::tmpnam(nullptr); 
		}

		//Recover constructor
		CacheFile(	std::string&& _fileName,
					CachedResourceData&& _resourceData,
					CachedSize _totalSize) :
					state(CacheFileState::CLOSED), 
					size(_totalSize),
					filePath(std::move(_fileName)),
					resources(std::move(_resourceData)) {}

		~CacheFile() {
			//Close filestream if it currently opend
			if (state & CacheFileState::OPEND && !(state & CacheFileState::INVALID))
				stream->close();
			//Remove cache file from disk if it was created
			if (state & CacheFileState::CREATED) {
				if (!std::remove(filePath.c_str())) {
					//Report to debug log if error occurred during removal
					#ifdef DEBUG_CACHEFILE
						DEBUG_OUT << "ERROR::CACHE_FILE::~CacheFile" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Error occurred during tmp file removal." << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tFile path: " << filePath << DEBUG_NEXT_LINE;
					#endif
				}
			}
		}

		/**
		*	Open or create and open a filestream, 
		*	setup file position dependent on '_mode'.
		*	Return: 
		*	---- true - if no error occurred during open/creation process;
		*	---- false - else.
		*	Best end state : CacheFileState::CREATED | CacheFileState::OPEND | (_mode & CacheFileState::READORWRITE)
		*	Worst end state: (state | CacheFileState::INVALID) & ~CacheFileState::CREATED
		*	Locks on CacheFileState::INVALID
		**/
		bool Open(int _mode) {
			//Lock function if state is invalid
			if (state & CacheFileState::INVALID)
				return false;
			//If stream was not created -> create it
			if (!(state & CacheFileState::CREATED)) {
				stream.reset(new std::fstream(filePath));
				state |= CacheFileState::CREATED;
				if (stream->fail()) {
					//Report about failed initial open operation
					#ifdef DEBUG_CACHEFILE
						DEBUG_OUT << "ERROR::CACHE_FILE::Open" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Error occurred during tmp file initial opening." << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tFile path: " << filePath << DEBUG_NEXT_LINE;
					#endif
					state &= ~CacheFileState::CREATED;
					state |= CacheFileState::INVALID;
					return false;
				} else {
					state |= CacheFileState::OPEND;
				}
			}
			//Check if stream is not opend
			if (state & CacheFileState::CREATED && !(state & CacheFileState::OPEND)) {
				stream->open(filePath);
				if (stream->fail()) {
					//Report about failed open operation
					#ifdef DEBUG_CACHEFILE
						DEBUG_OUT << "ERROR::CACHE_FILE::Open" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Error occurred during tmp file opening." << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tFile path: " << filePath << DEBUG_NEXT_LINE;
					#endif
					state &= ~CacheFileState::CREATED;
					state |= CacheFileState::INVALID;
					return false;
				} else {
					state |= CacheFileState::OPEND;
				}
			}
			//Check stream direction and setup file position
			if (_mode & CacheFileState::WRITE) {
				state |= CacheFileState::WRITE;
				state &= ~CacheFileState::READ;
				stream->seekg(0,std::ios_base::end);
			} else if (_mode & CacheFileState::READ) {
				state |= CacheFileState::READ;
				state &= ~CacheFileState::WRITE;
				stream->seekp(0,std::ios_base::beg);
			}
			if (stream->fail()) {
				//Report about failed seek operation
				#ifdef DEBUG_CACHEFILE
					DEBUG_OUT << "ERROR::CACHE_FILE::Open" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Error occurred during seeking." << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tFile path: " << filePath << DEBUG_NEXT_LINE;
				#endif
				state &= ~CacheFileState::CREATED;
				state |= CacheFileState::INVALID;
				return false;
			}
			stream->clear();
			//Now stream is ready to be used
			return true;
		}

		/**
		*	Closes file stream, check for IO error happend during previous operations.
		*	Return:
		*	---- true - if no error occurred during closure process and previous IO operations;
		*	---- false - else.
		*	Best end state : unchanged
		*	Worst end state: (state | CacheFileState::INVALID) & ~CacheFileState::CREATED
		*	Locks on CacheFileState::INVALID
		**/
		bool Close() {
			//Lock function if state is invalid
			if (state & CacheFileState::INVALID)
				return false;
			//Check if during last operations on stream error has occurred
			if (stream->fail()) {
				//Msg to log
				#ifdef DEBUG_CACHEFILE
					DEBUG_OUT << "ERROR::CACHE_FILE::Close" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Error occurred during last stream operation." << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tFile path: " << filePath << DEBUG_NEXT_LINE;
				#endif
				//Try to reboot stream
				stream->clear();
				stream->close();
				if (stream->fail())	{
					#ifdef DEBUG_CACHEFILE
						DEBUG_OUT << "ERROR::CACHE_FILE::getStream" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Error occurred during stream closure on rebooting, stream marked as invalid." << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tFile path: " << filePath << DEBUG_NEXT_LINE;
					#endif
					state &= ~CacheFileState::CREATED;
					state |= CacheFileState::INVALID;
					return false;
				}
				//If stream still fails then mark it as invalid and print msg
				if (!Open(state & CacheFileState::READORWRITE)) {
					#ifdef DEBUG_CACHEFILE
						DEBUG_OUT << "ERROR::CACHE_FILE::getStream" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Error occurred during stream opening on rebooting, stream marked as invalid." << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tFile path: " << filePath << DEBUG_NEXT_LINE;
					#endif
					state &= ~CacheFileState::CREATED;
					state |= CacheFileState::INVALID;
					return false;
				}
			}
			//If no errors has occurred during last io operations then try to close stream
			stream->close();
			if (stream->fail())	{
				#ifdef DEBUG_CACHEFILE
					DEBUG_OUT << "ERROR::CACHE_FILE::getStream" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Error occurred during stream closure, stream marked as invalid." << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tFile path: " << filePath << DEBUG_NEXT_LINE;
				#endif
				state &= ~CacheFileState::CREATED;
				state |= CacheFileState::INVALID;
				return false;
			}
			return true;
		}

		/**
		*	Preforms an attempt to obtain shared_ptr to handled file stream.
		*	If stream wasn't previously created or opend then creates/opens it.
		*	Return:
		*	---- std::shared_ptr<std::fstream>(stream) - if no error occurred;
		*	---- std::shared_ptr<std::fstream>(nullptr) - else.
		*	Best end state : state | CacheFileState::OPEND || state | CacheFileState::OPEND | CacheFileState::CREATED
		*	Worst end state: (state | CacheFileState::INVALID) & ~CacheFileState::CREATED
		*	Locks on CacheFileState::INVALID
		**/
		std::shared_ptr<std::fstream> getStream(CacheFileState _mode = CacheFileState::WRITE) {
			//Lock function if state is invalid
			if (state & CacheFileState::INVALID)
				return std::shared_ptr<std::fstream>(nullptr);
			//If stream not created or not opend then create/open it
			if (!(state & CacheFileState::CREATED) || !(state & CacheFileState::OPEND)) {
				//If Open doesn't succeed then stream is invalid
				if (!Open(_mode)) {
					#ifdef DEBUG_CACHEFILE
						DEBUG_OUT << "ERROR::CACHE_FILE::getStream" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Error occurred during stream boot, stream marked as invalid." << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tFile path: " << filePath << DEBUG_NEXT_LINE;
					#endif
					state &= ~CacheFileState::CREATED;
					state |= CacheFileState::INVALID;
					return std::shared_ptr<std::fstream>(nullptr);
				}
			}
			return std::move(std::shared_ptr<std::fstream>(stream)); 
		}

		/**
		*	Returns current chache file state.
		**/
		int getState() { return state; }

		/**
		*	Moves resource data from invalid stream to provided storage.
		*	Return: true on success, false - else.
		*	Unlocks on CacheFileState::INVALID
		**/
		bool Recover(std::string& _filePath, CachedResourceData& _resorceData, CachedSize& _totalSize) {
			if (state & CacheFileState::INVALID) {
				_filePath = filePath;
				_resorceData = std::move(resources);
				_totalSize = std::move(size);
				return true;
			}
			return false;
		}

		/**
		*	Moves resource data from invalid stream to valid one.
		*	other.state must be CacheFileState::CLOSED
		*	Return: true on success, false - else.
		*	Unlocks on CacheFileState::INVALID
		**/
		bool Recover(CacheFile& other) {
			if (state & CacheFileState::INVALID && !other.state) {
				other.filePath = std::move(filePath);
				other.resources = std::move(resources);
				other.size = std::move(size);
				return true;
			}
			return false;
		}

		/**
		*	Preforms an attempt to register new resource record.
		*	Return: true on success, false - else.
		*	Locks on CacheFileState::INVALID
		**/
		bool registerResource(ResourceID _Id, unsigned int _size) {
			//Lock function if state is invalid
			if (state & CacheFileState::INVALID)
				return false;
			if (resources.count(_Id))
				return false;
			resources[_Id] = _size;
			size += _size;
			return true;
		}

		/**
		*	Preforms an attempt to unregister resource record.
		*	Return: true on success, false - else.
		*	Locks on CacheFileState::INVALID
		**/
		bool unregisterResource(ResourceID _Id) {
			//Lock function if state is invalid
			if (state & CacheFileState::INVALID)
				return false;
			if (!resources.count(_Id))
				return false;
			size -= resources[_Id];
			resources.erase(_Id);
			return true;
		}

		/**
		*	Return true if resource was registered.
		**/
		bool isRegistred(ResourceID _Id) { return resources.count(_Id); }

		/**
		*	Return current summed cached resource size in bytes.
		**/
		unsigned int getCurrentSize() { return size; }

		/**
		*	Returns cached size of specified resource in bytes if it presented, else returns zero.
		**/
		unsigned int getResourceSize(ResourceID _Id) { return resources.count(_Id) ? resources[_Id] : 0; }

		/**
		*	Return current free space size in bytes.
		**/
		int getFreeSize() { return (long int)RHE_CACHE_FILE_SIZE_LIMIT - (long int)size; }

		/**
		*	Return count of currently handled resources.
		**/
		unsigned int holdedResourcesCount() { return resources.size(); }
	};
}
#endif