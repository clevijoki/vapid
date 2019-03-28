#pragma once
#include "Assert.h"

template<typename T>
inline void DefaultMutexProtectedResourceDeleter(T& res)
{
	res.~T();
}

BOOL MutexProtectedResourceCloseHandle(HANDLE);

enum ENoInit { NoInit };

template<typename T, void DELETER(T&)=DefaultMutexProtectedResourceDeleter>
class MutexProtectedResource
{
	union { T resource; };
	HANDLE mutex = INVALID_HANDLE_VALUE;

public:

	MutexProtectedResource(ENoInit)
	{
	}

	MutexProtectedResource(const char* name = nullptr)
	{
		new (&resource) T();
		mutex = CreateMutex(nullptr, FALSE, name);

		EXPECT_NE(INVALID_HANDLE_VALUE, mutex);
	}

	MutexProtectedResource(T res, const char* name=nullptr)
	{
		Initialize(std::move(res), name);
	}

	void Initialize(T res, const char* name=nullptr)
	{
		new (&resource) T(res);
		mutex = CreateMutex(nullptr, FALSE, name);

		EXPECT_NE(INVALID_HANDLE_VALUE, mutex);
	}

	~MutexProtectedResource()
	{
		release();
	}

	void release()
	{
		if (mutex != INVALID_HANDLE_VALUE)
		{
			DELETER(resource);
			
			EXPECT_EQ(TRUE, MutexProtectedResourceCloseHandle(mutex));
			mutex = INVALID_HANDLE_VALUE;
		}		
	}

	class Lock
	{
		friend class MutexProtectedResource;

		T* resource = nullptr;
		HANDLE mutex = INVALID_HANDLE_VALUE;
	public:
		Lock()
		{
		}

		Lock(Lock&& rhs)
		: resource(rhs.resource)
		, mutex(rhs.mutex)
		{
			rhs.mutex = INVALID_HANDLE_VALUE;
			rhs.resource = nullptr;
		}

		~Lock()
		{
			release();
		}

		void operator=(Lock&& rhs)
		{
			release();
			new (this) Lock(std::move(rhs));
		}


		void release()
		{
			if (mutex != INVALID_HANDLE_VALUE)
			{
				EXPECT_NE(FALSE, ReleaseMutex(mutex));
				resource = nullptr;
				mutex = INVALID_HANDLE_VALUE;
			}
		}

		T* get() const { return resource; }
		T* operator->() const { return resource; }
		T& operator*() const { return *resource; }
	};

	Lock lock()
	{
		Lock result;
		result.resource = &resource;
		result.mutex = mutex;

		EXPECT_EQ(WAIT_OBJECT_0, WaitForSingleObject(mutex, INFINITE));

		return result;
	}
};
