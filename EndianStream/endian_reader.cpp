/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/
#include <iostream>
#include "include/EndianStream/endian_reader.h"

namespace SysIO
{
    EndianReader::EndianReader(std::string_view path, const ByteOrder& endianness) : fileEndianness(endianness)
    {
        this->open(path);
    }

    EndianReader::EndianReader(const ByteOrder& endianness) : fileEndianness(endianness)
    {}

    EndianReader::~EndianReader()
    {
        // cleanup
        this->close();
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- Stream State
    void EndianReader::open(std::string_view path)
    {
        // If a file is open, close it.
        this->close();
        file.open( static_cast<std::string>(path), std::ios_base::in | std::ios_base::binary );

        this->isOpen(); // Set EXCEPTION_STATUS if file didnt open.
    }

    void EndianReader::close()
    {
        // Close the stream
        if ( this->isOpen() ) file.close();
    }

    bool EndianReader::isOpen()
    {
        if (file.is_open())
            return true;
        this->setException(EXCEPTION_FILE_ACCESS);
        return false;
    }

    void EndianReader::setEndianness(const SysIO::ByteOrder& newEndianness)
    {
        fileEndianness = newEndianness;
    }

    const size_t& EndianReader::getFileSize() noexcept
    {
        if ( !this->isOpen() ) return (fileSize = 0, fileSize);
        const size_t init_pos{ this->tell() };

        if (!fileSize)
        {
            file.seekg(0, std::ios_base::end);
            fileSize = file.tellg();
            file.seekg(init_pos);
        }

        return fileSize;
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- Stream Manipulation
    void EndianReader::seek(const size_t& offset, const std::ios_base::seekdir& dir)
    {
        if ( !this->isOpen() ) return;

        // If requested offset exceeds the bounds of the file size. something is wrong.
        if(offset > this->getFileSize()) EXCEPTION_STATUS = EXCEPTION_FILE_BOUNDS;

        file.seekg(offset, dir);
    }

    const bool EndianReader::isInBounds(const size_t& offset)
    {
        if (offset < this->getFileSize())
            return true;
        this->setException(EXCEPTION_FILE_BOUNDS);
        return false;
    }

    const size_t EndianReader::tell()
    {
        if ( !this->isOpen() ) return 0;

        // Return the current position in the ifstream
        return file.tellg();
    }

    void EndianReader::pad( const size_t& n )
    {
        // Skip over padding by seeking to the current position + padding size
        try { this->seek(tell() + n); }
        catch (...) {}  // If except, we've reached eof. No need to actually except.

        return;
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- Stream Access
    std::string EndianReader::readString( const size_t& size )
    {
        if (!this->isOpen()) return "";

        // Create a string large enough to hold the data.
        std::string ret(size, '\0');

        // Read into, and return the string.
        file.read( ret.data(), size );
        return ret;
    }

    std::string EndianReader::readString()
    {
        if ( !this->isOpen() ) return "";

        std::string ret;
        char buffer;

        // Try to reduce memory reallocations by reserving 32 chars.
        ret.reserve(DEFAULT_STRING_LENGTH);

        // MAXIMUM_STRING_LENGTH prevents infinite loop
        for(size_t i = 0; i <= MAXIMUM_STRING_LENGTH; ++i)
        {
            if (i == MAXIMUM_STRING_LENGTH)
            {
                EXCEPTION_STATUS = EXCEPTION_RECURSION;
                return "";
            }
            // Read a char at a time until we reach a null terminator.
            file.read(&buffer, 1);
            if(buffer == '\0')
                break;
            ret.push_back(buffer);
        };

        return ret;
    }

    ByteArray EndianReader::readRaw(const size_t& offset, size_t n)
    {
        // If there is no file open, or the base offset exceeds the bounds of the file
        if ( !this->isOpen() || !this->isInBounds(offset) ) return ByteArray();

        // If the requested data starts in the file, but exceeds the end of the file, adjust n to be remaining bytes to eof
        if ( !this->isInBounds(offset + n) ) n = this->getFileSize() - offset;

        // Store the original position, and create a vector<byte> of the right size
        const size_t initPos { this->tell() };
        ByteArray ret(n);

        // Seek to the beginning of the data, Read the data, then return to initPos
        this->seek(offset);
        file.read(reinterpret_cast<char*>(ret.data()), n);
        this->seek(initPos);

        // Note if the entire chunk wasn't read, the getException function will return a EXCEPTION_FILE_BOUNDS, but will return fine.
        return ret;
    }

    ByteArray EndianReader::readRaw(size_t n)
    {
        if (!this->isOpen()) return ByteArray();

        // If the requested data exceeds file size, just read to end.
        if (this->tell() + n > this->getFileSize()) n = this->getFileSize() - this->tell();

        // Create a ByteArray of 'n' width. Read from the file into byte array
        ByteArray ret(n);
        file.read(reinterpret_cast<char*>(ret.data()), n);

        return ret;
    }

    std::shared_ptr<ByteArray> EndianReader::get(size_t offset, size_t size)
    {
        return std::make_shared<ByteArray>( readRaw(offset,size) );
    }

    /*template <class T> T EndianReader::read()
    {
        T ret{};
        this->readInto(ret);
        return ret;
    }*/
}
