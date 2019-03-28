#pragma once

//#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>

#include <Windows.h>

#include <string>

inline std::string Win32ErrorToString(int ErrorCode)
{
    static CHAR Message[1024];

    // If this program was multithreaded, we'd want to use
    // FORMAT_MESSAGE_ALLOCATE_BUFFER instead of a static buffer here.
    // (And of course, free the buffer when we were done with it)

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
                  FORMAT_MESSAGE_MAX_WIDTH_MASK,
                  NULL, ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  Message, 1024, NULL);

    return Message;
}

inline std::string Win32LastErrorToString()
{
	return Win32ErrorToString(GetLastError());
}

inline std::string WSALastErrorToString()
{
	return Win32ErrorToString(WSAGetLastError());
}

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS 0
#endif