HANDLE CreateFileA(
	LPCSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes __IGNORE(nullptr),
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE       	      hTemplateFile __IGNORE(nullptr) );

HANDLE CreateFileW(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes __IGNORE(nullptr),
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile __IGNORE(nullptr)
);

BOOL CloseHandle(HANDLE hObject);

BOOL ReadFile(
	HANDLE hFile,
	LPVOID lpBuffer __BUFFER_SIZE(nNumberOfBytesToRead),
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped __IGNORE(nullptr)
);

__NO_IMPL BOOL ReadFileEx(
  HANDLE                          hFile,
  LPVOID                          lpBuffer,
  DWORD                           nNumberOfBytesToRead,
  LPOVERLAPPED                    lpOverlapped,
  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);

BOOL WriteFile(
	HANDLE       hFile,
	LPCVOID 	 lpBuffer __BUFFER_SIZE(nNumberOfBytesToWrite),
	DWORD        nNumberOfBytesToWrite,
	LPDWORD      lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped __IGNORE(nullptr)
);

__NO_IMPL BOOL WriteFileEx(
  HANDLE                          hFile,
  LPCVOID                         lpBuffer,
  DWORD                           nNumberOfBytesToWrite,
  LPOVERLAPPED                    lpOverlapped,
  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);

__FAIL_VALUE(INVALID_FILE_SIZE)
DWORD GetFileSize(
  HANDLE  hFile,
  LPDWORD lpFileSizeHigh
);

__FAIL_VALUE(INVALID_FILE_SIZE)
BOOL GetFileSizeEx(
  HANDLE         hFile,
  PLARGE_INTEGER lpFileSize
);

BOOL SetFilePointerEx(
  HANDLE         hFile,
  LARGE_INTEGER  liDistanceToMove,
  PLARGE_INTEGER lpNewFilePointer,
  DWORD          dwMoveMethod
);

DWORD SetFilePointer(
  HANDLE hFile,
  LONG   lDistanceToMove,
  PLONG  lpDistanceToMoveHigh __INOUT,
  DWORD  dwMoveMethod
);

BOOL CreateDirectoryA(
  LPCSTR                lpPathName,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes __IGNORE(nullptr)
);

BOOL CreateDirectoryW(
  LPCWSTR               lpPathName,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes __IGNORE(nullptr)
);

BOOL DeleteFileA(
  LPCSTR lpFileName
);

BOOL DeleteFileW(
  LPCWSTR lpFileName
);

DWORD GetFileAttributesA(
  LPCSTR lpFileName
);

DWORD GetFileAttributesW(
  LPCWSTR lpFileName
);

DWORD GetFileType(
  HANDLE hFile
);
