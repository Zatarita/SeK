/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/

#ifndef ENDIANREADER
#define ENDIANREADER
#include "sys_io.h"

#include <fstream>
#include <string>
#include <string_view>
#include <exception>

namespace SysIO
{
	/** @brief
	* A stream object that is aware of the endianness of the system, and the endianness of the file being read.
	* the stream can translate the data to the systems native endianness as it reads in the data.
	**/
	class EndianReader : public StreamExcept, public StreamInputObject
	{
		/// EXCEPTION_FILE_ACCESS - "Unable To Access Requested File."
	    static constexpr const char* EXCEPTION_FILE_ACCESS { "[EXCEPTION_FILE_ACCESS] Unable To Access Requested File." };
		/// EXCEPTION_FILE_BOUNDS - "Requested Offset Exceeds Bounds Of The File."
		static constexpr const char* EXCEPTION_FILE_BOUNDS { "[EXCEPTION_FILE_BOUNDS] Requested Offset Exceeds Bounds Of The File." };
		/// EXCEPTION_FILE_BOUNDS - "Requested Offset Exceeds Bounds Of The File."
		static constexpr const char* EXCEPTION_RECURSION   { "[EXCEPTION_RECURSION] Maximum Recursion Reached While Attempting to Read String" };
		/// MAXIMUM_STRING_LENGTH - 0xffff (65535)
		static const uint32_t        MAXIMUM_STRING_LENGTH{ 0xffffffff };
		/// DEFAULT_STRING_LENGTH - 0x20 (32)
		static const size_t	         DEFAULT_STRING_LENGTH{ 0x20 };
		/// Pointer to last exception
		const char*					 EXCEPTION_STATUS{ nullptr };

		/// Size of the file on disk
	    size_t        fileSize {};
		/// Stream access to the file being read.
		std::ifstream file {};
		/// Endianness of the file, assigned at construction
		ByteOrder     fileEndianness {};

	public:
		/// @brief Constructor wrapping open()
		/// @param std::string_view Path - File the stream is designated to read
		/// @param ByteOrder Endianness - Endianness of the file in question
		EndianReader(std::string_view, const ByteOrder& = SysIO::ByteOrder::Little);
		/// @brief default constructor
		EndianReader(const ByteOrder&);

		/// @brief Cleanup ifstream
		~EndianReader();


		/// @brief Load a file, and designate the endianness of the stream
		/// @param std::string Path - File the stream is designated to read
		/// @param ByteOrder Endianness - Endianness of the file in question
		void open(std::string_view);
		/// @brief close the stream
		void close();
		/// @brief Tells if the underlying stream is currently open (also sets EXCEPTION_FILE_ACCESS on failure)
		bool isOpen();
		/// @brief (re)assigns the file endianness
		void setEndianness(const SysIO::ByteOrder&);
		/// @brief Gets the file size, or calculates it if it hasn't already. (must always return a value)
		const size_t& getFileSize() noexcept;

		/// @brief Goto a specific offset
		/// @param size_t Offset - Offset to the new stream position
		/// @param size_t Dir - Seek direction. default beginning
        void  seek(const size_t&, const std::ios_base::seekdir& = std::ios_base::beg);
		/// @brief Validates a specific offset is within the bounds.
		/// @param size_t - offset to validate
		/// @return const bool - If offset violated bounds. (also sets EXCEPTION_STATUS on failure)
		const bool isInBounds(const size_t&);
		/// @brief Gets the current position in the stream
		/// @return size_t - Stream position
		const size_t tell();
		/// @brief Treats n bytes as padding, skipping over them
		/// @param size_t n - Number of bytes as padding
		void  pad(const size_t&);

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
		ByteArray readRaw(const size_t&, size_t);
		/// @brief Read n bytes from current position
		/// @param size_t n - Number for bytes to read
		/// @return ByteArray - Range of bytes requested
		ByteArray readRaw(size_t);

		std::shared_ptr<ByteArray> get(size_t offset, size_t size);

		/// @brief Read some data from the stream. Creates a new instance of type
		/// @tparam Return type 'T' of the data
		/// @return type - A new instance of T read from the stream and adjusted for endianness
		template <class T> T read()
		{
			T ret{};
			this->readInto(ret);
			return ret;
		}

		/// @brief Read some data from the stream and place it into an object.
		/// @tparam Type of data to be read from stream
		/// @return type - A new instance of T read from the stream and adjusted for endianness
		template <class T> void readInto(T& data)
		{
			// Read data from the stream, swap the endianness if needed.
			file.read(reinterpret_cast<char*>(&data), sizeof(data));
			if (SysIO::systemEndianness != fileEndianness)
				SysIO::EndianSwap(data);
		}

		/// @brief Read some data from the stream, without updating stream position. Creates a new instance of type.
		/// @tparam return type 'T' of the data
		/// @return type - A new instance of 'type' read from the stream and adjusted for endianness
		template <class T> T peek()
		{
			// Read data of a certain type, seek the stream back, and return the data
			T ret{ this->read<T>() };
			seek(tell() - sizeof(T));

			return ret;
		}

		/// @brief Read some data from the stream into an existing object.
		/// @tparam type - Template type
		/// @param data - Destination for the data read in from stream
		/// @return EndianReader& - Returns it's self for chaining of >> operators
		template <class T> EndianReader& operator>>(T& data)
		{
			this->readInto(data);
			return *this;
		}
	};
}
#endif
