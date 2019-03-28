#pragma once

inline void DebugLog(const wchar_t* fmt, ...)
{
	wchar_t buf[2048] = {0};

	va_list args;
	va_start(args, fmt);
	vswprintf_s(buf, _countof(buf), fmt, args);
	OutputDebugStringW(buf);
	fputws(buf, stdout);
	va_end(args);
}

inline void DebugLog(const char* fmt, ...)
{
	char buf[2048] = {0};

	va_list args;
	va_start(args, fmt);
	vsprintf_s(buf, _countof(buf), fmt, args);
	OutputDebugStringA(buf);
	fputs(buf, stdout);
	va_end(args);
}