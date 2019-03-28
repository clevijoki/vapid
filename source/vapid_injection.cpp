#include "detours/detours.h"
#include <cstdio>
#include <cwchar>
#include <stdarg.h>
#include <unordered_map>

#include "MutexProtectedResource.h"
#include "DebugLog.h"
// #include "Serializer.h"

#include "Worker.inl"
#include "GeneratedDetours.inl"

BOOL MutexProtectedResourceCloseHandle(HANDLE handle)
{
	return worker::os::CloseHandle(handle);
}

BOOL WINAPI DllMain(
	HINSTANCE hinst,
	DWORD     dwReason,
	LPVOID    pvReserved
)
{
	if (DetourIsHelperProcess())
	{
		return TRUE;
	}

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		worker::g_handle_table.Initialize({});

		detours::Inject();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		detours::UndoInject();

		shutdown(*worker::g_socket.lock(), SD_SEND);

		WSACleanup();
	}

	return TRUE;
}