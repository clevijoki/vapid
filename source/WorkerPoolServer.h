#pragma once
#include "NetworkUtil.h"

namespace worker_pool
{
	enum class Result : int32_t
	{
		Success,
		NetworkError,
		NoWorkers,
	};

	struct Server;
	Server* ServerCreate();
	void ServerDestroy(Server* server);
	size_t ServerGetWorkerCount(Server* server);
	void ServerRun(Server* server, SOCKET listener_socket);

	// register a worker in the worker list, disconnect to clean it up
	Result RegisterWorker(SOCKET s, const char* address, const uint16_t port);

	// removes ourself from the worker pool (perhaps we're busy?)
	Result UnregisterWorker(SOCKET s);

	// this is for clients to request workers from the worker pool
	// will tell the available worker to connect to this address/port
	Result RequestWorker(SOCKET s, const char* address, const uint16_t port);
}

namespace std
{
	std::string to_string(worker_pool::Result result);	
}
