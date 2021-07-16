#ifndef SABERGENERICENTRY
#define SABERGENERICENTRY
#include <string_view>

#include "MccCompress.h"


template <class offset_t>
class SaberGenericEntry
{
	static_assert(std::is_integral<offset_t>(), "offset_t Type Must be an Integer.");
protected:
	offset_t	 offset{};
	uint32_t	 size{};
	std::string  name{};

	mutable ByteArray rawData{};
	mutable bool hasData{};

public:
	template<class DecObj_t = CEADecObj>
	const ByteArray& getData(DecObj_t& stream) const
	{
		// Verify
		static_assert(std::is_base_of<SysIO::StreamInputObject, DecObj_t>(), "DecObj_t must be a supported stream type.");

		// If we already have the data return it
		if (hasData) return rawData;

		// If not try and read the data from the stream.
		if (auto ret = stream.get(offset, size); ret)
		{
			rawData = *ret;
			hasData = true;
			return rawData;
		}
		else
		{
			// exception
			return ByteArray();
		}
	}

	/**
	 * \brief
	 * Changes the data assigned to the entry. Reassigns the size variable
	 */
	virtual void setData(const ByteArray& newData)
	{
		rawData = newData;
		size = rawData.size();

		if(!newData.empty())
			hasData = true;
	}

	const std::string& getName() const
	{
		return name;
	}

	virtual void setName(const std::string& newName)
	{
		name = newName;
	}

	const uint32_t& getSize() const
	{
		return size;
	}

	const offset_t& getOffset() const
	{
		return offset;
	}

	void setOffset(offset_t& newOffset)
	{
		offset = newOffset;
	}

	// Formats the header info and writes is to file
	virtual void writeHeader(SysIO::EndianWriter&) = 0;
	// Reads the data from chunk read from file
	virtual void readHeader(SysIO::ByteReader&) = 0;

	// returns the header size to use for offset calculations
	virtual uint32_t getHeaderSize() = 0;
};

#endif // !SABERGENERICENTRY
