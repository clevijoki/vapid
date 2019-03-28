#pragma once

#include "NetworkUtil.h"
#include <memory>
#include <vector>
#include "Hash.h"

namespace cache {

	struct Server;

	// creates a server instance
	Server* ServerCreate();

	// destroys a server instance
	void ServerDestroy(Server* server);

	// runs the cache server, on an existing listener socker
	void ServerRun(Server* server, SOCKET socket);

	// a deleter so we can manage lifetimes with unique_ptr or other smart ptr
	struct ServerDeleter{ void operator()(Server* s){ ServerDestroy(s); }};

	struct FileHash {

		FileHash() = default;
		FileHash(std::string filename, Hash hash)
		: filename(std::move(filename))
		, hash(std::move(hash))
		{
		}

		std::string filename;
		Hash hash;
	};

	struct FileData : FileHash {

		FileData() = default;
		FileData(std::string filename, Hash hash, std::vector<uint8_t> in_bytes)
		: FileHash(std::move(filename), std::move(hash))
		, bytes(std::move(in_bytes))
		{
		}		

		std::vector<uint8_t> bytes;
	};

	// client API

	enum class RequestResult : int32_t
	{
		CacheHit,
		CacheMiss,
		NetworkError,
	};

	enum class StoreTaskOutputsResult : int32_t
	{
		Success,
		NetworkError,
	};

	Hash CreateExplicitHash(
		const std::string& command_line,
		const std::vector<FileHash>& source_files
	);

	Hash CreateImplicitHash(
		const std::vector<FileHash>& dependencies
	);

	// first clients send the hashes of the current rule info
	// to determine what the dependencies are and what the hashes are
	// if it returns RequestResult::CacheHit then the results are filled out
	// or it returns RequestResult::CacheMiss if the hash was not found

	RequestResult RequestTaskDependencies(
		SOCKET s, 
		const Hash& explicit_hash, // hash of the command line, source filenames, and source contents
		std::vector<std::vector<FileHash>>& out_dependencies // a list of all dependencies lists, so we can see what is in the cache server
	);

	// if we had a cache hit, then we also need to add the contents of our dependencies to the hash
	// to get the full task hash
	RequestResult RequestTaskOutputs(
		SOCKET s, 
		const Hash& explicit_hash, // hash of the command line, source filenames, and source contents
		const Hash& implicit_hash, // hash of the dependency filenames and contents
		std::vector<FileHash>& out_outputs // a list of all of our dependencies, the client must check if these match to ensure a complete match
	);

	// store the results of a task
	StoreTaskOutputsResult StoreTaskOutputs(
		SOCKET s,
		const Hash& explicit_hash, // hash of the command line and source file contents
		const Hash& implicit_hash, // hash of command line, source file contents, and dependency contents
		const std::vector<FileHash>& dependencies, // list of dependencies
		const std::vector<FileData>& outputs // list of output files
	);

	// if the cache hit was successful, and our dependencies all match, we can defer the retrieval of files
	// until that file is actually required for reading.
	RequestResult RequestFile(
		SOCKET s,
		const FileHash& key,
		FileData& out_result
	);
}

namespace std
{
	std::string to_string(cache::RequestResult result);
	std::string to_string(cache::StoreTaskOutputsResult result);	
}