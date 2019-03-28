#pragma once

#include <memory>
#include "AtScopeExit.h"
#include "MutexProtectedResource.h"
#include "BufOps.h"
#include <algorithm>
#include <thread>

const constexpr uint16_t DEFAULT_CACHE_SERVER_PORT = 27016;
const constexpr char DEFAULT_CACHE_SERVER_PORT_STR[] = "27016";

inline void CloseSocketDeleter(SOCKET& s)
{
	closesocket(s);
};

inline bool SendData(SOCKET s, const void* data, size_t size)
{
	size_t remaining = size;

	do
	{
		int sent_bytes = send(s, (const char*)data, size, 0);
		
		if (sent_bytes == SOCKET_ERROR)
			return false;

		remaining -= sent_bytes;
		data = (uint8_t*)data + sent_bytes;
	}
	while (remaining > 0);

	return true;
}

inline bool ReceiveData(SOCKET s, void* data, size_t size)
{
	size_t remaining = size;

	do
	{
		int received_bytes = recv(s, (char*)data, size, 0);

		if (received_bytes == 0)
			return false; // connection closed

		if (received_bytes == SOCKET_ERROR)
		{
			//DebugLog("recv failed: %s\n", Win32ErrorToString(WSAGetLastError()).c_str());

			return false;
		}
		else
		{
			remaining -= received_bytes;
			data = (uint8_t*)data + received_bytes;	
		}
	}
	while (remaining > 0);

	return true;
}

// this is a fast local buffered packet sender
struct SendBuffer
{
	uint8_t buffer[1500];
	size_t offset = 0;
	SOCKET sock;

	SendBuffer(SOCKET s)
	: sock(s)
	{

	}

	~SendBuffer()
	{
		flush();
	}

	void pack_buffer(const void* data, size_t size)
	{
		const uint8_t* ptr = (const uint8_t*)data;
		const uint8_t* end = ptr + size;

		while (ptr < end)
		{
			const size_t copy_size = std::min<size_t>(sizeof(buffer)-offset, end-ptr);
			memcpy(buffer + offset, ptr, copy_size);
			ptr += copy_size;
			offset += copy_size;

			if (offset == sizeof(buffer))
				flush();
		}
	}

	void pack() {}
	template<typename T, typename... Args>
	void pack(const T& t, Args... args)
	{
		pack_buffer(&t, sizeof(T));
		pack(args...);
	}

	bool flush()
	{
		if (offset != 0 && sock != INVALID_SOCKET && !SendData(sock, buffer, offset))
		{
			sock = INVALID_SOCKET;
		}

		offset = 0;

		return sock != INVALID_SOCKET;
	}
};



template<typename... Args>
bool SendPacket(SOCKET s, Args... args)
{
	char buffer[bufops::PackedSize<Args...>::Result];
	bufops::Write(buffer, args...);
	return SendData(s, buffer, sizeof(buffer));
}


template<typename... Args>
bool ReceivePacket(SOCKET s, Args... args)
{
	char buffer[bufops::PackedPtrSize<Args...>::Result];
	if (!ReceiveData(s, buffer, sizeof(buffer)))
		return false;

	char* ptr = buffer;
	bufops::ReadElements(ptr, args...);
	return true;
}


