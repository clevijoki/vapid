#include "Assert.h"
#include <stdio.h>
#include <thread>
#include <map>

#include "MutexProtectedResource.h"
#include "NetworkUtil.h"
#include "CacheServer.h"
#include "picosha2.h"

namespace std
{
	std::string to_string(cache::RequestResult result)
	{
		switch (result)
		{
			case cache::RequestResult::CacheHit: return "cache::RequestResult::CacheHit";
			case cache::RequestResult::CacheMiss: return "cache::RequestResult::CacheMiss";
			case cache::RequestResult::NetworkError: return "cache::RequestResult::NetworkError";
		}

		return "Unknown cache::RequestResult";
	}

	std::string to_string(cache::StoreTaskOutputsResult result)
	{
		switch (result)
		{
			case cache::StoreTaskOutputsResult::Success: return "cache::StoreTaskOutputsResult::Success";
			case cache::StoreTaskOutputsResult::NetworkError: return "cache::StoreTaskOutputsResult::NetworkError";
		}

		return "Unknown cache::StoreTaskOutputsResult";
	}
}

namespace cache
{
	// the cache has several layers instead of having one hash lookup, because we want to be able to tell
	// exactly what files mismatched so we know why things needed to rebuild
	// so we have nested hash lookups, less efficient, but this information is too valuable to lose.

	struct TaskEntry
	{
		std::vector<FileHash> dependencies;
		std::vector<FileHash> outputs;
	};

	struct RuleCacheEntry
	{
		// this maps a filename hash, based on names only
		std::map<Hash, TaskEntry> implicit_hashes;
	};

	struct Server
	{
		// this maps the command line (rule) to a list of potential matches
		MutexProtectedResource<std::map<Hash, RuleCacheEntry>> explicit_cache;
		MutexProtectedResource<std::map<Hash, FileData>> output_cache;
	};

	Server* ServerCreate()
	{
		return new Server;
	}

	void ServerDestroy(Server* server)
	{
		delete server;
	}

	bool AreFileHashesEqual(const std::vector<FileHash>& lhs, const std::vector<FileHash>& rhs, std::vector<uint32_t>* out_mismatches)
	{
		if (lhs.size() != rhs.size())
			return false;

		for (size_t n = 0, count = lhs.size(); n < count; ++n)
		{
			EXPECT_EQ(lhs[n].filename, rhs[n].filename); // these should be equal, always sort them before sending to the hash server for storage

			if (lhs[n].hash != rhs[n].hash)
			{
				out_mismatches->push_back((uint32_t)n);
			}
		}

		return out_mismatches->empty();
	}

	Hash CreateImplicitHash(const std::vector<FileHash>& dependencies)
	{
		picosha2::hash256_one_by_one hasher;
		for (const FileHash& fh : dependencies)
		{
			hasher.process(fh.filename.begin(), fh.filename.end());
			hasher.process(std::begin(fh.hash.bytes), std::end(fh.hash.bytes));
		}

		hasher.finish();
		Hash hash;
		hasher.get_hash_bytes(hash.bytes, hash.bytes + sizeof(hash.bytes));
		return hash;
	}	


	Hash CreateExplicitHash(const std::string& command_line, const std::vector<FileHash>& source_files)
	{
		picosha2::hash256_one_by_one hasher;
		hasher.process(command_line.begin(), command_line.end());

		for (const FileHash& fh : source_files)
		{
			hasher.process(fh.filename.begin(), fh.filename.end());
			hasher.process(std::begin(fh.hash.bytes), std::end(fh.hash.bytes));
		}

		hasher.finish();		
		Hash hash;
		hasher.get_hash_bytes(hash.bytes, hash.bytes + sizeof(hash.bytes));
		return hash;
	}

	// used to code-gen a bunch of mappings

#define CACHE_SERVER_API_LIST \
	X(RequestTaskDependencies)\
	X(RequestTaskOutputs)\
	X(StoreTaskOutputs)\
	X(RequestFile)\

	enum class ApiTag : uint8_t
	{
	#define X(x) x,
		CACHE_SERVER_API_LIST
	#undef X
	};

	const uint32_t SOURCE_NAMES_CHANGED_INDEX = uint32_t(-1);
	const uint32_t COMMAND_LINE_CHANGED_INDEX = uint32_t(-2);

	void PackFileHashes(SendBuffer& p, const std::vector<FileHash>& hashes)
	{
		// we have already sent the count
		if (hashes.empty())
			return;

		for (const FileHash& fh : hashes)
		{
			p.pack((uint32_t)fh.filename.size(), fh.hash);
			p.pack_buffer(fh.filename.c_str(), fh.filename.size());
		}
	}

	bool ReceiveFileHashes(SOCKET s, uint32_t count, std::vector<FileHash>& out_hashes)
	{
		out_hashes.resize(count);

		if (count == 0)
			return true;

		for (FileHash& fh : out_hashes)
		{
			uint32_t filename_size = 0;
			if (!ReceivePacket(s, &filename_size, &fh.hash))
				return false;

			fh.filename.resize(filename_size);
			if (!ReceiveData(s, &fh.filename[0], filename_size))
				return false;

		}

		return true;
	}

