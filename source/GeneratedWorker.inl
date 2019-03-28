
/* This file is generated from generated_overloaded.py, do not edit directly */

namespace worker
{
    void EnsureNetworkIsSetup(SOCKET& s);
    HANDLE_DATA_TYPE LookupRealHandle(HANDLE fake_handle);

    void ReleaseFakeHandle(HANDLE);
    HANDLE CreateFakeHandle(HANDLE_DATA_TYPE data_type);

    namespace os
    {
		HANDLE (*CreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
		HANDLE (*CreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
		BOOL (*CloseHandle)(HANDLE);
		BOOL (*ReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
		BOOL (*ReadFileEx)(HANDLE, LPVOID, DWORD, LPOVERLAPPED, LPOVERLAPPED_COMPLETION_ROUTINE);
		BOOL (*WriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
		BOOL (*WriteFileEx)(HANDLE, LPCVOID, DWORD, LPOVERLAPPED, LPOVERLAPPED_COMPLETION_ROUTINE);
		DWORD (*GetFileSize)(HANDLE, LPDWORD);
		BOOL (*GetFileSizeEx)(HANDLE, PLARGE_INTEGER);
		BOOL (*SetFilePointerEx)(HANDLE, LARGE_INTEGER, PLARGE_INTEGER, DWORD);
		DWORD (*SetFilePointer)(HANDLE, LONG, PLONG, DWORD);
		BOOL (*CreateDirectoryA)(LPCSTR, LPSECURITY_ATTRIBUTES);
		BOOL (*CreateDirectoryW)(LPCWSTR, LPSECURITY_ATTRIBUTES);
		BOOL (*DeleteFileA)(LPCSTR);
		BOOL (*DeleteFileW)(LPCWSTR);
		DWORD (*GetFileAttributesA)(LPCSTR);
		DWORD (*GetFileAttributesW)(LPCWSTR);
		DWORD (*GetFileType)(HANDLE);

    }

	HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	HANDLE CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	BOOL CloseHandle(HANDLE hObject);
	BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
	BOOL ReadFileEx(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	BOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
	BOOL WriteFileEx(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
	BOOL GetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize);
	BOOL SetFilePointerEx(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod);
	DWORD SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);
	BOOL CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
	BOOL CreateDirectoryW(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
	BOOL DeleteFileA(LPCSTR lpFileName);
	BOOL DeleteFileW(LPCWSTR lpFileName);
	DWORD GetFileAttributesA(LPCSTR lpFileName);
	DWORD GetFileAttributesW(LPCWSTR lpFileName);
	DWORD GetFileType(HANDLE hFile);


	HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
	{
		const size_t param_size = sizeof(uint8_t) + sizeof(uint32_t) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD);
		const size_t fixed_response_size = sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::CreateFileA));
		const uint32_t lpFileName_len = static_cast<uint32_t>(strlen(lpFileName));
		bufops::Write<uint32_t>(buf, lpFileName_len);
		bufops::Write<DWORD>(buf, dwDesiredAccess);
		bufops::Write<DWORD>(buf, dwShareMode);
		bufops::Write<DWORD>(buf, dwCreationDisposition);
		bufops::Write<DWORD>(buf, dwFlagsAndAttributes);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return INVALID_HANDLE_VALUE;
		if (!SendData(*socket_ptr, lpFileName, lpFileName_len))
			return INVALID_HANDLE_VALUE;

		const size_t dynamic_response_size = sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return INVALID_HANDLE_VALUE;
		buf = data_buf;
		const HANDLE result = CreateFakeHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));
		if (result == INVALID_HANDLE_VALUE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	HANDLE CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
	{
		const size_t param_size = sizeof(uint8_t) + sizeof(uint32_t) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD);
		const size_t fixed_response_size = sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::CreateFileW));
		const uint32_t lpFileName_len = static_cast<uint32_t>(wcslen(lpFileName));
		bufops::Write<uint32_t>(buf, lpFileName_len);
		bufops::Write<DWORD>(buf, dwDesiredAccess);
		bufops::Write<DWORD>(buf, dwShareMode);
		bufops::Write<DWORD>(buf, dwCreationDisposition);
		bufops::Write<DWORD>(buf, dwFlagsAndAttributes);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return INVALID_HANDLE_VALUE;
		if (!SendData(*socket_ptr, lpFileName, lpFileName_len * sizeof(wchar_t)))
			return INVALID_HANDLE_VALUE;

