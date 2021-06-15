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
    /** @brief
    * A stream object that is aware of the endianness of the system, and the endianness of the file being written.
    * the stream can translate the data to any desired endianness as it writes the data.
    **/
    class EndianWriter
    {
        // Exceptions
        /// @brief EXCEPTION_FILE_ACCESS - "Unable To Access Requested File."
        static constexpr const char* EXCEPTION_FILE_ACCESS {"Unable To Access Requested File."};

        /// Stream access to the file being written to.
        std::ofstream file {};
        /// Endianness of the file, assigned at construction
        ByteOrder     fileEndianness {};
    public:
        /// @brief prepare a file for writing, and designate the endianness of the stream
        /// @param std::string Path - File the stream is designated to write to
        /// @param ByteOrder Endianness - Endianness of the file in question
        EndianWriter(const std::string&, const ByteOrder&);
        /// @brief Cleanup ofstream
        ~EndianWriter();

        /// @brief Goto a specific offset
        /// @param size_t Offset - Offset to the new stream position
        void   seek(const size_t&);
        /// @brief Treats n bytes as padding, skipping over them
        /// @param size_t n - Number of bytes as padding
        void   pad (const size_t&);
        /// @brief Gets the current position in the stream
        /// @return size_t - Stream position
        size_t tell();

        /// @brief Write a string to the stream. Null terminating if desired
        /// @param std::string str - String to write to the stream
        /// @param bool nullTerminated - adds the null terminator to the end of the write
        void writeString(const std::string&, const bool& = false);
        /// @brief Write raw data to file from ByteArray (ByteArray is a vector<std::byte>)
        /// @param ByteArray data - Raw data to write to file
        void writeRaw   (const ByteArray&);

        // Template Functions
        /// @brief Write some data to file. Adjusted for endianness if required
        /// @param type data - Data to write to file
        /// @tparam type - Template type
        template <class type>
        void write(type data)
        {
            // swap the endianness if needed, then write the data
            if (SysIO::systemEndianness != fileEndianness)
				SysIO::EndianSwap(data);
            file.write(reinterpret_cast<char*>(&data), sizeof(type));
        }

        /// @brief Write wrapper for << override
        /// @param type data - Data to write to file
        /// @tparam type - Template type
        template <class type>
        EndianWriter& operator<<(const type& data)
        {
            // wrapper for write function
            write(data);
            return *this;
        }

        /// @brief Write some data to file at a specific offset. Adjusted for endianness if required (note stream position remains unchanged)
        /// @param type data - Data to write to file
        /// @param size_t offset - Offset to write the data at
        /// @tparam type - Template type
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
