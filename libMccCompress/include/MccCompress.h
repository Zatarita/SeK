#ifndef LIBMCCCOMPRESS
#define LIBMCCCOMPRESS

#include "libMccCompress/DecompressionObject.h"
#include "libMccCompress/CompressionObject.h"
#include "libMccCompress/shared.h"

using CEADecObj  = Compression::DecompressionObject<uint32_t, Compression::ChunkType::H1A>;
using H2ADecObj  = Compression::DecompressionObject<uint64_t, Compression::ChunkType::H2A>;
using H2AMDecObj = Compression::DecompressionObject<uint32_t, Compression::ChunkType::H2AM>;

using CEACompObj  = Compression::CompressionObject<uint32_t, Compression::ChunkType::H1A>;
using H2ACompObj  = Compression::CompressionObject<uint64_t, Compression::ChunkType::H2A>;
using H2AMCompObj = Compression::CompressionObject<uint32_t, Compression::ChunkType::H2AM>;

static CEADecObj H1ADecompressionObject(std::string_view path)
{
    return CEADecObj(path);
}

static H2ADecObj H2ADecompressionObject(std::string_view path)
{
    return H2ADecObj(path);
}

static H2AMDecObj H2AMDecompressionObject(std::string_view path)
{
    return H2AMDecObj(path);
}


static CEACompObj H1ACompressionObject()
{
	return CEACompObj();
}

static H2ACompObj H2ACompressionObject()
{
	return H2ACompObj();
}

static H2AMCompObj H2AMCompressionObject()
{
	return H2AMCompObj();
}

#endif // LIBMCCCOMPRESS_H_INCLUDED
