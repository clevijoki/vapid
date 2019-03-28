
/* This function is generated from geneated_overloaded.py, do not edit directly */

namespace master {

bool Update(SOCKET socket)
{
    using namespace bufops;

    common::ApiTag tag;
    if (!ReceiveData(socket, &tag, sizeof(uint8_t)))
        return false;

    switch (tag)
    {
		case common::ApiTag::CreateFileA:
		{
			const size_t param_size = sizeof(uint32_t) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD);
			const size_t fixed_response_size = sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const uint32_t lpFileName_len = bufops::Read<uint32_t>(buf);
			const DWORD dwDesiredAccess = bufops::Read<DWORD>(buf);
			const DWORD dwShareMode = bufops::Read<DWORD>(buf);
			const DWORD dwCreationDisposition = bufops::Read<DWORD>(buf);
			const DWORD dwFlagsAndAttributes = bufops::Read<DWORD>(buf);
			EXPECT_EQ(buf, data_buf + param_size);

			std::unique_ptr<char[]> lpFileName(new char[lpFileName_len+1]);
			if (!ReceiveData(socket, lpFileName.get(), lpFileName_len))
				return false;
			lpFileName[lpFileName_len] = 0;

			const HANDLE result = CreateFileA(
				lpFileName.get(),
				dwDesiredAccess,
				dwShareMode,
				nullptr,
				dwCreationDisposition,
				dwFlagsAndAttributes,
				nullptr
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD);
			bufops::Write<HANDLE_DATA_TYPE>(buf, ToHandleDataType(result));
			if (result == INVALID_HANDLE_VALUE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::CreateFileW:
		{
			const size_t param_size = sizeof(uint32_t) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD);
			const size_t fixed_response_size = sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const uint32_t lpFileName_len = bufops::Read<uint32_t>(buf);
			const DWORD dwDesiredAccess = bufops::Read<DWORD>(buf);
			const DWORD dwShareMode = bufops::Read<DWORD>(buf);
			const DWORD dwCreationDisposition = bufops::Read<DWORD>(buf);
			const DWORD dwFlagsAndAttributes = bufops::Read<DWORD>(buf);
			EXPECT_EQ(buf, data_buf + param_size);

			std::unique_ptr<wchar_t[]> lpFileName(new wchar_t[lpFileName_len+1]);
			if (!ReceiveData(socket, lpFileName.get(), lpFileName_len * sizeof(wchar_t)))
				return false;
			lpFileName[lpFileName_len] = 0;

			const HANDLE result = CreateFileW(
				lpFileName.get(),
				dwDesiredAccess,
				dwShareMode,
				nullptr,
				dwCreationDisposition,
				dwFlagsAndAttributes,
				nullptr
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD);
			bufops::Write<HANDLE_DATA_TYPE>(buf, ToHandleDataType(result));
			if (result == INVALID_HANDLE_VALUE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::CloseHandle:
		{
			const size_t param_size = sizeof(HANDLE_DATA_TYPE);
			const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const HANDLE hObject = ToHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));
			EXPECT_EQ(buf, data_buf + param_size);

			const BOOL result = CloseHandle(
				hObject
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD);
			bufops::Write<BOOL>(buf, result);
			if (result == FALSE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::ReadFile:
		{
			const size_t param_size = sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD) + sizeof(uint8_t);
			const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const HANDLE hFile = ToHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));
			const DWORD nNumberOfBytesToRead = bufops::Read<DWORD>(buf);
			const bool lpNumberOfBytesRead_is_null = bufops::Read<uint8_t>(buf) == 0;
			EXPECT_EQ(buf, data_buf + param_size);

			std::unique_ptr<uint8_t[]> lpBuffer(new uint8_t[nNumberOfBytesToRead]);
			DWORD lpNumberOfBytesRead{};
			const BOOL result = ReadFile(
				hFile,
				lpBuffer.get(),
				nNumberOfBytesToRead,
				lpNumberOfBytesRead_is_null ? nullptr : &lpNumberOfBytesRead,
				nullptr
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD) + (lpNumberOfBytesRead_is_null ? 0 : sizeof(DWORD));
			bufops::Write<BOOL>(buf, result);
			if (result == FALSE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				if (!lpNumberOfBytesRead_is_null) bufops::Write<DWORD>(buf, lpNumberOfBytesRead);
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
			if (!SendData(socket, lpBuffer.get(), nNumberOfBytesToRead))
				return false;
		}
		return true;

		case common::ApiTag::WriteFile:
		{
			const size_t param_size = sizeof(HANDLE_DATA_TYPE) + sizeof(DWORD) + sizeof(uint8_t);
			const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const HANDLE hFile = ToHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));
			const DWORD nNumberOfBytesToWrite = bufops::Read<DWORD>(buf);
			const bool lpNumberOfBytesWritten_is_null = bufops::Read<uint8_t>(buf) == 0;
			EXPECT_EQ(buf, data_buf + param_size);

			std::unique_ptr<uint8_t[]> lpBuffer(new uint8_t[nNumberOfBytesToWrite]);
			if (!ReceiveData(socket, lpBuffer.get(), nNumberOfBytesToWrite))
				return false;
			DWORD lpNumberOfBytesWritten{};
			const BOOL result = WriteFile(
				hFile,
				lpBuffer.get(),
				nNumberOfBytesToWrite,
				lpNumberOfBytesWritten_is_null ? nullptr : &lpNumberOfBytesWritten,
				nullptr
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD) + (lpNumberOfBytesWritten_is_null ? 0 : sizeof(DWORD));
			bufops::Write<BOOL>(buf, result);
			if (result == FALSE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				if (!lpNumberOfBytesWritten_is_null) bufops::Write<DWORD>(buf, lpNumberOfBytesWritten);
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::GetFileSize:
		{
			const size_t param_size = sizeof(HANDLE_DATA_TYPE) + sizeof(uint8_t);
			const size_t fixed_response_size = sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const HANDLE hFile = ToHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));
			const bool lpFileSizeHigh_is_null = bufops::Read<uint8_t>(buf) == 0;
			EXPECT_EQ(buf, data_buf + param_size);

			DWORD lpFileSizeHigh{};
			const DWORD result = GetFileSize(
				hFile,
				lpFileSizeHigh_is_null ? nullptr : &lpFileSizeHigh
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(DWORD) + sizeof(DWORD) + (lpFileSizeHigh_is_null ? 0 : sizeof(DWORD));
			bufops::Write<DWORD>(buf, result);
			if (result == INVALID_FILE_SIZE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				if (!lpFileSizeHigh_is_null) bufops::Write<DWORD>(buf, lpFileSizeHigh);
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::GetFileSizeEx:
		{
			const size_t param_size = sizeof(HANDLE_DATA_TYPE) + sizeof(uint8_t);
			const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD) + sizeof(LARGE_INTEGER);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const HANDLE hFile = ToHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));
			const bool lpFileSize_is_null = bufops::Read<uint8_t>(buf) == 0;
			EXPECT_EQ(buf, data_buf + param_size);

			LARGE_INTEGER lpFileSize{};
			const BOOL result = GetFileSizeEx(
				hFile,
				lpFileSize_is_null ? nullptr : &lpFileSize
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD) + (lpFileSize_is_null ? 0 : sizeof(LARGE_INTEGER));
			bufops::Write<BOOL>(buf, result);
			if (result == INVALID_FILE_SIZE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				if (!lpFileSize_is_null) bufops::Write<LARGE_INTEGER>(buf, lpFileSize);
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::SetFilePointerEx:
		{
			const size_t param_size = sizeof(HANDLE_DATA_TYPE) + sizeof(LARGE_INTEGER) + sizeof(uint8_t) + sizeof(DWORD);
			const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD) + sizeof(LARGE_INTEGER);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const HANDLE hFile = ToHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));
			const LARGE_INTEGER liDistanceToMove = bufops::Read<LARGE_INTEGER>(buf);
			const bool lpNewFilePointer_is_null = bufops::Read<uint8_t>(buf) == 0;
			const DWORD dwMoveMethod = bufops::Read<DWORD>(buf);
			EXPECT_EQ(buf, data_buf + param_size);

