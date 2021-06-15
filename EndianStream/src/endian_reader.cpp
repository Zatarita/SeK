/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/

#include "endian_reader.h"

namespace SysIO
{
    EndianReader::EndianReader(const std::string& path, const ByteOrder& endianness) : fileEndianness(endianness)
    {
        // Try and open file
        file.open(path, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
        if(!file.is_open())
            throw std::runtime_error(EXCEPTION_FILE_ACCESS);

        // Get the file size if successful.
        fileSize = file.tellg();
        file.seekg(0);
        return;
    }

    EndianReader::~EndianReader()
    {
        // cleanup
        file.close();
    }

    void EndianReader::seek(const size_t& offset)
    {
        // If requested offset exceeds the bounds of the file size. something is wrong.
        if(offset > fileSize)
            throw std::runtime_error(EXCEPTION_FILE_BOUNDS);
        // If not seek and return;
        file.seekg(offset);
        return;
    }

    void EndianReader::pad(const size_t& n)
    {
        // Skip over padding by seeking to the current position + padding size
        seek( tell() + n );
        return;
    }

    size_t EndianReader::tell()
    {
        // Return the current position in the ifstream
        return file.tellg();
    }

    std::string EndianReader::readString(const size_t& size)
    {
        // Create a string large enough to hold the data.
        std::string ret(size, '\0');

        // Read into, and return the string.
        file.read(ret.data(), size);
        return ret;
    }

    std::string EndianReader::readString()
    {
        std::string ret;
        char buffer;

        // For loop prevents infinite loop
        for(size_t i = 0; i < MAXIMUM_STRING_LENGTH; ++i)
        {
            // Read a char at a time until we reach a null terminator.
            file.read(&buffer, 1);
            if(buffer == '\0')
                break;
            ret.push_back(buffer);
        };

        return ret;
    }

    ByteArray EndianReader::readRaw(const size_t& offset, const size_t& n)
    {
        // Store the original position, and create a vector<byte> of the right size
        const size_t initPos { tell() };
        ByteArray ret(n);

        // If the end of the block exceeds the file size, we'll end up with an overflow.
        if(offset + n > fileSize)
            throw std::runtime_error(EXCEPTION_FILE_BOUNDS);

        // Seek to the beginning of the data, Read the data, then return to initPos
        seek(offset);
        file.read(reinterpret_cast<char*>(ret.data()), n);
        seek(initPos);

        return ret;
    }

    ByteArray EndianReader::readRaw(const size_t& n)
    {
        // If the end of the block exceeds the file size, we'll end up with an overflow.
        if(tell() + n > fileSize)
            throw std::runtime_error(EXCEPTION_FILE_BOUNDS);

        // Create a ByteArray of 'size' width. Read from the file into byte array
        ByteArray ret(n);
        file.read(reinterpret_cast<char*>(ret.data()), n);

        return ret;
    }
}
