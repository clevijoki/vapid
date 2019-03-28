#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "DebugLog.h"

inline void AssertFunc(const char* expr)
{
	DebugLog("ASSERT: %s", expr);
}

inline void AssertFunc(const char* expr, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	char tmp[2048];
	vsnprintf_s(tmp, sizeof(tmp), fmt, args);
	va_end(args);

	DebugLog("ASSERT: %s: %s\n", expr, tmp);
}

template<typename T>
struct ToStringHelper
{
	static std::string run(T t)
	{
		return std::to_string(t);
	}
};

template<typename T>
struct ToStringHelper<const T*>
{
	static std::string run(const T* t)
	{
		char tmp[64];
		snprintf(tmp, sizeof(tmp), "%p", t);
		return tmp;
	}
};

template<typename T>
struct ToStringHelper<T*>
{
	static std::string run(T* t)
	{
		char tmp[64];
		snprintf(tmp, sizeof(tmp), "%p", t);
		return tmp;
	}
};

template<>
struct ToStringHelper<std::string>
{
	static std::string run(const std::string& t)
	{
		return t;
	}
};

template<typename LHS_TYPE, typename RHS_TYPE>
inline bool ExpectEQHelper(LHS_TYPE lhs, RHS_TYPE rhs, const char* lhs_str, const char* rhs_str, const char* line, const int line_num) 
{
	if (lhs == rhs)
		return true;

	DebugLog("%s(%d): Expected ('%s' which is '%s') == ('%s' which is '%s')\n",
		line, line_num, lhs_str, ToStringHelper<LHS_TYPE>::run(lhs).c_str(), rhs_str, ToStringHelper<RHS_TYPE>::run(rhs).c_str());

	return false;
}

template<typename LHS_TYPE, typename RHS_TYPE>
inline bool ExpectNEHelper(LHS_TYPE lhs, RHS_TYPE rhs, const char* lhs_str, const char* rhs_str, const char* line, const int line_num) 
{
	if (lhs != rhs)
		return true;

	DebugLog("%s(%d): Expected '%s' != '%s', which is '%s'\n",
		line, line_num, lhs_str, ToStringHelper<LHS_TYPE>::run(lhs).c_str(), rhs_str, ToStringHelper<RHS_TYPE>::run(rhs).c_str());
	
	return false;
}

template<typename LHS_TYPE, typename RHS_TYPE>
inline bool ExpectGTHelper(LHS_TYPE lhs, RHS_TYPE rhs, const char* lhs_str, const char* rhs_str, const char* line, const int line_num) 
{
	if (lhs > rhs)
		return true;

	DebugLog("%s(%d): Expected ('%s' which is '%s') > ('%s' which is '%s')\n",
		line, line_num, lhs_str, ToStringHelper<LHS_TYPE>::run(lhs).c_str(), rhs_str, ToStringHelper<RHS_TYPE>::run(rhs).c_str());
	
	return false;
}

template<typename LHS_TYPE, typename RHS_TYPE>
inline bool ExpectGTEHelper(LHS_TYPE lhs, RHS_TYPE rhs, const char* lhs_str, const char* rhs_str, const char* line, const int line_num) 
{
	if (lhs >= rhs)
		return true;

	DebugLog("%s(%d): Expected ('%s' which is '%s') >= ('%s' which is '%s')\n",
		line, line_num, lhs_str, ToStringHelper<LHS_TYPE>::run(lhs).c_str(), rhs_str, ToStringHelper<RHS_TYPE>::run(rhs).c_str());
	
	return false;
}

template<typename LHS_TYPE, typename RHS_TYPE>
inline bool ExpectLTHelper(LHS_TYPE lhs, RHS_TYPE rhs, const char* lhs_str, const char* rhs_str, const char* line, const int line_num) 
{
	if (lhs < rhs)
		return true;

	DebugLog("%s(%d): Expected ('%s' which is '%s') < ('%s' which is '%s')\n",
		line, line_num, lhs_str, ToStringHelper<LHS_TYPE>::run(lhs).c_str(), rhs_str, ToStringHelper<RHS_TYPE>::run(rhs).c_str());
	
	return false;
}

template<typename LHS_TYPE, typename RHS_TYPE>
inline bool ExpectLTEHelper(LHS_TYPE lhs, RHS_TYPE rhs, const char* lhs_str, const char* rhs_str, const char* line, const int line_num) 
{
	if (lhs <= rhs)
		return true;

	DebugLog("%s(%d): Expected ('%s' which is '%s') <= ('%s' which is '%s')\n",
		line, line_num, lhs_str, ToStringHelper<LHS_TYPE>::run(lhs).c_str(), rhs_str, ToStringHelper<RHS_TYPE>::run(rhs).c_str());
	
	return false;
}


template<typename TYPE>
inline bool ExpectNULLHelper(TYPE val, const char* val_str, const char* line, const int line_num) 
{
	if (val == nullptr)
		return true;

	DebugLog("%s(%d): Expected ('%s' which is '%s') to be null\n",
		line, line_num, val_str, ToStringHelper<TYPE>::run(val).c_str());
	
	return false;
}


#define TO_STR2(x) #x
#define TO_STR(x) TO_STR2(x)

#define ASSERT(x, ...) do { if (!(x)) { AssertFunc(__FILE__ "(" TO_STR(__LINE__) ")" #x , __VA_ARGS__); DebugBreak(); } } while(0)
#define VERIFY(x, ...) do { if (!(x)) { AssertFunc(__FILE__ "(" TO_STR(__LINE__) ")" #x, __VA_ARGS__); DebugBreak(); } } while(0)

#define EXPECT_EQ(lhs, rhs) do { if (!ExpectEQHelper((lhs), (rhs), #lhs, #rhs, __FILE__, __LINE__)) DebugBreak(); } while (0)
#define EXPECT_NE(lhs, rhs) do { if (!ExpectNEHelper((lhs), (rhs), #lhs, #rhs, __FILE__, __LINE__)) DebugBreak(); } while (0)
#define EXPECT_GT(lhs, rhs) do { if (!ExpectGTHelper((lhs), (rhs), #lhs, #rhs, __FILE__, __LINE__)) DebugBreak(); } while (0)
#define EXPECT_GTE(lhs, rhs) do { if (!ExpectGTEHelper((lhs), (rhs), #lhs, #rhs, __FILE__, __LINE__)) DebugBreak(); } while (0)
#define EXPECT_LT(lhs, rhs) do { if (!ExpectLTHelper((lhs), (rhs), #lhs, #rhs, __FILE__, __LINE__)) DebugBreak(); } while (0)
#define EXPECT_LTE(lhs, rhs) do { if (!ExpectLTEHelper((lhs), (rhs), #lhs, #rhs, __FILE__, __LINE__)) DebugBreak(); } while (0)
#define EXPECT_NULL(value) do { if (!ExpectNULLHelper((value), #value, __FILE__, __LINE__)) DebugBreak(); } while (0)

#define CHECK_RETURN(x, ...) if (!x) return __VA_ARGS__;
