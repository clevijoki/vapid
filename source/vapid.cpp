#include "detours/detours.h"
#include <string>
#include <stdio.h>
#include <vector>
#include "Assert.h"
#include "AtScopeExit.h"

#include <chrono>
#include "Master.inl"

class ScopeTimer
{
	std::chrono::high_resolution_clock::time_point t1;
	const char* scope_name;
public:
	ScopeTimer(const char* scope_name) : t1(std::chrono::high_resolution_clock::now()), scope_name(scope_name)
	{

	}

	~ScopeTimer()
	{
		release();
	}

	void release()
	{
		if (scope_name)
		{
			std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

  			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
  			DebugLog("%s took %g seconds\n", scope_name, time_span.count());
		}

		scope_name = nullptr;
	}
};

BOOL MutexProtectedResourceCloseHandle(HANDLE handle)
{
	return CloseHandle(handle);
}

//////////////////////////////////////////////////////////////////////////////
//
//  This code verifies that the named DLL has been configured correctly
//  to be imported into the target process.  DLLs must export a function with
//  ordinal #1 so that the import table touch-up magic works.
//
struct ExportContext
{
    BOOL    fHasOrdinal1 = FALSE;
    ULONG   nExports = 0;
};

static BOOL CALLBACK ExportCallback(_In_opt_ PVOID pContext,
                                    _In_ ULONG nOrdinal,
                                    _In_opt_ LPCSTR pszSymbol,
                                    _In_opt_ PVOID pbTarget)
{
    (void)pContext;
    (void)pbTarget;
    (void)pszSymbol;

    ExportContext *pec = (ExportContext *)pContext;

    if (nOrdinal == 1) {
        pec->fHasOrdinal1 = TRUE;
    }
    pec->nExports++;

    return TRUE;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: vapid appname [appargs]\n");
		return -1;
	}

	std::string command_line;

    CHAR full_exe_path[1024] = "\0";
	if (SearchPathA(nullptr, argv[1], ".exe", _countof(full_exe_path), full_exe_path, nullptr) == 0)
	{
		fprintf(stderr, "Could not find '%s' executable\n", argv[1]);
	}

	for (int n = 2; n < argc; ++n)
	{
		command_line += " \"";
		command_line += argv[n];
		command_line += '"';
	}

    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};

    si.cb = sizeof(si);

	LPCSTR dlls[] = {
		"vapid_injection64"
	};

	for (LPCSTR dll : dlls)
	{
		// inspect them
        HMODULE hDll = LoadLibraryExA(dll, NULL, DONT_RESOLVE_DLL_REFERENCES);
        if (hDll == NULL) {
            printf("vapid.exe: Error: %s failed to load (error %d).\n",
                   dll,
                   GetLastError());
            return -1;
        }

		ExportContext ec{};
		DetourEnumerateExports(hDll, &ec, ExportCallback);

        if (!ec.fHasOrdinal1) {
            printf("vapid.exe: Error: %s does not export ordinal #1.\n",
                   dll);
            printf("             See help entry DetourCreateProcessWithDllEx in Detours.chm.\n");
            return -1;
        }

        FreeLibrary(hDll);
	}

	// initialize winsock/etc
	WSADATA wsa_data;
	EXPECT_EQ(NO_ERROR, WSAStartup(MAKEWORD(2,2), &wsa_data));
	AT_SCOPE_EXIT([](){ WSACleanup(); });

	SocketCreateListenerResults listener = SocketCreateListener();

	std::thread listener_thread(master::RunServer, listener.socket);

	std::vector<char> enviornment_variables;
	enviornment_variables.reserve(1024);

	auto add_env_var= [&](const char* var)
	{	
		const size_t var_len = strlen(var);

		if (var_len > 0)
		{
			std::copy(var, var + var_len + 1, std::back_inserter(enviornment_variables));
		}

		return var + var_len + 1;
	};

	{
		// build the environment variable string block
		const char* env_itr = GetEnvironmentStrings();

		while(env_itr[0])
		{
			env_itr = add_env_var(env_itr);
		}
	}

	add_env_var("VAPID_SERVER_NAME=127.0.0.1");
	add_env_var("VAPID_PORT=27015");

	enviornment_variables.push_back(0); // windows expects another trailing 0 to terminate the environment string block

	CHAR szExeFileName[MAX_PATH]; 
	GetModuleFileNameA(NULL, szExeFileName, MAX_PATH);

	// always set our current working directory to be the vapid.exe location, for consistency
	std::string cwd = szExeFileName;
	for (char& c : cwd)
	{
		c = c == '/' ? '\\' : c;
	}

	cwd = cwd.substr(0, cwd.find_last_of('\\'));

	{
		// ScopeTimer cp("DetourCreateProcessWithDlls");
		if (!DetourCreateProcessWithDlls(
			full_exe_path,
			(LPSTR)command_line.c_str(),
			NULL,
			NULL,
			FALSE, // don't inherit handles
			CREATE_DEFAULT_ERROR_MODE,
			enviornment_variables.data(),
			cwd.c_str(),
			&si,
			&pi,
			_countof(dlls),
			dlls,
			NULL
		))
		{
			DebugLog("DetourCreateProcessWithDlls failed: %s\n", Win32LastErrorToString().c_str());
			return -1;
		}
	}

	{
		//ScopeTimer rt("ResumeThread");
    	ResumeThread(pi.hThread);
	}


	{
		//ScopeTimer rt("WaitForSingleObject");
 	   	WaitForSingleObject(pi.hProcess, INFINITE);
 	}

    DWORD dwResult = 0;
    if (!GetExitCodeProcess(pi.hProcess, &dwResult)) {
        printf("vapid.exe: GetExitCodeProcess failed: %s\n", Win32LastErrorToString().c_str());
        return 9010;
    }

	closesocket(listener.socket); // should tell the listener_thread to start cleaning up

	listener_thread.join();    

	return int(dwResult);
}