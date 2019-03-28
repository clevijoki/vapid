
#pragma once

namespace bufops
{
	inline void ReadBuffer(char*& buf, void *data, size_t size)
	{
		memcpy(data, buf, size);
		buf += size;
	}

	inline void WriteBuffer(char*& buf, const void *data, size_t size)
	{
		memcpy(buf, data, size);
		buf += size;
	}
		
	template<size_t SIZE>
	void ReadBuffer(char*& buf, void *data)
	{
		memcpy(data, buf, SIZE);
		buf += SIZE;
	}

	template<size_t SIZE>
	void WriteBuffer(char*& buf, const void *data)
	{
		memcpy(buf, data, SIZE);
		buf += SIZE;
	}

	template<typename ... Ts>
	struct PackedSize
	{
	};

	template<typename T>
	struct PackedSize<T>
	{
		enum { Result = sizeof(T) };
	};

	template<typename T, typename... Args>
	struct PackedSize<T, Args...>
	{
		enum { Result = sizeof(T) + PackedSize<Args...>::Result };
	};

	inline void Write(char *&buf)
	{
	}

	template<typename T, typename... Args>
	void Write(char *&buf, const T& value, Args... args)
	{
		WriteBuffer<sizeof(T)>(buf, &value);
		Write(buf, args...);
	}


	template<typename ... Ts>
	struct PackedPtrSize
	{
	};

	template<typename T>
	constexpr size_t SizeOfPtrType(const T*) { return sizeof(T); }

	template<typename T>
	struct PackedPtrSize<T>
	{
		enum { Result = SizeOfPtrType((T)0) };
	};

	template<typename T, typename... Args>
	struct PackedPtrSize<T, Args...>
	{
		enum { Result = SizeOfPtrType((T)0) + PackedPtrSize<Args...>::Result };
	};

	inline void ReadElements(char *&buf)
	{
	}

	template<typename T, typename... Args>
	void ReadElements(char *&buf, T* value, Args... args)
	{
		ReadBuffer<sizeof(T)>(buf, value);
		ReadElements(buf, args...);
	}

	template<typename T>
	T Read(char *& buf)
	{
		T result;
		ReadBuffer<sizeof(T)>(buf, &result);
		return result;
	}

}