inline std::string SockAddrToString(const SOCKADDR *p)
{
	char res[INET6_ADDRSTRLEN];
	ULONG res_len = _countof(res);

	HMODULE ntdll = GetModuleHandleA("ntdll.dll");

	if (p->sa_family == AF_INET6)
	{
		const sockaddr_in6* sai = reinterpret_cast<const sockaddr_in6*>(p);

		NTSTATUS (*pRtlIpv6AddressToStringExA)(
			const struct in6_addr *Address,
			ULONG ScopeId,
			USHORT Port,
			PSTR AddressString,
			PULONG AddressStringLength
			);

		pRtlIpv6AddressToStringExA = (decltype(pRtlIpv6AddressToStringExA))GetProcAddress(ntdll, "RtlIpv6AddressToStringExA");

		if (pRtlIpv6AddressToStringExA)
		{
			if (pRtlIpv6AddressToStringExA(&sai->sin6_addr, sai->sin6_scope_id, sai->sin6_port, res, &res_len) == 0)
			{
				return res;
			}
			else
			{
				return "Error Converting IP String";
			}
		}
	}
	else if (p->sa_family == AF_INET)
	{
		const sockaddr_in* sai = reinterpret_cast<const sockaddr_in*>(p);

		NTSTATUS (*pRtlIpv4AddressToStringExA)(
			const struct in_addr *Address,
			USHORT Port,
			PSTR AddressString,
			PULONG AddressStringLength
			);

		pRtlIpv4AddressToStringExA = (decltype(pRtlIpv4AddressToStringExA))GetProcAddress(ntdll, "RtlIpv4AddressToStringExA");

		if (pRtlIpv4AddressToStringExA)
		{
			if (pRtlIpv4AddressToStringExA(&sai->sin_addr, sai->sin_port, res, &res_len) == 0)
			{
				return res;
			}
			else
			{
				return "Error Converting IP String";
			}				
		}
	}

	return res;
}

// connects to a socket/(port|service) name,
// services need be manually set to ports in C:\Windows\System32\drivers\etc\services if you want that

inline SOCKET SocketConnectToAddress(const char* server_name, const char* service_or_port)
{
	ADDRINFO hints{};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	ADDRINFO *addr_info = nullptr;
	VERIFY(getaddrinfo(server_name, service_or_port, &hints, &addr_info)==0, "Unable to find address '%s' service '%s' : %s", server_name, service_or_port, WSALastErrorToString().c_str());
	AT_SCOPE_EXIT([addr_info](){ freeaddrinfo(addr_info); });

	// setup the slave socket
	for (ADDRINFO* ai = addr_info; ai; ai = ai->ai_next)
	{
		SOCKET result = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (result != INVALID_SOCKET)
		{
			int res = connect(result, ai->ai_addr, (int)ai->ai_addrlen);
			if (res != SOCKET_ERROR)
			{
				return result;
			}
			else
			{
				DebugLog("Couldn't connect to '%s' with family:%d, type:%d, protocol:%d, reason:%s\n", SockAddrToString(ai->ai_addr).c_str(), ai->ai_family, ai->ai_socktype, ai->ai_protocol, WSALastErrorToString().c_str());
				closesocket(result);
			}
		}
	}

	return INVALID_SOCKET;
}

inline SOCKET SocketConnectToAddress(const char* server_name, const uint16_t port)
{
	char tmp[32];
	snprintf(tmp, sizeof(tmp), "%u", port);
	return SocketConnectToAddress(server_name, tmp);
}

// result from creating a listener
struct SocketCreateListenerResults
{
	// the socket for the connection
	SOCKET socket = INVALID_SOCKET;
	// the address of the connection we are bound to
	char address[INET6_ADDRSTRLEN];
	// the service or port we are connected to
	uint16_t port;
};

