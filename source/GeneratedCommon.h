#pragma once
/* This file is generated from generated_overloaded.py, do not edit directly */

namespace common
{
    enum class ApiTag : uint8_t
    {
		CreateFileA,
		CreateFileW,
		CloseHandle,
		ReadFile,
		ReadFileEx,
		WriteFile,
		WriteFileEx,
		GetFileSize,
		GetFileSizeEx,
		SetFilePointerEx,
		SetFilePointer,
		CreateDirectoryA,
		CreateDirectoryW,
		DeleteFileA,
		DeleteFileW,
		GetFileAttributesA,
		GetFileAttributesW,
		GetFileType,

    };
}

typedef int64_t HANDLE_DATA_TYPE;

inline HANDLE_DATA_TYPE ToHandleDataType(HANDLE handle)
{
    return (HANDLE_DATA_TYPE)handle;
}

inline HANDLE ToHandle(HANDLE_DATA_TYPE handle)
{
    return (HANDLE)handle;
}

const HANDLE_DATA_TYPE INVALID_HANDLE_DATA_TYPE = ToHandleDataType(INVALID_HANDLE_VALUE);


