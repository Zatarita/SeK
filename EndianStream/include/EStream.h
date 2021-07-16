/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/

#ifndef ESTREAM
#define ESTREAM

/// Includes for the EndianReader and EndianWriter classes.

#include "EndianStream\endian_reader.h"
#include "EndianStream\endian_writer.h"
#include "EndianStream\byte_writer.h"
#include "EndianStream\byte_reader.h"
#include "EndianStream\sys_io.h"

#include <string_view>

static SysIO::EndianReader LEndianReader(std::string_view path)
{
    return SysIO::EndianReader(path, SysIO::ByteOrder::Little);
}

static SysIO::EndianReader BEndianReader(std::string_view path)
{
    return SysIO::EndianReader(path, SysIO::ByteOrder::Big);
}

static SysIO::EndianWriter LEndianWriter(std::string_view path)
{
    return SysIO::EndianWriter(path, SysIO::ByteOrder::Little);
}

static SysIO::EndianWriter BEndianWriter(std::string_view path)
{
    return SysIO::EndianWriter(path, SysIO::ByteOrder::Big);
}

// -- utility functions ( These don't really fit anywhere particular; however, are highlevel functions -- //
ByteArray ByteArrayFromFile(std::string_view path)
{
    auto stream = LEndianReader(path);
    return stream.readRaw(stream.getFileSize());
}

#endif // ESTREAM_H_INCLUDED
