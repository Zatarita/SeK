#include "include/EndianStream/byte_reader.h"

namespace SysIO
{
	ByteReader::ByteReader(const ByteView& data, const ByteOrder& byteorder) :
		rawData(data),
		endianness(byteorder)
	{}

	std::string ByteReader::getString(std::optional<size_t> size)
	{
		return readString(rawData, streamPos, size);
	}

	ByteReader& ByteReader::operator>>(std::string& data) 
	{
		data = readString(rawData, streamPos, std::nullopt);
		return *this;
	}

	void ByteReader::seek(const size_t& pos)
	{
		if (streamPos > rawData.size())
			streamPos = rawData.size();
		else
			streamPos = pos;
	}

	void ByteReader::pad(const size_t& size)
	{
		if (streamPos + size > rawData.size())
			streamPos = rawData.size();
		else
			streamPos += size;
	}

	const size_t& ByteReader::tell() const
	{
		return streamPos;
	}

	const size_t& ByteReader::getFilesize() const
	{
		return rawData.size();
	}

	ByteArray ByteReader::readRaw(size_t size)
	{
		if (streamPos + size > rawData.size())
			size = rawData.size();

		return ByteArray(rawData.begin() + streamPos, rawData.begin() + streamPos + size);
	}

	std::string ByteReader::readString(const ByteView& stream, size_t& position, std::optional<size_t> size)
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
			ret.resize(stringLength);
			std::memcpy(ret.data(), stream.data() + position, stringLength);
		}

		position += stringLength;
		return ret;
	}
}