		const size_t dynamic_response_size = sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return INVALID_HANDLE_VALUE;
		buf = data_buf;
		const HANDLE result = CreateFakeHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));
		if (result == INVALID_HANDLE_VALUE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	BOOL CloseHandle(HANDLE hObject)
	{
		HANDLE_DATA_TYPE hObject_data = LookupRealHandle(hObject);
		if (hObject_data == INVALID_HANDLE_DATA_TYPE)
			return os::CloseHandle(hObject);

		const size_t param_size = sizeof(uint8_t) + sizeof(HANDLE_DATA_TYPE);
		const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::CloseHandle));
		bufops::Write<HANDLE_DATA_TYPE>(buf, hObject_data);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return FALSE;

		const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return FALSE;
		buf = data_buf;
		const BOOL result = bufops::Read<BOOL>(buf);
		if (result == FALSE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		ReleaseFakeHandle(hObject);
		return result;
	}

	BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
	{
		HANDLE_DATA_TYPE hFile_data = LookupRealHandle(hFile);
		if (hFile_data == INVALID_HANDLE_DATA_TYPE)
			return os::ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

		const size_t param_size = sizeof(uint8_t) + sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD) + sizeof(uint8_t);
		const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::ReadFile));
		bufops::Write<HANDLE_DATA_TYPE>(buf, hFile_data);
		bufops::Write<DWORD>(buf, nNumberOfBytesToRead);
		bufops::Write<uint8_t>(buf, lpNumberOfBytesRead ? 1 : 0);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return FALSE;

		const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD) + (lpNumberOfBytesRead ? sizeof(DWORD) : 0);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return FALSE;
		buf = data_buf;
		const BOOL result = bufops::Read<BOOL>(buf);
		if (result == FALSE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		if (lpBuffer) ReceiveData(*socket_ptr, lpBuffer, nNumberOfBytesToRead);
		if (lpNumberOfBytesRead) *lpNumberOfBytesRead = bufops::Read<DWORD>(buf);
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	BOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
	{
		HANDLE_DATA_TYPE hFile_data = LookupRealHandle(hFile);
		if (hFile_data == INVALID_HANDLE_DATA_TYPE)
			return os::WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);

		const size_t param_size = sizeof(uint8_t) + sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD) + sizeof(uint8_t);
		const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::WriteFile));
		bufops::Write<HANDLE_DATA_TYPE>(buf, hFile_data);
		bufops::Write<DWORD>(buf, nNumberOfBytesToWrite);
		bufops::Write<uint8_t>(buf, lpNumberOfBytesWritten ? 1 : 0);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return FALSE;
		if (!SendData(*socket_ptr, lpBuffer, nNumberOfBytesToWrite))
			return FALSE;

		const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD) + (lpNumberOfBytesWritten ? sizeof(DWORD) : 0);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return FALSE;
		buf = data_buf;
		const BOOL result = bufops::Read<BOOL>(buf);
		if (result == FALSE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		if (lpNumberOfBytesWritten) *lpNumberOfBytesWritten = bufops::Read<DWORD>(buf);
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
	{
		HANDLE_DATA_TYPE hFile_data = LookupRealHandle(hFile);
		if (hFile_data == INVALID_HANDLE_DATA_TYPE)
			return os::GetFileSize(hFile, lpFileSizeHigh);

		const size_t param_size = sizeof(uint8_t) + sizeof(HANDLE_DATA_TYPE) + sizeof(uint8_t);
		const size_t fixed_response_size = sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::GetFileSize));
		bufops::Write<HANDLE_DATA_TYPE>(buf, hFile_data);
		bufops::Write<uint8_t>(buf, lpFileSizeHigh ? 1 : 0);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return INVALID_FILE_SIZE;

		const size_t dynamic_response_size = sizeof(DWORD) + sizeof(DWORD) + (lpFileSizeHigh ? sizeof(DWORD) : 0);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return INVALID_FILE_SIZE;
		buf = data_buf;
		const DWORD result = bufops::Read<DWORD>(buf);
		if (result == INVALID_FILE_SIZE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		if (lpFileSizeHigh) *lpFileSizeHigh = bufops::Read<DWORD>(buf);
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	BOOL GetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize)
	{
		HANDLE_DATA_TYPE hFile_data = LookupRealHandle(hFile);
		if (hFile_data == INVALID_HANDLE_DATA_TYPE)
			return os::GetFileSizeEx(hFile, lpFileSize);

		const size_t param_size = sizeof(uint8_t) + sizeof(HANDLE_DATA_TYPE) + sizeof(uint8_t);
		const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD) + sizeof(LARGE_INTEGER);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::GetFileSizeEx));
		bufops::Write<HANDLE_DATA_TYPE>(buf, hFile_data);
		bufops::Write<uint8_t>(buf, lpFileSize ? 1 : 0);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return INVALID_FILE_SIZE;

		const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD) + (lpFileSize ? sizeof(LARGE_INTEGER) : 0);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return INVALID_FILE_SIZE;
		buf = data_buf;
		const BOOL result = bufops::Read<BOOL>(buf);
		if (result == INVALID_FILE_SIZE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		if (lpFileSize) *lpFileSize = bufops::Read<LARGE_INTEGER>(buf);
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	BOOL SetFilePointerEx(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod)
	{
		HANDLE_DATA_TYPE hFile_data = LookupRealHandle(hFile);
		if (hFile_data == INVALID_HANDLE_DATA_TYPE)
			return os::SetFilePointerEx(hFile, liDistanceToMove, lpNewFilePointer, dwMoveMethod);

		const size_t param_size = sizeof(uint8_t) + sizeof(HANDLE_DATA_TYPE) + sizeof(LARGE_INTEGER) + sizeof(uint8_t) + sizeof(DWORD);
		const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD) + sizeof(LARGE_INTEGER);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::SetFilePointerEx));
		bufops::Write<HANDLE_DATA_TYPE>(buf, hFile_data);
		bufops::Write<LARGE_INTEGER>(buf, liDistanceToMove);
		bufops::Write<uint8_t>(buf, lpNewFilePointer ? 1 : 0);
		bufops::Write<DWORD>(buf, dwMoveMethod);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return FALSE;

		const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD) + (lpNewFilePointer ? sizeof(LARGE_INTEGER) : 0);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return FALSE;
		buf = data_buf;
		const BOOL result = bufops::Read<BOOL>(buf);
		if (result == FALSE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		if (lpNewFilePointer) *lpNewFilePointer = bufops::Read<LARGE_INTEGER>(buf);
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	DWORD SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
	{
		HANDLE_DATA_TYPE hFile_data = LookupRealHandle(hFile);
		if (hFile_data == INVALID_HANDLE_DATA_TYPE)
			return os::SetFilePointer(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);

		const size_t param_size = sizeof(uint8_t) + sizeof(HANDLE_DATA_TYPE) + sizeof(LONG) + sizeof(uint8_t) + sizeof(LONG) + sizeof(DWORD);
		const size_t fixed_response_size = sizeof(DWORD) + sizeof(DWORD) + sizeof(LONG);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::SetFilePointer));
		bufops::Write<HANDLE_DATA_TYPE>(buf, hFile_data);
		bufops::Write<LONG>(buf, lDistanceToMove);
		bufops::Write<uint8_t>(buf, lpDistanceToMoveHigh ? 1 : 0);
		bufops::Write<DWORD>(buf, dwMoveMethod);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return -1;

		const size_t dynamic_response_size = sizeof(DWORD) + sizeof(DWORD) + (lpDistanceToMoveHigh ? sizeof(LONG) : 0);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return -1;
		buf = data_buf;
		const DWORD result = bufops::Read<DWORD>(buf);
		if (result == -1)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		if (lpDistanceToMoveHigh) *lpDistanceToMoveHigh = bufops::Read<LONG>(buf);
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	BOOL CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
	{
		const size_t param_size = sizeof(uint8_t) + sizeof(uint32_t);
		const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::CreateDirectoryA));
		const uint32_t lpPathName_len = static_cast<uint32_t>(strlen(lpPathName));
		bufops::Write<uint32_t>(buf, lpPathName_len);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return FALSE;
		if (!SendData(*socket_ptr, lpPathName, lpPathName_len))
			return FALSE;

		const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return FALSE;
		buf = data_buf;
		const BOOL result = bufops::Read<BOOL>(buf);
		if (result == FALSE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	BOOL CreateDirectoryW(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
	{
		const size_t param_size = sizeof(uint8_t) + sizeof(uint32_t);
		const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::CreateDirectoryW));
		const uint32_t lpPathName_len = static_cast<uint32_t>(wcslen(lpPathName));
		bufops::Write<uint32_t>(buf, lpPathName_len);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return FALSE;
		if (!SendData(*socket_ptr, lpPathName, lpPathName_len * sizeof(wchar_t)))
			return FALSE;

		const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return FALSE;
		buf = data_buf;
		const BOOL result = bufops::Read<BOOL>(buf);
		if (result == FALSE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	BOOL DeleteFileA(LPCSTR lpFileName)
	{
		const size_t param_size = sizeof(uint8_t) + sizeof(uint32_t);
		const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::DeleteFileA));
		const uint32_t lpFileName_len = static_cast<uint32_t>(strlen(lpFileName));
		bufops::Write<uint32_t>(buf, lpFileName_len);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return FALSE;
		if (!SendData(*socket_ptr, lpFileName, lpFileName_len))
			return FALSE;

		const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return FALSE;
		buf = data_buf;
		const BOOL result = bufops::Read<BOOL>(buf);
		if (result == FALSE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	BOOL DeleteFileW(LPCWSTR lpFileName)
	{
		const size_t param_size = sizeof(uint8_t) + sizeof(uint32_t);
		const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::DeleteFileW));
		const uint32_t lpFileName_len = static_cast<uint32_t>(wcslen(lpFileName));
		bufops::Write<uint32_t>(buf, lpFileName_len);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return FALSE;
		if (!SendData(*socket_ptr, lpFileName, lpFileName_len * sizeof(wchar_t)))
			return FALSE;

		const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return FALSE;
		buf = data_buf;
		const BOOL result = bufops::Read<BOOL>(buf);
		if (result == FALSE)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	DWORD GetFileAttributesA(LPCSTR lpFileName)
	{
		const size_t param_size = sizeof(uint8_t) + sizeof(uint32_t);
		const size_t fixed_response_size = sizeof(DWORD) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::GetFileAttributesA));
		const uint32_t lpFileName_len = static_cast<uint32_t>(strlen(lpFileName));
		bufops::Write<uint32_t>(buf, lpFileName_len);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return -1;
		if (!SendData(*socket_ptr, lpFileName, lpFileName_len))
			return -1;

		const size_t dynamic_response_size = sizeof(DWORD) + sizeof(DWORD);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return -1;
		buf = data_buf;
		const DWORD result = bufops::Read<DWORD>(buf);
		if (result == -1)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	DWORD GetFileAttributesW(LPCWSTR lpFileName)
	{
		const size_t param_size = sizeof(uint8_t) + sizeof(uint32_t);
		const size_t fixed_response_size = sizeof(DWORD) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::GetFileAttributesW));
		const uint32_t lpFileName_len = static_cast<uint32_t>(wcslen(lpFileName));
		bufops::Write<uint32_t>(buf, lpFileName_len);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return -1;
		if (!SendData(*socket_ptr, lpFileName, lpFileName_len * sizeof(wchar_t)))
			return -1;

		const size_t dynamic_response_size = sizeof(DWORD) + sizeof(DWORD);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return -1;
		buf = data_buf;
		const DWORD result = bufops::Read<DWORD>(buf);
		if (result == -1)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}

	DWORD GetFileType(HANDLE hFile)
	{
		HANDLE_DATA_TYPE hFile_data = LookupRealHandle(hFile);
		if (hFile_data == INVALID_HANDLE_DATA_TYPE)
			return os::GetFileType(hFile);

		const size_t param_size = sizeof(uint8_t) + sizeof(HANDLE_DATA_TYPE);
		const size_t fixed_response_size = sizeof(DWORD) + sizeof(DWORD);
		char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];

		char *buf = data_buf;
		bufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::GetFileType));
		bufops::Write<HANDLE_DATA_TYPE>(buf, hFile_data);
		EXPECT_EQ(buf, data_buf + param_size);

		auto socket_ptr = worker::g_socket.lock();
		EnsureNetworkIsSetup(*socket_ptr);

		if (!SendData(*socket_ptr, data_buf, param_size))
			return -1;

		const size_t dynamic_response_size = sizeof(DWORD) + sizeof(DWORD);
		if (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))
			return -1;
		buf = data_buf;
		const DWORD result = bufops::Read<DWORD>(buf);
		if (result == -1)
		{
			SetLastError(bufops::Read<DWORD>(buf));
			return result;
		}
		EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));

		return result;
	}


}

