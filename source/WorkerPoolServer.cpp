#include "WorkerPoolServer.h"
#include "MutexProtectedResource.h"
#include <vector>

namespace std
{
	std::string to_string(worker_pool::Result result)
	{
		switch (result)
		{
			case worker_pool::Result::Success: return "worker_pool::Result::Success";
			case worker_pool::Result::NetworkError: return "worker_pool::Result::NetworkError";
			case worker_pool::Result::NoWorkers: return "worker_pool::Result::NoWorkers";
		}

		return "Unknown worker_pool::Result";
	}
}

namespace worker_pool
{
	struct WorkerEntry
	{
		bool is_enabled = false;

		// the address of the connection we are bound to
		char address[INET6_ADDRSTRLEN] = {0};
		// the service or port we are connected to
		uint16_t port = {0};
	};

	void WorkerEntryPack(SendBuffer& p, const char* address, uint16_t port)
	{
		const uint32_t address_len = (uint32_t)strlen(address);

		p.pack(address_len, port);
		p.pack_buffer(address, address_len);
	};


	bool WorkerEntryReceive(SOCKET s, WorkerEntry* out)
	{
		uint32_t address_len;
		if (!ReceivePacket(s, &address_len, &out->port))
			return false;

		if (address_len >= sizeof(out->address))
			return false;

		if (!ReceiveData(s, out->address, address_len))
			return false;

		out->address[address_len] = 0;

		return true;	
	}

	struct Server
	{
		MutexProtectedResource<std::vector<WorkerEntry*>> workers;
	};

	Server* ServerCreate()
	{
		return new Server;
	}

	void ServerDestroy(Server* server)
	{
		delete server;
	}

	size_t ServerGetWorkerCount(Server* server)
	{
		return server->workers.lock()->size();
	}

#define WORKER_POOL_SERVER_API_LIST \
	X(RegisterWorker)\
	X(UnregisterWorker)\
	X(RequestWorker)\

	enum class ApiTag : uint8_t
	{
	#define X(x) x,
		WORKER_POOL_SERVER_API_LIST
	#undef X
	};


	Result RegisterWorker(SOCKET s, const char* address, const uint16_t port)
	{
		SendBuffer p(s);
		p.pack(ApiTag::RegisterWorker);

		WorkerEntryPack(p, address, port);

		return p.flush() ? Result::Success : Result::NetworkError;
	}

	bool HandleRegisterWorker(const std::string& connection_name, Server* server, SOCKET s, WorkerEntry* worker_entry)
	{
		uint32_t address_len;
		if (!ReceivePacket(s, &address_len, &worker_entry->port))
			return false;

		if (address_len >= sizeof(worker_entry->address))
			return false;

		if (!ReceiveData(s, worker_entry->address, address_len))
			return false;

		worker_entry->address[address_len] = 0;
		worker_entry->is_enabled = true;

		DebugLog("Cache Server(%s): Added address '%s' port '%d' to worker list\n", connection_name.c_str(), worker_entry->address, (int)worker_entry->port);
		server->workers.lock()->push_back(worker_entry);

		return true;
	}

	Result UnregisterWorker(SOCKET s)
	{
		SendBuffer p(s);
		p.pack(ApiTag::UnregisterWorker);

		return p.flush() ? Result::Success : Result::NetworkError;
	}

	void UnregisterWorkerInternal(const std::string& connection_name, Server* server, WorkerEntry* worker_entry)
	{
		auto workers = server->workers.lock();

		workers->erase(std::remove(workers->begin(), workers->end(), worker_entry), workers->end());
		DebugLog("Worker Pool (%s): Removed '%s' port '%d' from worker list via disconnection\n", connection_name.c_str(), worker_entry->address, (int)worker_entry->port);
		worker_entry->is_enabled = false;
	}

	bool HandleUnregisterWorker(const std::string& connection_name, Server* server, SOCKET s, WorkerEntry* worker_entry)
	{
		uint32_t address_len;
		if (!ReceivePacket(s, &address_len, &worker_entry->port))
			return false;

		if (address_len >= sizeof(worker_entry->address))
			return false;

		if (!ReceiveData(s, worker_entry->address, address_len))
			return false;

		worker_entry->address[address_len] = 0;

		DebugLog("Cache Server(%s): Added address '%s' port '%d' to worker list\n", connection_name.c_str(), worker_entry->address, (int)worker_entry->port);
		server->workers.lock()->push_back(worker_entry);

		return true;
	}

	Result RequestWorker(SOCKET s, const char* address, const uint16_t port)
	{
		SendBuffer p(s);
		p.pack(ApiTag::RequestWorker);

		WorkerEntryPack(p, address, port);

		if (!p.flush())
			return Result::NetworkError;

		// wait for a result
		uint8_t result;
		if (!ReceivePacket(s, &result))
			return Result::NetworkError;


		return result ? Result::Success : Result::NoWorkers;
	}

	bool HandleRequestWorker(const std::string& connection_name, Server* server, SOCKET s, WorkerEntry* worker_entry)
	{

		return true;
	}

	// process API tags to figure out what we need to run
	void ServerRun(Server* server, SOCKET listener_socket)
	{
		RunServerListener("Worker Pool", listener_socket, [&](SOCKET client_connection, const SOCKADDR* client_addr){

			std::string connection_name = SockAddrToString(client_addr);

			bool is_in_worker_list = false;

			WorkerEntry worker_entry;

			bool running = true;

			while (running)
			{
				ApiTag tag;

				if (!ReceivePacket(client_connection, &tag))
				{
					running = false;
					break;
				}

				switch (tag)
				{
				#define X(x) case ApiTag::##x: {\
					DebugLog("Worker Pool Server (%s): " #x "\n", connection_name.c_str());\
					running = Handle##x(connection_name, server, client_connection, &worker_entry);  \
					break; }\

					WORKER_POOL_SERVER_API_LIST
				#undef X

				default:
					DebugLog("Worker Pool (%s): Unknown cache::ApiTag %d\n", connection_name.c_str(), (int)tag);
					running = false;
					break;
				}
			}

			if (worker_entry.is_enabled)
			{
				UnregisterWorkerInternal(connection_name, server, &worker_entry);
			}
		});
	}

}