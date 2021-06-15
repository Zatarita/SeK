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
	/// @brief Valid System Endianness Options
	enum class ByteOrder : unsigned char
	{
		Little,
		Big
	};

	/// @brief	Determines system endianness.
	/// @return	ByteOrder - System endianness
	ByteOrder getSystemEndianness();

	/// @brief	Stores the system endianness to be checked against at runtime.
	static const ByteOrder systemEndianness{ getSystemEndianness() };

	/// @brief Swaps the endianness for the passed parameter
	/// @tparam Type - Type of the object.
	/// @param Type data - Reference to the memory location to swap.
	template <class type>
	void EndianSwap(type& data)
	{
		byte* rawData = new(&data) byte[sizeof(type)];
		std::reverse(rawData, rawData + sizeof(type));
	}

	/// @brief Swaps the endianness. Creates a copy of the original value. Like EndianSwap, but non-destructive
	/// @tparam Type - Type of the object.
	/// @param Type data - Reference to the memory location to swap.
	/// @return type - Swapped data
	template <class type>
	type EndianCast(type data)
	{
		byte* rawData = new(&data) byte[sizeof(type)];
		std::reverse(rawData, rawData + sizeof(type));
		return data;
	}
}


#endif
