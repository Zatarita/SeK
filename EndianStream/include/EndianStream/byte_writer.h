#ifndef BYTEWRITER
#define BYTEWRITER
#include "sys_io.h"

namespace SysIO
{
	class ByteWriter : public StreamOutputObject
	{
		ByteArray rawData;
		ByteOrder endianness;
		size_t streamPos{};
	public:
		ByteWriter(ByteArray& data, const ByteOrder& byteorder = ByteOrder::Little);
		ByteWriter(const size_t& size, const ByteOrder& byteorder = ByteOrder::Little);

		template <class type> void write(const type& data);
		void writeString(const std::string_view str);

		template <class type> ByteWriter& operator<<(const type& data)
		{
			endianPlace(rawData, streamPos, data);
			return *this;
		}

		/// \todo Update to concepts once c++20 has better support
		ByteWriter& operator<<(std::string_view data);
		ByteWriter& operator<<(const std::string& data);
		ByteWriter& operator<<(const char* data);

		// Change "stream" position
		void seek(const size_t& pos);
		// Find out "stream" position
		const size_t& tell() const;

		void reserve(const size_t&);

		const ByteView& getData();


		// write data
		template <class type>
		static void endianPlace(ByteView stream, size_t& position, const type& data, const ByteOrder& endianness = ByteOrder::Little)
		{
			if (position > stream.size() || position + sizeof(type) > stream.size())
				return;
			type* place = new(stream.data() + position) type{ data };
			if (endianness != systemEndianness)
				EndianSwap(place);
			position += sizeof(type);
		}

		template <class type>
		static void endianPlace(ByteView stream, const size_t& position, const type& data, const ByteOrder& endianness = ByteOrder::Little)
		{
			if (position > stream.size() || position + sizeof(type) > stream.size())
				return;
			type* place = new(stream.data() + position) type{ data };
			if (endianness != systemEndianness)
				EndianSwap(place);
		}

		// write string
		static void writeString(ByteView stream, size_t& position, std::string_view str);
	};
}

#endif
