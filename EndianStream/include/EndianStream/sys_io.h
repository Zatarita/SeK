/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/

#ifndef SYSIO
#define SYSIO

#include <algorithm>
#include <cstddef>
#include <vector>

using std::byte; // cstddef
using ByteArray = std::vector<byte>;

namespace SysIO
{
	enum class ByteOrder : unsigned char
	{
		Little,
		Big
	};

	ByteOrder getSystemEndianness();

	static const ByteOrder systemEndianness{ getSystemEndianness() };

	template <class type>
	void EndianSwap(type& data)
	{
		byte* rawData = new(&data) byte[sizeof(type)];
		std::reverse(rawData, rawData + sizeof(type));
	}

	template <class type>
	type EndianCast(type data)
	{
		byte* rawData = new(&data) byte[sizeof(type)];
		std::reverse(rawData, rawData + sizeof(type));
		return data;
	}
}


#endif