	void PackFileData(SendBuffer& p, const FileData& fd)
	{
		p.pack(uint16_t(fd.filename.size()), uint64_t(fd.bytes.size()), fd.hash);
		p.pack_buffer(fd.filename.c_str(), fd.filename.size());
		p.pack_buffer(fd.bytes.data(), fd.bytes.size());
	}

	bool ReceiveFileData(SOCKET s, FileData& fd)
	{
		uint16_t filename_size;
		uint64_t data_size;
		if (!ReceivePacket(s, &filename_size, &data_size, &fd.hash))
			return false;

		fd.filename.resize(filename_size);
		if (!ReceiveData(s, &fd.filename[0], filename_size))
			return false;

		fd.bytes.resize(data_size);
		if (!ReceiveData(s, fd.bytes.data(), data_size))
			return false;

		return true;
	}

	void PackFileDatas(SendBuffer& p, const std::vector<FileData>& files)
	{
		// we have already sent the count
		if (files.empty())
			return;

		for (const FileData& fh : files)
		{
			PackFileData(p, fh);
		}
	}

	bool ReceiveFileDatas(SOCKET s, uint32_t count, std::vector<FileData>& out_hashes)
	{
		out_hashes.resize(count);

		if (count == 0)
			return true;

		for (FileData& fd : out_hashes)
		{
			ReceiveFileData(s, fd);
		}

		return true;
	}


	RequestResult RequestTaskDependencies(
		SOCKET s, 
		const Hash& explicit_hash, // hash of the command line, source filenames, and source contents
		std::vector<std::vector<FileHash>>& out_dependencies // a list of all of our dependencies, the client must check if these match to ensure a complete match
	)
	{
		{
			SendBuffer p(s);
			p.pack(ApiTag::RequestTaskDependencies, explicit_hash);

			if (!p.flush())
				return RequestResult::NetworkError;
		}

		uint32_t dep_list_count;
		if (!ReceivePacket(s, &dep_list_count))
			return RequestResult::NetworkError;

		out_dependencies.resize(dep_list_count);

		if (dep_list_count == 0)
			return RequestResult::CacheMiss;

		// now receive them all
		for (std::vector<FileHash>& dep_list : out_dependencies)
		{
			uint32_t dep_list_count;
			if (!ReceivePacket(s, &dep_list_count))
				return RequestResult::NetworkError;

			if (!ReceiveFileHashes(s, dep_list_count, dep_list))
				return RequestResult::NetworkError;
		}

		return RequestResult::CacheHit;
	}

	bool HandleRequestTaskDependencies(Server* cache_server, SOCKET s)
	{
		// the server side
		Hash explicit_hash{};

		if (!ReceivePacket(s, &explicit_hash))
			return false;


		SendBuffer p(s);

		{
			auto explicit_cache = cache_server->explicit_cache.lock();

			// now we have to look up in the cache_server to see if we have something with this
			// command line hash and filename hash list
			const auto explicit_cache_itr = explicit_cache->find(explicit_hash);
			if (explicit_cache_itr == explicit_cache->end())
			{
				SendBuffer p(s);
				p.pack(uint32_t(0));
				return p.flush();
			}

			const RuleCacheEntry& rce = explicit_cache_itr->second;

			// if we got here we all match, send the data that was requested

			p.pack(uint32_t(rce.implicit_hashes.size()));

			for (const auto& kv : rce.implicit_hashes)
			{
				p.pack(uint32_t(kv.second.dependencies.size()));
				PackFileHashes(p, kv.second.dependencies);
			}
		}

		return p.flush();
	}

	RequestResult RequestTaskOutputs(
		SOCKET s, 
		const Hash& explicit_hash,
		const Hash& implicit_hash, // hash of the command line, source contents, and dependencies, 
		std::vector<FileHash>& out_outputs) // a list of all of our dependencies, the client must check if these match to ensure a complete match
	{
		out_outputs.clear();
		
		{
			SendBuffer sb(s);

			sb.pack(ApiTag::RequestTaskOutputs, explicit_hash, implicit_hash);

			if (!sb.flush())
				return RequestResult::NetworkError; 
		}

		uint32_t output_count;
		if (!ReceivePacket(s, &output_count))
			return RequestResult::NetworkError;
		
		if (output_count == uint32_t(-1))
			return RequestResult::CacheMiss;

		if (!ReceiveFileHashes(s, output_count, out_outputs))
			return RequestResult::NetworkError;

		return RequestResult::CacheHit;
	}

