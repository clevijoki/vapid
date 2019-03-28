#include "BufOps.h"
#include <unordered_map>
#include "DebugLog.h"
#include "NetworkUtil.h"

#include "GeneratedCommon.h"

namespace worker
{
	MutexProtectedResource<SOCKET> g_socket(INVALID_SOCKET);
}

#include "GeneratedWorker.inl"

namespace worker
{
	MutexProtectedResource<std::unordered_map<HANDLE,HANDLE_DATA_TYPE>> g_handle_table(NoInit);
	WSADATA g_WSADATA;

	void EnsureNetworkIsSetup(SOCKET& sock)
	{
		if (sock != INVALID_SOCKET)
			return;

		WSAStartup(MAKEWORD(2,2), &worker::g_WSADATA);

		char server_name[256] = {0};
		char port_str[64] = {0};

		GetEnvironmentVariable("VAPID_SERVER_NAME", server_name, _countof(server_name));
		GetEnvironmentVariable("VAPID_SERVICE_OR_PORT", port_str, _countof(port_str));

		sock = SocketConnectToAddress(server_name, port_str);
		EXPECT_NE(sock, INVALID_SOCKET);
	}	

	inline HANDLE_DATA_TYPE LookupRealHandle(HANDLE fake_handle)
	{
		// map our fake handle to the real one so we can look it up later
		auto fh_lock = g_handle_table.lock();

		auto itr = fh_lock->find(fake_handle);
		if (itr != fh_lock->end())
		{
			return itr->second;
		}

		return INVALID_HANDLE_DATA_TYPE;
	}

	inline void ReleaseFakeHandle(HANDLE fake_handle)
	{
		// map our fake handle to the real one so we can look it up later
		auto fh_lock = g_handle_table.lock();
		auto itr = fh_lock->find(fake_handle);
		if (itr != fh_lock->end())
		{
			HANDLE_DATA_TYPE value = itr->second;
			fh_lock->erase(itr);
			os::CloseHandle(fake_handle);
		}

		return;
	}

	// the local process file handle must stay valid to succeed
	inline HANDLE CreateFakeHandle(HANDLE_DATA_TYPE key)
	{
		HANDLE result = CreateEventA(NULL, TRUE, FALSE, NULL);
		g_handle_table.lock()->emplace(result, key);
		return result;

	}

	BOOL ReadFileEx(
	  HANDLE                          hFile,
	  LPVOID                          lpBuffer,
	  DWORD                           nNumberOfBytesToRead,
	  LPOVERLAPPED                    lpOverlapped,
	  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	)
	{
		DWORD nNumberOfBytesRead = 0;
		BOOL result = worker::ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, &nNumberOfBytesRead, nullptr);
		lpCompletionRoutine(GetLastError(), nNumberOfBytesRead, lpOverlapped);
		return result;
	}

	BOOL WriteFileEx(
	  HANDLE                          hFile,
	  LPCVOID                         lpBuffer,
	  DWORD                           nNumberOfBytesToWrite,
	  LPOVERLAPPED                    lpOverlapped,
	  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	)
	{
		DWORD nNumberOfBytesWritten = 0;
		BOOL result = worker::WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, &nNumberOfBytesWritten, nullptr);
		lpCompletionRoutine(GetLastError(), nNumberOfBytesWritten, lpOverlapped);
		return result;
	}	
}

