/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/

#ifndef ESTREAM
#define ESTREAM

/// Includes for the EndianReader and EndianWriter classes.

#include "EndianStream\endian_reader.h"
#include "EndianStream\endian_writer.h"
#include "EndianStream\sys_io.h"

#include <string_view>

SysIO::EndianReader LEndianReader(std::string_view path)
{
    return SysIO::EndianReader(path, SysIO::ByteOrder::Little);
}

SysIO::EndianReader BEndianReader(std::string_view path)
{
    return SysIO::EndianReader(path, SysIO::ByteOrder::Big);
}

SysIO::EndianWriter LEndianWriter(std::string_view path)
{
    return SysIO::EndianWriter(path, SysIO::ByteOrder::Little);
}

SysIO::EndianWriter BEndianWriter(std::string_view path)
{
    return SysIO::EndianWriter(path, SysIO::ByteOrder::Big);
}

#endif // ESTREAM_H_INCLUDED