	bool HandleRequestTaskOutputs(Server* cache_server, SOCKET s)
	{
		Hash explicit_hash;
		Hash implicit_hash;

		if (!ReceivePacket(s, &explicit_hash, &implicit_hash))
			return false;

		SendBuffer p(s);

		// now we have to look up in the cache_server to see if we have something with this
		{
			auto explicit_cache = cache_server->explicit_cache.lock();

			// command line hash and filename hash list
			const auto explicit_cache_itr = explicit_cache->find(explicit_hash);
			if (explicit_cache_itr == explicit_cache->end())
			{
				p.pack(uint32_t(-1));
				return p.flush();
			}

			const RuleCacheEntry& rce = explicit_cache_itr->second;
			const auto implicit_cache_itr = rce.implicit_hashes.find(implicit_hash);
			if (implicit_cache_itr == rce.implicit_hashes.end())
			{
				p.pack(uint32_t(-1));
				return p.flush();
			}

			const TaskEntry& te = implicit_cache_itr->second;
			p.pack(uint32_t(te.outputs.size()));
			PackFileHashes(p, te.outputs);
		}

		return p.flush();
	}

	StoreTaskOutputsResult StoreTaskOutputs(
		SOCKET s,
		const Hash& explicit_hash, // hash of the command line and source file contents
		const Hash& implicit_hash, // hash of command line, source file contents, and dependency contents
		const std::vector<FileHash>& dependencies, // list of dependencies
		const std::vector<FileData>& outputs // list of output files
	)
	{
		SendBuffer p(s);

		p.pack(ApiTag::StoreTaskOutputs, explicit_hash, implicit_hash, uint32_t(dependencies.size()), uint32_t(outputs.size()));

		PackFileHashes(p, dependencies);
		PackFileDatas(p, outputs);

		if (!p.flush())
			return StoreTaskOutputsResult::NetworkError;

		return StoreTaskOutputsResult::Success;
	}

	bool HandleStoreTaskOutputs(Server* cache_server, SOCKET s)
	{
		Hash explicit_hash;
		Hash implicit_hash;
		uint32_t dependencies_size;
		uint32_t outputs_size;

		if (!ReceivePacket(s, &explicit_hash, &implicit_hash, &dependencies_size, &outputs_size))
			return false;

		std::vector<FileHash> dependencies(dependencies_size);
		std::vector<FileData> outputs(outputs_size);

		if (!ReceiveFileHashes(s, dependencies_size, dependencies))
			return false;
		
		if (!ReceiveFileDatas(s, outputs_size, outputs))
			return false;

		{
			auto explicit_cache = cache_server->explicit_cache.lock();

			RuleCacheEntry& rce = (*explicit_cache)[explicit_hash];
			TaskEntry& te = rce.implicit_hashes[implicit_hash];

			te.dependencies = std::move(dependencies);
			te.outputs.reserve(outputs_size);

			auto output_cache = cache_server->output_cache.lock();

			for (FileData& fd : outputs)
			{
				te.outputs.push_back(fd);

				(*output_cache)[fd.hash] = std::move(fd);
			}
		}

		return true;
	}

	RequestResult RequestFile(
		SOCKET s,
		const FileHash& key,
		FileData& out_result
	)
	{
		{
			SendBuffer p(s);
			p.pack(ApiTag::RequestFile, key.hash);
			if (!p.flush())
				return RequestResult::NetworkError;
		}

		RequestResult result;
		if (!ReceiveData(s, &result, sizeof(result)))
			return RequestResult::NetworkError;

		if (result != RequestResult::CacheHit)
			return result;

		if (!ReceiveFileData(s, out_result))
			return RequestResult::NetworkError;

		return RequestResult::CacheHit;
	}

	bool HandleRequestFile(Server* cache_server, SOCKET s)
	{
		Hash hash;
		if (!ReceivePacket(s, &hash))
			return false;

		SendBuffer p(s);

		{
			auto output_cache = cache_server->output_cache.lock();

			auto output_cache_itr = output_cache->find(hash);
			if (output_cache_itr == output_cache->end())
			{
				p.pack(RequestResult::CacheMiss);
				return p.flush();
			}

			p.pack(RequestResult::CacheHit);
			PackFileData(p, output_cache_itr->second);
		}

		return p.flush();
	}

	// process API tags to figure out what we need to run
	void ServerRun(Server* server, SOCKET listener_socket)
	{
		RunServerListener("Cache Server", listener_socket, [&](SOCKET client_connection, const SOCKADDR* client_addr){

			std::string connection_name = SockAddrToString(client_addr);

			while (true)
			{
				ApiTag tag;

				if (!ReceivePacket(client_connection, &tag))
				{
					return;
				}

				switch (tag)
				{
				#define X(x) case ApiTag::##x: {\
					DebugLog("Cache Server (%s): " #x "\n", connection_name.c_str());\
					if (!Handle##x(server, client_connection)) return;  \
					break; }\

					CACHE_SERVER_API_LIST
				#undef X

				default:
					DebugLog("Cache Server (%s): Unknown cache::ApiTag %d\n", connection_name.c_str(), (int)tag);
					return;
				}
			}

		});
	}

}

