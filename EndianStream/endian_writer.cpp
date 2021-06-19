/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/

#include "include/EndianStream/endian_writer.h"

namespace SysIO
{
    EndianWriter::EndianWriter(const std::string& path, const ByteOrder& endianness) : fileEndianness(endianness)
    {
        file.open(path, std::ios_base::binary);
        if(!file.is_open())
            throw std::runtime_error(EXCEPTION_FILE_ACCESS);
    }

    EndianWriter::~EndianWriter()
    {
        close();
    }

    void EndianWriter::close()
    {
        file.close();
    }

    void EndianWriter::seek(const size_t& offset)
    {
        file.seekp(offset);
    }

    void EndianWriter::pad(const size_t& n)
    {
        file.seekp(tell() + n);
    }

    size_t EndianWriter::tell()
    {
        return file.tellp();
    }

    void EndianWriter::writeString(const std::string& str, const bool& nullTerminated)
    {
        file.write(str.data(), str.size());
        if(nullTerminated)
            file.write(new char{'\0'}, 1);
    }

    void EndianWriter::writeRaw(const ByteArray& raw)
    {
        file.write(reinterpret_cast<const char*>(raw.data()), raw.size());
    }
}
