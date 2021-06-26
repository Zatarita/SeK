/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/

#ifndef SYSIO
#define SYSIO

#include <algorithm>
#include <cstddef>
#include <vector>
#include <span>
#include <string>
#include <string.h>
#include <optional>

using std::byte;
using ByteArray = std::vector<byte>;
using ByteView  = std::span<byte>;

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

	template <class type>
	static bool endianPlace(ByteView stream, const size_t& position, const type& data, const ByteOrder& endianness = ByteOrder::Little)
	{
		if (position > stream.size() || position + sizeof(type) > stream.size())
			return false;
		type* place = new(stream.data() + position) type{ data };
		if (endianness != systemEndianness)
			EndianSwap(place);
	}

	template <class type>
	static type endianGet(ByteView stream, size_t& position, const ByteOrder& endianness = ByteOrder::Little)
	{
		if (position > stream.size())
			return false;
		type newData = *( new(stream.data() + position) type );
		if (endianness != systemEndianness)
			EndianSwap(newData);

		position += sizeof(type);
		return newData;
	}

	static std::string readString(ByteView stream, size_t& position, std::optional<size_t> size)
	{
		std::string ret;
		size_t stringLength{};

		if (size)
		{
			stringLength = *size;
			ret.resize(stringLength);
			std::memcpy(ret.data(), stream.data() + position, stringLength);
		}
		else
		{
			for (;; ++stringLength)
			{
				if (position + stringLength > stream.size())
					return "";
				if (stream[position + stringLength] == std::byte{ '\0' })
					break;
			}
			std::memcpy(ret.data(), stream.data() + position, stringLength);
		}

		position += stringLength;
		return ret;
	}

	struct byteReader
	{
		ByteView rawData;
		ByteOrder endianness;
		size_t streamPos{};

		byteReader(const ByteView& data, const ByteOrder& byteorder = ByteOrder::Little) :
			rawData(data),
			endianness(byteorder)
		{}

		template <class type>
		type read()
		{
			return SysIO::endianGet(rawData, streamPos, endianness);
		}

		std::string getString(std::optional<size_t> size)
		{
			return SysIO::readString(rawData, streamPos, size);
		}

		template <class type>
		byteReader& operator<<(type& data)
		{
			data = SysIO::endianGet(rawData, streamPos, endianness);
			return *this;
		}

		byteReader& operator<<(std::string& data)
		{
			data = SysIO::readString(rawData, streamPos, std::nullopt);
		}

		void seek(const size_t& pos)
		{ streamPos = pos; }

		const size_t& tell()
		{ return streamPos; }
	};
}


#endif
