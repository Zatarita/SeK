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
#include <string_view>
#include <string.h>
#include <optional>

using std::byte;
using ByteArray = std::vector<byte>;
using ByteView  = std::span<byte>;

const static inline ByteArray EMPTY_ARRAY;

namespace SysIO
{
	class StreamTypeObject {}; // Helper class for type validation
	class StreamInputObject : StreamTypeObject {}; // Helper class for type validation
	class StreamOutputObject : StreamTypeObject {}; // Helper class for type validation

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

	class StreamExcept
	{
	private:
		const char*      EXCEPTION_STATUS{ nullptr };
	public:
		void			 setException(const char*) noexcept;
		const bool       hasException() const noexcept;
		std::string_view getException() const noexcept;
		std::string_view releaseException() noexcept;
		void             clearException() noexcept;
	};
}


#endif
