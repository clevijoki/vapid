#pragma once
/* This file is generated from generated_overloaded.py, do not edit directly */
#include "detours/detours.h"

namespace detours
{
    // save all original pointers
    void Inject()
    {
		worker::os::CreateFileA = &::CreateFileA;
		worker::os::CreateFileW = &::CreateFileW;
		worker::os::CloseHandle = &::CloseHandle;
		worker::os::ReadFile = &::ReadFile;
		worker::os::ReadFileEx = &::ReadFileEx;
		worker::os::WriteFile = &::WriteFile;
		worker::os::WriteFileEx = &::WriteFileEx;
		worker::os::GetFileSize = &::GetFileSize;
		worker::os::GetFileSizeEx = &::GetFileSizeEx;
		worker::os::SetFilePointerEx = &::SetFilePointerEx;
		worker::os::SetFilePointer = &::SetFilePointer;
		worker::os::CreateDirectoryA = &::CreateDirectoryA;
		worker::os::CreateDirectoryW = &::CreateDirectoryW;
		worker::os::DeleteFileA = &::DeleteFileA;
		worker::os::DeleteFileW = &::DeleteFileW;
		worker::os::GetFileAttributesA = &::GetFileAttributesA;
		worker::os::GetFileAttributesW = &::GetFileAttributesW;
		worker::os::GetFileType = &::GetFileType;

        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)worker::os::CreateFileA, worker::CreateFileA);
		DetourAttach(&(PVOID&)worker::os::CreateFileW, worker::CreateFileW);
		DetourAttach(&(PVOID&)worker::os::CloseHandle, worker::CloseHandle);
		DetourAttach(&(PVOID&)worker::os::ReadFile, worker::ReadFile);
		DetourAttach(&(PVOID&)worker::os::ReadFileEx, worker::ReadFileEx);
		DetourAttach(&(PVOID&)worker::os::WriteFile, worker::WriteFile);
		DetourAttach(&(PVOID&)worker::os::WriteFileEx, worker::WriteFileEx);
		DetourAttach(&(PVOID&)worker::os::GetFileSize, worker::GetFileSize);
		DetourAttach(&(PVOID&)worker::os::GetFileSizeEx, worker::GetFileSizeEx);
		DetourAttach(&(PVOID&)worker::os::SetFilePointerEx, worker::SetFilePointerEx);
		DetourAttach(&(PVOID&)worker::os::SetFilePointer, worker::SetFilePointer);
		DetourAttach(&(PVOID&)worker::os::CreateDirectoryA, worker::CreateDirectoryA);
		DetourAttach(&(PVOID&)worker::os::CreateDirectoryW, worker::CreateDirectoryW);
		DetourAttach(&(PVOID&)worker::os::DeleteFileA, worker::DeleteFileA);
		DetourAttach(&(PVOID&)worker::os::DeleteFileW, worker::DeleteFileW);
		DetourAttach(&(PVOID&)worker::os::GetFileAttributesA, worker::GetFileAttributesA);
		DetourAttach(&(PVOID&)worker::os::GetFileAttributesW, worker::GetFileAttributesW);
		DetourAttach(&(PVOID&)worker::os::GetFileType, worker::GetFileType);

        DetourTransactionCommit();
    }

    // undo detours
    void UndoInject()
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)worker::os::CreateFileA, worker::CreateFileA);
		DetourDetach(&(PVOID&)worker::os::CreateFileW, worker::CreateFileW);
		DetourDetach(&(PVOID&)worker::os::CloseHandle, worker::CloseHandle);
		DetourDetach(&(PVOID&)worker::os::ReadFile, worker::ReadFile);
		DetourDetach(&(PVOID&)worker::os::ReadFileEx, worker::ReadFileEx);
		DetourDetach(&(PVOID&)worker::os::WriteFile, worker::WriteFile);
		DetourDetach(&(PVOID&)worker::os::WriteFileEx, worker::WriteFileEx);
		DetourDetach(&(PVOID&)worker::os::GetFileSize, worker::GetFileSize);
		DetourDetach(&(PVOID&)worker::os::GetFileSizeEx, worker::GetFileSizeEx);
		DetourDetach(&(PVOID&)worker::os::SetFilePointerEx, worker::SetFilePointerEx);
		DetourDetach(&(PVOID&)worker::os::SetFilePointer, worker::SetFilePointer);
		DetourDetach(&(PVOID&)worker::os::CreateDirectoryA, worker::CreateDirectoryA);
		DetourDetach(&(PVOID&)worker::os::CreateDirectoryW, worker::CreateDirectoryW);
		DetourDetach(&(PVOID&)worker::os::DeleteFileA, worker::DeleteFileA);
		DetourDetach(&(PVOID&)worker::os::DeleteFileW, worker::DeleteFileW);
		DetourDetach(&(PVOID&)worker::os::GetFileAttributesA, worker::GetFileAttributesA);
		DetourDetach(&(PVOID&)worker::os::GetFileAttributesW, worker::GetFileAttributesW);
		DetourDetach(&(PVOID&)worker::os::GetFileType, worker::GetFileType);

        DetourTransactionCommit();

    }
}

