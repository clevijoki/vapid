#include "CacheServer.h"
#include "AtScopeExit.h"

BOOL MutexProtectedResourceCloseHandle(HANDLE handle)
{
	return CloseHandle(handle);
}

int main(int argc, const char** argv)
{
	WSADATA wsa_data;
	EXPECT_EQ(NO_ERROR, WSAStartup(MAKEWORD(2,2), &wsa_data));
	AT_SCOPE_EXIT([](){ WSACleanup(); });

	SocketCreateListenerResults listener = SocketCreateListener();
	ASSERT(listener.socket != INVALID_SOCKET, "Coudn't create listener : %s\n", WSALastErrorToString().c_str());
	AT_SCOPE_EXIT([&](){ closesocket(listener.socket); });

	cache::Server *server = cache::ServerCreate();
	AT_SCOPE_EXIT([server](){ ServerDestroy(server); });

	ServerRun(server, listener.socket);

	return 0;
}