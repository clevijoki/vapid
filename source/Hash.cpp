#include "WindowsInclude.h"
#include "Hash.h"
#include "Assert.h"
#include "picosha2.h"

Hash HashBytes(const void* data, size_t data_size)
{
	Hash result;

	picosha2::hash256(
		reinterpret_cast<const uint8_t*>(data), 
		reinterpret_cast<const uint8_t*>(data) + data_size, 
		result.bytes, 
		result.bytes + sizeof(result.bytes));

	return result;
}

std::string std::to_string(const Hash& val)
{
	std::string result;
	result.reserve(64);

	const char vals[] = "0123456789abcdef";

	for (uint8_t byte : val.bytes)
	{
		result.push_back(vals[byte&0xF]);
		result.push_back(vals[(byte>>4)&0xF]);
	}

	return result;
}