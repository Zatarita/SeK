/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/

#ifndef ENDIANWRITER
#define ENDIANWRITER
#include "sys_io.h"

#include <fstream>
#include <exception>

namespace SysIO
{
    class EndianWriter
    {
        // Exceptions
        static constexpr const char* EXCEPTION_FILE_ACCESS {"Unable To Access Requested File."};

        std::ofstream file {};
        ByteOrder     fileEndianness {};
    public:
        EndianWriter(const std::string&, const ByteOrder&);
        ~EndianWriter();

        void   seek(const size_t&);                                 // Seek to a specific position in stream
        void   pad (const size_t&);                                 // Burn padding
        size_t tell();                                              // Get current position in stream

        void writeString(const std::string&, const bool& = false);  // Write a string. possibly null terminated
        void writeRaw   (const ByteArray&);                         // Write raw data from ByteArray (defined in sys_io.h)

        // Template Functions
        template <class type>
        void write(type data)
        {
            // swap the endianness if needed, then write the data
            if (SysIO::systemEndianness != fileEndianness)
				SysIO::EndianSwap(data);
            file.write(reinterpret_cast<char*>(&data), sizeof(type));
        }

        template <class type>
        EndianWriter& operator<<(const type& data)
        {
            // wrapper for write function
            write(data);
            return *this;
        }

        template <class type>
        void writeAt(const type& data, const size_t& offset)
        {
            // Store initial position, then seek to desired offset.
            const size_t initialPos {tell()};
            seek(offset);
            // write the data, and return to where we were at the start.
            write(data);
            seek(initialPos);
        }
    };
}

#endif // ENDIANWRITER
