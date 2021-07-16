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


    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- Stream State
    void EndianWriter::open(std::string_view path)
    {
        if ( this->isOpen() ) this->close();

        file.open(std::string(path).c_str(), std::ios_base::binary);
        this->isOpen();
    }

    bool EndianWriter::isOpen()
    {
        if (file.is_open())
            return true;
        this->setException(EXCEPTION_FILE_ACCESS);
        return false;
    }

    void EndianWriter::close()
    {
        file.close();
    }

    void EndianWriter::setEndianness(const SysIO::ByteOrder& newEndianness)
    {
        fileEndianness = newEndianness;
    }

    void EndianWriter::seek(const size_t& offset)
    {
        file.seekp(offset);
    }

    void EndianWriter::pad(const size_t& n)
    {
        ByteArray padding(n, {});
        writeRaw( { padding } );
    }

    const size_t EndianWriter::tell()
    {
        return file.tellp();
    }

    void EndianWriter::writeString(std::string_view str, const bool& nullTerminated)
    {
        file.write( str.data(), str.size() );
        if(nullTerminated)
            file.write( new char{'\0'}, 1 );
    }

    void EndianWriter::writeRaw(ByteView raw)
    {
        file.write(reinterpret_cast<const char*>(raw.data()), raw.size());
    }

    void EndianWriter::writeRaw(const ByteArray& raw)
    {
        file.write(reinterpret_cast<const char*>(raw.data()), raw.size());
    }
}
