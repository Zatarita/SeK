/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/

#include "include/EndianStream/endian_writer.h"



namespace SysIO
{
    EndianWriter::EndianWriter(std::string_view path, const ByteOrder& endianness) :
        fileEndianness(endianness)
    { 
        this->open(path); 
    }

    EndianWriter::EndianWriter(const ByteOrder& endianness) : 
        fileEndianness(endianness) 
    {}

    EndianWriter::~EndianWriter()
    {
        this->close();
    }

    void EndianWriter::open(std::string_view path)
    {
        if (this->is_open()) this->close();

        file.open(std::string(path).c_str(), std::ios_base::binary);
        if (!this->is_open()) throw std::runtime_error(EXCEPTION_FILE_ACCESS);
    }

    void EndianWriter::close()
    {
        file.close();
    }

    bool EndianWriter::is_open()
    {
        return file.is_open();
    }

    void EndianWriter::seek(const size_t& offset)
    {
        file.seekp(offset);
    }

    void EndianWriter::pad(const size_t& n)
    {
        ByteArray temp(n, {});
        writeRaw( { temp } );
    }

    const size_t EndianWriter::tell()
    {
        return file.tellp();
    }

    void EndianWriter::writeString(const std::string& str, const bool& nullTerminated)
    {
        file.write(str.data(), str.size());
        if(nullTerminated)
            file.write(new char{'\0'}, 1);
    }

    void EndianWriter::writeRaw(ByteView raw)
    {
        file.write(reinterpret_cast<const char*>( raw.data()), raw.size() );
    }
}
