#ifndef BYTEREADER
#define BYTEREADER
#include "sys_io.h"

namespace SysIO
{
	class ByteReader : public StreamInputObject
	{
		ByteView rawData;
		ByteOrder endianness;
		mutable size_t streamPos{};

	public:
		ByteReader(const ByteView& data, const ByteOrder& byteorder = ByteOrder::Little);

		std::string   getString(std::optional<size_t> size);
		void	      seek(const size_t& pos);
		void		  pad(const size_t& size);
		const size_t& tell() const;
		const size_t& getFilesize() const;

		ByteArray readRaw(size_t size);

		// read data
		template <class type> type read()
		{
			return this->endianGet<type>(rawData, streamPos, endianness);
		}

		template <class type> type peek() const
		{
			type ret{ this->read() };
			streamPos -= sizeof(type);
			return ret;
		}

		// read data >> wrapper
		template <class type> ByteReader& operator>>(type& data)
		{
			data = this->endianGet<type>(rawData, streamPos, endianness);
			return *this;
		}
		ByteReader& operator>>(std::string& data);


	// Static Functions
		template <class type>
		static type endianGet(const ByteView& stream, const size_t& position, const SysIO::ByteOrder& endianness = ByteOrder::Little)
		{
			if (position > stream.size())
				return type();
			type newData = *(new(stream.data() + position) type);
			if (endianness != systemEndianness)
				EndianSwap(newData);

			const_cast<size_t&>(position) += sizeof(type);
			return newData;
		}

		static std::string readString(const ByteView& stream, size_t& position, std::optional<size_t> size);
	};
}

#endif
