#include "include/EndianStream/byte_writer.h"

namespace SysIO
{
	ByteWriter::ByteWriter(ByteArray& data, const ByteOrder& byteorder) :
		rawData(data),
		endianness(byteorder)
	{}

	ByteWriter::ByteWriter(const size_t& size, const ByteOrder& byteorder) :
		rawData(ByteArray( size, {} )),
		endianness(byteorder)
	{}

	void ByteWriter::writeString(ByteView stream, size_t& position, std::string_view str)
	{
		std::memcpy(stream.data() + position, str.data(), str.size());
		position += str.size();
	}

	template <class type> void ByteWriter::write(const type& data)
	{
		endianPlace(rawData, streamPos, data);
	}

	void ByteWriter::writeString(const std::string_view str)
	{
		writeString(rawData, streamPos, str);
	}

	ByteWriter& ByteWriter::operator<<(std::string_view data)
	{
		writeString(data);
		return *this;
	}

	ByteWriter& ByteWriter::operator<<(const std::string& data)
	{
		writeString({ data });
		return *this;
	}

	ByteWriter& ByteWriter::operator<<(const char* data)
	{
		writeString({ data });
		return *this;
	}

	void ByteWriter::seek(const size_t& pos)
	{
		streamPos = pos;
	}

	const size_t& ByteWriter::tell() const
	{
		return streamPos;
	}

	void ByteWriter::reserve(const size_t& size)
	{
		rawData.resize(size);
	}

	const ByteView& ByteWriter::getData()
	{
		return { rawData };
	}
}