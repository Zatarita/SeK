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
	class EndianReader
	{
	    public:
	    // Exceptions/Constraints
	    static constexpr const char* EXCEPTION_FILE_ACCESS {"Unable To Access Requested File."};
	    static constexpr const char* EXCEPTION_FILE_BOUNDS {"Requested Offset Exceeds Bounds Of The File."};
        static const uint16_t        MAXIMUM_STRING_LENGTH {0xffff};

	    size_t        fileSize {};
		std::ifstream file {};
		ByteOrder     fileEndianness {};
	public:
		EndianReader(const std::string&, const ByteOrder&);
		~EndianReader();

        void   seek(const size_t&);                      // Goto a specific offset
        void   pad (const size_t&);                      // Burn padding
        size_t tell();                                   // Get current position

        std::string readString(const size_t&);           // Fixed length
        std::string readString();                        // Null terminated

        ByteArray readRaw(const size_t&, const size_t&); // Seek, read, and seek back
        ByteArray readRaw(const size_t&);                // Read from current position

		// Template Functions
		template <class type>
		type read()
		{
		    // Read data of a certain type and return it.
		    type ret;
		    *this >> ret;
            return ret;
		}

		template <class type>
		type peek()
		{
		    // Read data of a certain type, seek the stream back, and return the data
		    type ret;
		    *this >> ret;

		    seek( tell() - sizeof(type) );

		    return ret;
		}

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
