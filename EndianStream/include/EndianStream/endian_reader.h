/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/

#ifndef ENDIANREADER
#define ENDIANREADER
#include "sys_io.h"

#include <fstream>
#include <string>
#include <exception>

namespace SysIO
{
	/** @brief
	* A stream object that is aware of the endianness of the system, and the endianness of the file being read.
	* the stream can translate the data to the systems native endianness as it reads in the data.
	**/
	class EndianReader
	{
	    public:
		/// EXCEPTION_FILE_ACCESS - "Unable To Access Requested File."
	    static constexpr const char* EXCEPTION_FILE_ACCESS {"Unable To Access Requested File."};
		/// EXCEPTION_FILE_BOUNDS - "Requested Offset Exceeds Bounds Of The File."
	    static constexpr const char* EXCEPTION_FILE_BOUNDS {"Requested Offset Exceeds Bounds Of The File."};
		/// MAXIMUM_STRING_LENGTH - 0xffff (65535)
        static const uint16_t        MAXIMUM_STRING_LENGTH {0xffff};

		/// Size of the file on disk
	    size_t        fileSize {};
		/// Stream access to the file being read.
		std::ifstream file {};
		/// Endianness of the file, assigned at construction
		ByteOrder     fileEndianness {};
	public:
		/// @brief Load a file, and designate the endianness of the stream
		/// @param std::string Path - File the stream is designated to read
		/// @param ByteOrder Endianness - Endianness of the file in question
		EndianReader(const std::string&, const ByteOrder&);
		/// @brief Cleanup ifstream
		~EndianReader();

		/// @brief close the stream
		void   close();

		/// @brief Goto a specific offset
		/// @param size_t Offset - Offset to the new stream position
        void   seek(const size_t&);
		/// @brief Treats n bytes as padding, skipping over them
		/// @param size_t n - Number of bytes as padding
        void   pad (const size_t&);
		/// @brief Gets the current position in the stream
		/// @return size_t - Stream position
        size_t tell();

		/// @brief Read a fixed length string from the stream
		/// @param size_t size - Size of the string
		/// @return std::string - String read from stream
        std::string readString(const size_t&);
		/// @brief Read a null terminated string from the stream
		/// @return std::string - String read from stream
        std::string readString();

		/// @brief Seek to an offset, read n bytes, and seek back original position
		/// @param size_t offset - Offset to start read from
		/// @param size_t n - Number of bytes to read
		/// @return ByteArray - Range of bytes requested
        ByteArray readRaw(const size_t&, const size_t&);
		/// @brief Read n bytes from current position
		/// @param size_t n - Number for bytes to read
		/// @return ByteArray - Range of bytes requested
        ByteArray readRaw(const size_t&);

		// Template Functions
		/// @brief Read some data from the stream. Creates a new instance of type
		/// @tparam type - Template type
		/// @return type - A new instance of 'type' read from the stream and adjusted for endianness
		template <class type>
		type read()
		{
		    // Read data of a certain type and return it.
		    type ret;
		    *this >> ret;
            return ret;
		}

		/// @brief Read some data from the stream, without updating stream position. Creates a new instance of type.
		/// @tparam type - Template Type 
		/// @return type - A new instance of 'type' read from the stream and adjusted for endianness
		template <class type>
		type peek()
		{
		    // Read data of a certain type, seek the stream back, and return the data
		    type ret;
		    *this >> ret;

		    seek( tell() - sizeof(type) );

		    return ret;
		}

		/// @brief Read some data from the stream into an existing object.
		/// @tparam type - Template type
		/// @param data - Destination for the data read in from stream
		/// @return EndianReader& - Returns it's self for chaining of >> operators
		template <class type>
		EndianReader& operator>>(type& data)
		{
		    // Read data from the stream, swap the endianness if needed.
			file.read(reinterpret_cast<char*>( &data ), sizeof(data));
			if (SysIO::systemEndianness != fileEndianness)
				SysIO::EndianSwap(data);
            return *this;
		};
	};
}
#endif