// by default, this will create a listener on any free port
inline SocketCreateListenerResults SocketCreateListener(uint16_t port = 0, uint16_t family = AF_UNSPEC)
{
	SocketCreateListenerResults results;

	union
	{
		sockaddr_storage storage;
		sockaddr_in6 addr6;
		sockaddr_in addr4;
	};

	// first attempt ipv6
	if (family == AF_INET6 || family == AF_UNSPEC)
	{
		memset(&storage, 0, sizeof(storage));
		
		addr6.sin6_family = AF_INET6;
		inet_pton(AF_INET6, "::1", &addr6.sin6_addr);
		addr6.sin6_port = htons(port);

		results.socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

		if (results.socket != INVALID_SOCKET)
		{
			if (bind(results.socket, reinterpret_cast<SOCKADDR*>(&addr6), sizeof(addr6)) == SOCKET_ERROR)
			{
				DebugLog("Unable to bind to ip6 %s: %s\n", SockAddrToString((SOCKADDR*)&storage).c_str(), WSALastErrorToString().c_str());
				closesocket(results.socket);
				results.socket = INVALID_SOCKET;
			}
		}
	}

	// fall back to ipv4
	if (results.socket == INVALID_SOCKET && (family == AF_INET || family == AF_UNSPEC))
	{
		memset(&storage, 0, sizeof(storage));
		
		addr4.sin_family = AF_INET;
		inet_pton(AF_INET, "127.0.0.1", &addr4.sin_addr);
		addr4.sin_port = htons(port);

		results.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (results.socket == INVALID_SOCKET)
			return SocketCreateListenerResults();

		if (bind(results.socket, reinterpret_cast<SOCKADDR*>(&addr4), sizeof(addr4)) == SOCKET_ERROR)
		{
			DebugLog("Unable to bind to ip4 %s: %s\n", SockAddrToString((SOCKADDR*)&storage).c_str(), WSALastErrorToString().c_str());
			closesocket(results.socket);
			results.socket = INVALID_SOCKET;
		}
	}

	if (results.socket != INVALID_SOCKET)
	{
		VERIFY(listen(results.socket, SOMAXCONN) != SOCKET_ERROR, WSALastErrorToString().c_str());

		// retrieve this service again, to get the connected port or service

		// get the port/ip we connected to
		memset(&storage, 0, sizeof(storage));
		int addr_len = sizeof(storage);
		VERIFY(getsockname(results.socket, (SOCKADDR*)&storage, &addr_len)!=SOCKET_ERROR, WSALastErrorToString().c_str());

		results.port = ntohs(storage.ss_family == AF_INET6 ? addr6.sin6_port : addr4.sin_port);

		inet_ntop(storage.ss_family, 
			storage.ss_family == AF_INET6 ? 
				(void*)&addr6.sin6_addr :
				(void*)&addr4.sin_addr,
			results.address,
			sizeof(results.address)
		);

		return results;		
	}

	return results;
}

// start the listener thread and manage a list of threads for each incoming connection
template<typename CONNECTION_FUNC>
void RunServerListener(const char* debug_name, SOCKET listener_socket, CONNECTION_FUNC && connection_func) 
{
	MutexProtectedResource<std::vector<std::pair<SOCKET, std::thread>>> server_threads;

	bool first = true;

	DebugLog("%s: Started\n", debug_name);

	while (true)
	{
		sockaddr_storage addr;
		socklen_t addr_len = sizeof(addr);

		// this is a blocking call
		SOCKET connection_socket = accept(listener_socket, (SOCKADDR*)&addr, &addr_len);

		if (connection_socket != INVALID_SOCKET)
		{
			DebugLog("%s: Connected to %s\n", debug_name, SockAddrToString((SOCKADDR*)&addr));

			first = false;

			server_threads.lock()->emplace_back(connection_socket, 
				[connection_socket, &server_threads, &connection_func, addr, debug_name]()
				{
					connection_func(connection_socket, (SOCKADDR*)&addr);

					DebugLog("%s: Disconnected from %s\n", debug_name, SockAddrToString((SOCKADDR*)&addr));

					closesocket(connection_socket);

					// now clean us up from server_threads
					auto lock = server_threads.lock();
					for (auto itr = lock->begin(); itr != lock->end(); ++itr)
					{
						if (itr->first == connection_socket)
						{
							itr->second.detach(); // this is us, detach us beacuse we're exiting anyway
							// swap and pop
							std::iter_swap(itr, lock->end()-1);
							lock->pop_back();
							return;
						}
					}
				}
			);
		}
		else
		{
			if (first) // it's likely if we have an issue on our first listen we have a problem with the listener socket
			{
				DebugLog("%s: Unable to listen on socket: %s\n", debug_name, WSALastErrorToString().c_str());
			}

			// otherwise this is how we force disconnect
			break;
		}
	}

	for (std::pair<SOCKET, std::thread>& connection : *server_threads.lock())
	{
		closesocket(connection.first);
	}

	// the server threads clean from server_threads gracefully, just wait until the array is empty then
	while (true)
	{
		{
			if (server_threads.lock()->empty())
				return;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // give it time
	}
}