			LARGE_INTEGER lpNewFilePointer{};
			const BOOL result = SetFilePointerEx(
				hFile,
				liDistanceToMove,
				lpNewFilePointer_is_null ? nullptr : &lpNewFilePointer,
				dwMoveMethod
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD) + (lpNewFilePointer_is_null ? 0 : sizeof(LARGE_INTEGER));
			bufops::Write<BOOL>(buf, result);
			if (result == FALSE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				if (!lpNewFilePointer_is_null) bufops::Write<LARGE_INTEGER>(buf, lpNewFilePointer);
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::SetFilePointer:
		{
			const size_t param_size = sizeof(HANDLE_DATA_TYPE) + sizeof(LONG) + sizeof(uint8_t) + sizeof(LONG) + sizeof(DWORD);
			const size_t fixed_response_size = sizeof(DWORD) + sizeof(DWORD) + sizeof(LONG);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const HANDLE hFile = ToHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));
			const LONG lDistanceToMove = bufops::Read<LONG>(buf);
			const bool lpDistanceToMoveHigh_is_null = bufops::Read<uint8_t>(buf) == 0;
			const DWORD dwMoveMethod = bufops::Read<DWORD>(buf);
			EXPECT_EQ(buf, data_buf + param_size);

			LONG lpDistanceToMoveHigh{};
			const DWORD result = SetFilePointer(
				hFile,
				lDistanceToMove,
				lpDistanceToMoveHigh_is_null ? nullptr : &lpDistanceToMoveHigh,
				dwMoveMethod
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(DWORD) + sizeof(DWORD) + (lpDistanceToMoveHigh_is_null ? 0 : sizeof(LONG));
			bufops::Write<DWORD>(buf, result);
			if (result == -1)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				if (!lpDistanceToMoveHigh_is_null) bufops::Write<LONG>(buf, lpDistanceToMoveHigh);
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::CreateDirectoryA:
		{
			const size_t param_size = sizeof(uint32_t);
			const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const uint32_t lpPathName_len = bufops::Read<uint32_t>(buf);
			EXPECT_EQ(buf, data_buf + param_size);

			std::unique_ptr<char[]> lpPathName(new char[lpPathName_len+1]);
			if (!ReceiveData(socket, lpPathName.get(), lpPathName_len))
				return false;
			lpPathName[lpPathName_len] = 0;

			const BOOL result = CreateDirectoryA(
				lpPathName.get(),
				nullptr
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD);
			bufops::Write<BOOL>(buf, result);
			if (result == FALSE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::CreateDirectoryW:
		{
			const size_t param_size = sizeof(uint32_t);
			const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const uint32_t lpPathName_len = bufops::Read<uint32_t>(buf);
			EXPECT_EQ(buf, data_buf + param_size);

			std::unique_ptr<wchar_t[]> lpPathName(new wchar_t[lpPathName_len+1]);
			if (!ReceiveData(socket, lpPathName.get(), lpPathName_len * sizeof(wchar_t)))
				return false;
			lpPathName[lpPathName_len] = 0;

			const BOOL result = CreateDirectoryW(
				lpPathName.get(),
				nullptr
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD);
			bufops::Write<BOOL>(buf, result);
			if (result == FALSE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::DeleteFileA:
		{
			const size_t param_size = sizeof(uint32_t);
			const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const uint32_t lpFileName_len = bufops::Read<uint32_t>(buf);
			EXPECT_EQ(buf, data_buf + param_size);

			std::unique_ptr<char[]> lpFileName(new char[lpFileName_len+1]);
			if (!ReceiveData(socket, lpFileName.get(), lpFileName_len))
				return false;
			lpFileName[lpFileName_len] = 0;

			const BOOL result = DeleteFileA(
				lpFileName.get()
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD);
			bufops::Write<BOOL>(buf, result);
			if (result == FALSE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::DeleteFileW:
		{
			const size_t param_size = sizeof(uint32_t);
			const size_t fixed_response_size = sizeof(BOOL) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const uint32_t lpFileName_len = bufops::Read<uint32_t>(buf);
			EXPECT_EQ(buf, data_buf + param_size);

			std::unique_ptr<wchar_t[]> lpFileName(new wchar_t[lpFileName_len+1]);
			if (!ReceiveData(socket, lpFileName.get(), lpFileName_len * sizeof(wchar_t)))
				return false;
			lpFileName[lpFileName_len] = 0;

			const BOOL result = DeleteFileW(
				lpFileName.get()
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(BOOL) + sizeof(DWORD);
			bufops::Write<BOOL>(buf, result);
			if (result == FALSE)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::GetFileAttributesA:
		{
			const size_t param_size = sizeof(uint32_t);
			const size_t fixed_response_size = sizeof(DWORD) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const uint32_t lpFileName_len = bufops::Read<uint32_t>(buf);
			EXPECT_EQ(buf, data_buf + param_size);

			std::unique_ptr<char[]> lpFileName(new char[lpFileName_len+1]);
			if (!ReceiveData(socket, lpFileName.get(), lpFileName_len))
				return false;
			lpFileName[lpFileName_len] = 0;

			const DWORD result = GetFileAttributesA(
				lpFileName.get()
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(DWORD) + sizeof(DWORD);
			bufops::Write<DWORD>(buf, result);
			if (result == -1)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::GetFileAttributesW:
		{
			const size_t param_size = sizeof(uint32_t);
			const size_t fixed_response_size = sizeof(DWORD) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const uint32_t lpFileName_len = bufops::Read<uint32_t>(buf);
			EXPECT_EQ(buf, data_buf + param_size);

			std::unique_ptr<wchar_t[]> lpFileName(new wchar_t[lpFileName_len+1]);
			if (!ReceiveData(socket, lpFileName.get(), lpFileName_len * sizeof(wchar_t)))
				return false;
			lpFileName[lpFileName_len] = 0;

			const DWORD result = GetFileAttributesW(
				lpFileName.get()
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(DWORD) + sizeof(DWORD);
			bufops::Write<DWORD>(buf, result);
			if (result == -1)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;

		case common::ApiTag::GetFileType:
		{
			const size_t param_size = sizeof(HANDLE_DATA_TYPE);
			const size_t fixed_response_size = sizeof(DWORD) + sizeof(DWORD);
			char data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];
			if (!ReceiveData(socket, data_buf, param_size))
				return false;

			char *buf = data_buf;
			const HANDLE hFile = ToHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));
			EXPECT_EQ(buf, data_buf + param_size);

			const DWORD result = GetFileType(
				hFile
			);

			buf = data_buf;
			const size_t dynamic_response_size = sizeof(DWORD) + sizeof(DWORD);
			bufops::Write<DWORD>(buf, result);
			if (result == -1)
			{
				bufops::Write<DWORD>(buf, GetLastError());
			}
			else
			{
				EXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));
			}

			if (!SendData(socket, data_buf, dynamic_response_size))
				return false;
		}
		return true;


        default:
            ASSERT(false, "Unknown tag code %d", tag);
    }

    return true;
}

} // namespace master
