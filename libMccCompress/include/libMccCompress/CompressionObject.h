#ifndef COMPRESSIONOBJECT
#define COMPRESSIONOBJECT

#include "EStream.h"
#include "zlib.h"
#include "shared.h"

namespace Compression
{
	enum : uint8_t
	{
		NORMAL = 0x0,
		MAX_COMPRESSION = 0x1,
		MINIMAL_HEADER = 0x2,
		UNCOMPRESSED = 0x4,

		MINIMAL_FILESIZE = MINIMAL_HEADER | MAX_COMPRESSION
	};

	template <class offsetType, ChunkType cType>
	class CompressionObject : public SysIO::StreamOutputObject, public Compression::CompressionTypeObject
	{
		static const uint16_t H2AM_BYTE_ALLIGN{ 0x80 };
		static const uint16_t H2AM_HEADER_SIZE{ 0x1000 };
		static const uint16_t H2AM_CHUNK_BLOCK_SIZE{ 0x2000 };
		static const uint32_t H1A_HEADER_SIZE{ 0x40000 };
		static const uint32_t H2A_HEADER_SIZE{ 0x600000 };

		ChunkType			   type;
		uint8_t				   flags{};

		ByteArray			   header;
		std::vector<ByteArray> chunks;

		ByteArray compressChunk(ByteView chunk)
		{
			ByteArray ret( static_cast<size_t>(type));
			uLong     compLen{ static_cast<uLong>(type) };

			if (flags & MAX_COMPRESSION)
				compress2(reinterpret_cast<Bytef*>(ret.data()), &compLen,
					      reinterpret_cast<const Bytef*>(chunk.data()), chunk.size(), 9);
			else
				compress(reinterpret_cast<Bytef*>(ret.data()), &compLen,
					     reinterpret_cast<const Bytef*>(chunk.data()), chunk.size());

			if (type == ChunkType::H2AM) // reduce array to fit
				ret.resize( nextH2AMBoundary(compLen) );
			else
				ret.resize(compLen);

			return ret;
		}

		size_t nextH2AMBoundary(const size_t& currentOffset)
		{
			return H2AM_BYTE_ALLIGN * (currentOffset / H2AM_BYTE_ALLIGN + (currentOffset % H2AM_BYTE_ALLIGN > 0));
		}

		void primeHeader(const size_t& chunkCount)
		{
			switch (type)
			{
			case ChunkType::H1A:
				SysIO::ByteWriter::endianPlace({ header }, 0, static_cast<uint32_t>(chunkCount));
				break;
			case ChunkType::H2A:
				SysIO::ByteWriter::endianPlace({ header }, 0, static_cast<uint32_t>(chunkCount));
				SysIO::ByteWriter::endianPlace({ header }, 4, static_cast<uint32_t>(flags & UNCOMPRESSED));
				break;
			case ChunkType::H2AM:
				break;
			}
		}

		void addOffset(const size_t& index, const offsetType& offset)
		{
			switch (type)
			{
			case ChunkType::H2A:
			case ChunkType::H1A:
				SysIO::ByteWriter::endianPlace({ header }, sizeof(offsetType) + ( index * sizeof(offsetType) ), offset);
				break;
			case ChunkType::H2AM:
				SysIO::ByteWriter::endianPlace({ header }, index * (sizeof(offsetType) * 2) + sizeof(offsetType) + H2AM_HEADER_SIZE, offset);
				break;
			}
		}

		void addChunkSize(const size_t& index, const size_t& size)
		{
			if(type == ChunkType::H2AM)
				SysIO::ByteWriter::endianPlace({ header }, index * (sizeof(offsetType) * 2) + H2AM_HEADER_SIZE, size);
		}

		void processChunks(SysIO::EndianReader& stream, std::string_view path, const size_t& chunkCount)
		{
			auto fileOut { LEndianWriter(path) };
			size_t offset{ header.size() };

			fileOut.seek(offset);
			primeHeader(chunkCount);

			for (size_t i = 0; i < chunkCount; ++i )
			{
				ByteArray rawChunk		 { stream.readRaw(static_cast<uint32_t>(type)) };
				ByteArray compressedChunk{ compressChunk({ rawChunk }) };

				addChunkSize(i, compressedChunk.size());
				addOffset(i, offset);

				if (type == ChunkType::H1A)
				{
					fileOut.write(static_cast<uint32_t>(rawChunk.size()));
					offset += sizeof(uint32_t);
				}
				fileOut.writeRaw( compressedChunk );


				offset += compressedChunk.size();
			}

			fileOut.seek(0);
			fileOut.writeRaw(header);
		}

		void resizeHeader(SysIO::EndianReader& stream, const size_t& chunkCount)
		{
			size_t offsetBlockSize = (chunkCount * sizeof(offsetType));
			size_t headerSize{};

			switch (type)
			{
			case ChunkType::H1A:
				if (!(flags & MINIMAL_HEADER))			   // if not minimizing header use header default size
					headerSize = H1A_HEADER_SIZE;
				else									   // If minimizing header size start chunks right after header
					headerSize = offsetBlockSize + sizeof(chunkCount);
				break;
			case ChunkType::H2A:
				if (!(flags & MINIMAL_HEADER))			   // if not minimizing header use header default size
					headerSize = H2A_HEADER_SIZE;
				else									   // If minimizing header size start chunks right after header
					headerSize = offsetBlockSize + sizeof(uint32_t) + sizeof(uint32_t);
				break;
			case ChunkType::H2AM:
				header = stream.readRaw(H2AM_HEADER_SIZE); // read and store blam header
				if (!(flags & MINIMAL_HEADER))			   // if not minimizing header use header default size
					headerSize = H2AM_CHUNK_BLOCK_SIZE + H2AM_HEADER_SIZE;
				else									   // If minimizing header size start chunks right after header
					headerSize = (offsetBlockSize * 2) + H2AM_HEADER_SIZE;
			}

			header.resize(headerSize);					   // Allocate the memory for the header block
		}

		size_t getChunkCount(const size_t & fileSize)
		{
			size_t chunkSize = static_cast<size_t>(type);
			return static_cast<size_t>(fileSize / chunkSize + (fileSize % chunkSize > 0));
		}
	public:
		CompressionObject() :
			type(cType),
			header(H2AM_HEADER_SIZE)
		{
			setFlag(MINIMAL_HEADER);
		}

		void setFlag(const uint32_t & flag)
		{
			flags |= flag;
		}

		void clearFlag(const uint32_t & flag)
		{
			flags -= flag;
		}

		void compressFile(std::string_view srcPath, std::string_view dstPath)
		{
			auto fileIn  = LEndianReader(srcPath);
			ByteArray compressedChunk;

			const size_t fileSize  { fileIn.getFileSize() };
			const size_t chunkCount{ getChunkCount(fileSize) };
			resizeHeader( fileIn, chunkCount );
			processChunks(fileIn, dstPath, chunkCount);
		}
	};
}

#endif // COMPRESSIONOBJECT