#pragma once

template<typename T> 
struct AtScopeExiter
{
	T exit_func;

	AtScopeExiter(T t) : exit_func(std::move(t)) {}
	~AtScopeExiter() { exit_func(); }
};

#define AT_SCOPE_EXIT_NAME2(a, b) a##b
#define AT_SCOPE_EXIT_NAME(a, b) AT_SCOPE_EXIT_NAME2(a, b)

#define AT_SCOPE_EXIT(func)	\
	auto AT_SCOPE_EXIT_NAME(scope_exit_func_, __LINE__) = func;\
	AtScopeExiter<decltype(AT_SCOPE_EXIT_NAME(scope_exit_func_, __LINE__))> AT_SCOPE_EXIT_NAME(scope_exit_caller_, __LINE__)(std::move(AT_SCOPE_EXIT_NAME(scope_exit_func_, __LINE__)))
