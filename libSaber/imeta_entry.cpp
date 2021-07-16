#include "include/libSaber/imeta_entry.h"

ImetaEntry::ImetaEntry()
{
	name.resize(NAME_LEN);
}

void ImetaEntry::writeHeader(SysIO::EndianWriter& stream)
{
	if (name.size() != NAME_LEN) name.resize(NAME_LEN);
	uint32_t adjSize = size - META_DATA_SIZE; // Adjust the size to be just the pixel data

	stream.writeString(name);

	stream.pad(0x8);
	stream << uint32_t(0x1)
		<< dimensions.width << dimensions.height << dimensions.depth
		<< mipmapCount << faceCount << format;
	stream.pad(0x8);

	/* This is messy in the file, A lot of redundant data and padding */
	uint32_t pad32{};
	stream << adjSize << pad32
		<< adjSize << offset << pad32
		<< adjSize << pad32;
}

void ImetaEntry::readHeader(SysIO::ByteReader& stream)
{
	name = stream.getString(NAME_LEN);
	stream.pad(0xC);
	stream >> dimensions.width >> dimensions.height >> dimensions.depth;
	stream >> mipmapCount >> faceCount >> format;
	stream.pad(0x10);
	stream >> size >> offset;
	stream.pad(0xC);

	// Size in the ipak is only the raw pixel data. Add this to account for metadata
	size += META_DATA_SIZE;

	// Name size is 0x100. I trim off the trailling zeros to make it easier to work with, and resize at save.
	name.erase(name.find_last_not_of('\0') + 1);
}

uint32_t ImetaEntry::getHeaderSize()
{
	return ENTRY_SIZE; // Ipak header entries fixed size
}