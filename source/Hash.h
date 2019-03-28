#pragma once
#include <memory>
#include <string>

union Hash
{
	uint8_t bytes[32];
	uint16_t words[16];
	uint32_t dwords[8];
	uint64_t qwords[4];
};

inline bool operator==(const Hash& lhs, const Hash& rhs)
{
	return 
		lhs.qwords[0] == rhs.qwords[0] &&
		lhs.qwords[1] == rhs.qwords[1] &&
		lhs.qwords[2] == rhs.qwords[2] &&
		lhs.qwords[3] == rhs.qwords[3];
}

inline bool operator!=(const Hash& lhs, const Hash& rhs)
{
	return !(lhs == rhs);
}

inline bool operator<(const Hash& lhs, const Hash& rhs)
{
	return lhs.qwords[0] < rhs.qwords[0] ||
		lhs.qwords[1] < rhs.qwords[1] ||
		lhs.qwords[2] < rhs.qwords[2] ||
		lhs.qwords[3] < rhs.qwords[3];
}


// defined in cpp
Hash HashBytes(const void* data, size_t data_size);

// utilities
inline Hash HashStdString(const std::string& str) { return HashBytes(str.c_str(), str.size()); }
template<size_t LEN> inline Hash HashString(const char (&str)[LEN]) { return HashBytes(str, LEN-1); }
inline Hash HashString(const char* str) { return HashBytes( str, strlen(str) ); }


namespace std
{
	std::string to_string(const Hash& val);
}
