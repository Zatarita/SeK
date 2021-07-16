#ifndef DECOMPRESSIONOBJECT
#define DECOMPRESSIONOBJECT

#include <string_view>
#include <iostream>

#include "EStream.h"
#include "zlib.h"
#include "shared.h"

namespace Compression
{
    /// \brief Only used for uncompressed flag in H2A on decompression object
    enum class Flag
    {
        UNCOMPRESSED                 = 0x04
    };

    /** \brief
     *  Handles loading, and decompressing data from file as needed.
     *  The decompression object is able to decompress only the chunks needed to extract a certain set of data. This
     *  functionality is available for each generation of compression.
     *
     *  We can also manually decompress chunks by utilizing:
     *    decompress(index)             Decompress a single chunk
     *    decompressRange(start, end)   Decompress a range of chunks
     *    decompressAll()               Decompress every chunk
     *
     *  The object also has two other functions for extracting data from the chunks
     *    Save(path)                    Decompress and save the entire file to disk
     *    SaveAt(path, offset, size)    Decompress the data between offset and size. Then save the data
     */
    template <class offsetType, ChunkType chunkType>
    class DecompressionObject : public SysIO::StreamInputObject, public Compression::DecompressionTypeObject
    {
        // Exceptions/Constraints
        const offsetType                    MAXIMUM_CHUNK_SIZE       {};
        const size_t                        HIGHEST_INDEXABLE_CHUNK  {};
        static inline const size_t          H2AM_MAX_OFFSETS         {0x400};
        static inline const size_t          H2AM_HEADER_SIZE         {0x1000};
        static inline constexpr const char* EXCEPTION_BOUNDS_EXCEEDED{"[!] Requested Index Exceeds The Bounds Of The Array."};
        static inline constexpr const char* EXCEPTION_BAD_FETCH      {"[!] Requested Offset Exceeds The Bounds Of The File."};
        static inline constexpr const char* EXCEPTION_CHUNK_UNKNOWN  {"[!] Unknown Chunk Type"};
        static inline constexpr const char* EXCEPTION_CHUNK_ERROR    {"[!] Unable to Load Chunk Data"};
        static inline constexpr const char* EXCEPTION_ZLIB_HEADER    {"[!] Invalid Zlib Header"};


        SysIO::EndianReader             stream{};

        const ChunkType                 type               {};
        size_t                          chunkCount         {};
        std::vector< offsetType >       chunkOffsets       {};
        std::vector< uint32_t >         chunkSizes         {};

        uint32_t                        flags              {};

        ByteArray                       header;
        std::vector< ByteArray >        decompressedChunks {};
        std::unique_ptr< std::byte[] >  decompressBuffer   { std::make_unique< std::byte[] >( MAXIMUM_CHUNK_SIZE ) };

        void readHeaderH1A()
        {
            chunkCount = static_cast<size_t>( stream.read<uint32_t>() );
        }

        void readHeaderH2A()
        {
            chunkCount = static_cast<size_t>( stream.read<uint32_t>() );
            stream >> flags;
        }

        void readHeaderH2AM()
        {
            header = stream.readRaw(H2AM_HEADER_SIZE);

            allocateMemory(H2AM_MAX_OFFSETS);
            readChunkOffsetsH2AM();
        }

        void allocateMemory(const size_t size)
        {
            if (type == ChunkType::H2AM)
            {
                chunkOffsets.reserve(size);
                chunkSizes.reserve(size);
            }
            else
            {
                chunkOffsets.reserve(static_cast<uint64_t>(size) + 1);
                resizeDecompressedChunks(size);
            }
        }

        void resizeDecompressedChunks(const size_t& size)
        {
            chunkCount = size;
            decompressedChunks.resize(size, ByteArray());
        }

        void readChunkOffsets()
        {
            for (uint32_t i = 0; i < chunkCount; ++i)
                chunkOffsets.push_back(stream.read<offsetType>());
            chunkOffsets.push_back(stream.getFileSize());
        }

        void readChunkOffsetsH2AM()
        {
            for (int i = 0; i < H2AM_MAX_OFFSETS; i++)
            {
                // If we've reached an empty chunksize, break and store the last index as the last chunk
                if (stream.peek<offsetType>() == 0)
                {
                    resizeDecompressedChunks(i);
                    break;
                }
                chunkSizes.push_back(stream.read<offsetType>());   // Read the chunk size from file
                chunkOffsets.push_back(stream.read<offsetType>()); // Read the offset from the file
            }
        }

        bool isUncompressed() const
        {
            return (flags & static_cast<uint32_t>(Flag::UNCOMPRESSED));
        }

        bool chunkNotEmpty(const size_t& index) const
        {
            if (index > chunkCount)
                return false;
            return !decompressedChunks[index].empty();
        }

        void seekToChunk(const size_t& index)
        {
            stream.seek(chunkOffsets[index]);

            // H1A has chunk count prefixed. It's unneeded so I just burn it as padding
            if (type == ChunkType::H1A)
                stream.seek(stream.tell() + sizeof(uint32_t));
        }
        
        uLong lengthCompressedData(const size_t& index) const
        {
            if (type == ChunkType::H1A)
                return (chunkOffsets[index + 1] - chunkOffsets[index]) - sizeof(uint32_t);
            else if (type == ChunkType::H2AM)
                return chunkSizes[index];
            else
                return chunkOffsets[index + 1] - chunkOffsets[index];
        }

        void readUncompressed(const size_t& index)
        {
            seekToChunk(index);

            decompressedChunks[index] = stream.readRaw(static_cast<uint32_t>(type));
        }

        void decompressRead(const size_t& index)
        {
            if (!verifyZlib(stream.peek<uint16_t>())) 
                throw std::logic_error(EXCEPTION_ZLIB_HEADER);

            // Calculate how big the decompressed chunk, and compressed chunk are.
            uLong     decompLength{ static_cast<uLong>(MAXIMUM_CHUNK_SIZE) };
            uLong     compLength = lengthCompressedData(index);

            // Read the compressed chunk from file
            auto compChunk{ stream.readRaw(compLength) };

            // Decompress the data from the file into a buffer
            uncompress(reinterpret_cast<Bytef*>(decompressBuffer.get()), &decompLength,
                reinterpret_cast<Bytef*>(compChunk.data()), compLength);

            // Allocate the memory
            decompressedChunks[index].resize(decompLength);

            // Store the decompressed chunk data
            std::memcpy(decompressedChunks[index].data(), decompressBuffer.get(), decompLength);
        }

        uint32_t compensateBlamHeader(ByteView ret, size_t& offset, size_t& size)
        {
            if (offset < header.size()) // If the offset starts in the header
            {
                // Calculate how much data we have to copy from the header, and copy it.
                uint32_t partialHeaderSizer = header.size() - offset;
                std::memcpy(ret.data(),
                    header.data() + offset,
                    partialHeaderSizer);

                size -= partialHeaderSizer;        // We already read some, so now we update the size to reflect that.
                return partialHeaderSizer;

                offset = 0;                        // Remaining data starts from first chunk
            }
            else                       // If the offset doesn't start in the header
                offset -= header.size();           // Adjust the offset to account for the uncompressed header

            return 0;
        }

        void extractData(ByteArray& ret, const size_t& offset, const size_t& size, size_t& streamPosition)
        {
            if (this->isUncompressed())
            {
                stream.seek(offset);
                ret = stream.readRaw(size);
                return;
            }

            // Calculate the offset the chunk should begin and end in
            size_t chunkStartingIndex{ offset / MAXIMUM_CHUNK_SIZE };
            size_t chunkEndIndex{ (offset + size) / MAXIMUM_CHUNK_SIZE };

            // Bounds checking
            if (chunkEndIndex > chunkCount || chunkStartingIndex > chunkCount)
                throw std::logic_error(EXCEPTION_BAD_FETCH);

            // Chunk offset relative to it's closest chunk boundary.
            size_t chunkStartingMagic{ offset - (chunkStartingIndex * MAXIMUM_CHUNK_SIZE) };
            size_t chunkEndMagic{ (offset + size) - (chunkEndIndex * MAXIMUM_CHUNK_SIZE) };

            // Decompress the chunks in the range we need
            decompressRange(chunkStartingIndex, chunkEndIndex + 1);

            // Copy the needed data from each chunk.
            // First chunk   [chunkStartingMagic -> end of chunk] : or if data exists in one chunk [chunkStartingMagic -> size]
            // Middle chunks [chunk start -> chunk end];
            // Last chunk    [chunk start -> chunkEndMagic]
            for (size_t i = chunkStartingIndex; i <= chunkEndIndex; ++i)
            {
                if (i == chunkStartingIndex) // First chunk
                {

                    if (i != chunkEndIndex) // If there is more than one chunk
                    {
                        std::memcpy(ret.data() + streamPosition,
                            decompressedChunks[i].data() + chunkStartingMagic,
                            decompressedChunks[i].size() - chunkStartingMagic);
                        // Update how much data has been written to the stream.
                        streamPosition += decompressedChunks[i].size() - chunkStartingMagic;
                    }
                    else                    // If this is the first, and only chunk
                    {
                        std::memcpy(ret.data() + streamPosition,
                            decompressedChunks[i].data() + chunkStartingMagic,
                            size);
                    }
                }
                else if (i == chunkEndIndex) // Last chunk
                    std::memcpy(ret.data() + streamPosition,
                        decompressedChunks[i].data(),
                        chunkEndMagic);
                else                        // Middle chunks
                {
                    std::memcpy(ret.data() + streamPosition,
                        decompressedChunks[i].data(),
                        decompressedChunks[i].size());
                    // Update how much data has been written to the stream.
                    streamPosition += decompressedChunks[i].size();
                }
            }
        }

    public:
        /** \brief
         *  Constructor for the decompression object.
         * \tparam offsetType - Determines how the chunk array is stored in file (h2a is 64bit, else 32bit)
         * \param path        - Location to the file on disk (Access will be held)
         * \param chunkType   - Determines the chunk size, as well as identifies the intended engine.
         */
        DecompressionObject(std::string_view path, const bool& uncompressed = false) :
            MAXIMUM_CHUNK_SIZE(static_cast<offsetType>(chunkType)),
            HIGHEST_INDEXABLE_CHUNK(std::numeric_limits<offsetType>::max() / MAXIMUM_CHUNK_SIZE),
            stream( LEndianReader(path) ),
            type(chunkType)
        {
            if (uncompressed)
            {
                setCompressed(false);
                return;
            }

            switch(chunkType)
            {
            case ChunkType::H1A:            // H1A Stores Chunk Size at the Beginning of Each Chunk.
                readHeaderH1A();
                break;
            case ChunkType::H2A:            // H2A Stores Flags Just After the Chunk Count
                readHeaderH2A();
                break;
            case ChunkType::H2AM:           // H2AM Stores (chunk size, offset) and no chunk count.
                readHeaderH2AM();
                return;
            default:             // If unknown chunk type panic
                throw std::logic_error(EXCEPTION_CHUNK_UNKNOWN);
            }
            
            allocateMemory(chunkCount);
            readChunkOffsets();
        }

        /** \brief
         * Returns a const ref to the chunk count.
         * \return const chunkCount
         */
        const uint32_t& getChunkCount() const { return chunkCount; }

        /** \brief
         * Decompress a specific chunk index
         * \param index - chunk index to decompress
         */

        void decompress(const size_t& index)
        {
            // Bounds checking, and seeing if we've already decompressed this index
            if(index > chunkCount)
                throw std::logic_error(EXCEPTION_BOUNDS_EXCEEDED);

            if( chunkNotEmpty(index) ) return;
            if (isUncompressed()) { readUncompressed(index); return; }

            seekToChunk(index);
            decompressRead(index);
        }

        /// \brief Decompress every chunk
        void decompressAll() { decompressRange(0, chunkCount); }
        /** \brief
         * Decompress a range of chunks
         * \param start - Starting index to decompress
         * \param end   - End index to decompress
         */
        void decompressRange(const size_t& start, const size_t& end) { for(size_t i = start; i < end; i++) decompress(i); }


        /** \brief
         * Get data from the file using it's uncompressed offset, and size.
         * The decompression object will determine which chunks it needs to decompress, and then return the data as a ByteArray.
         * \param offset      - Offset to the start of the decompressed data
         * \param size        - Size of the data
         * \return *ByteArray - pointer to the extracted data (as a shared_ptr)
         *
         */
        std::shared_ptr<ByteArray> get(size_t offset, size_t size)
        {
            std::shared_ptr<ByteArray> ret { std::make_shared<ByteArray>(size) };
            uint64_t streamPosition = 0;  // Holds current stream position/bytes copied

            // If it's h2a map we have to do some extra offset management to account for header
            if ( type == ChunkType::H2AM && !this->isUncompressed() )
                streamPosition += compensateBlamHeader({ ret->data(), ret->size() }, offset, size);

            try
            {
                extractData(*ret, offset, size, streamPosition);
            }
            catch (...)
            {
                this->setCompressed(false);
                try
                {
                    extractData(*ret, offset, size, streamPosition);
                }
                catch (...)
                {
                    return std::make_shared<ByteArray>();
                }
            }

            return ret;
        }

        /** \brief
         * Decompress the file and save it to disk.
         * \param path - Location to save the decompressed file
         */
        void save(std::string_view path)
        {
            // Decompress all the chunks
            decompressAll();

            // and write them to disk
            SysIO::EndianWriter fout(path, SysIO::ByteOrder::Little);

            if (type == ChunkType::H2AM)
                fout.writeRaw(header);

            for(ByteArray& chunk : decompressedChunks) fout.writeRaw(chunk);
        }

        /** \brief
         * Locate, decompress, and extract data from a file. Then save the file to disk.
         * \param path   - Location to save the decompressed data
         * \param offset - Offset to the start of the decompressed data
         * \param size   - Size of the data
         */
        void saveAt(std::string_view path, const uint32_t& offset, const uint32_t& size)
        {
            // Open a stream for writing the data
            SysIO::EndianWriter fout(SysIO::ByteOrder::Little);
            fout.open(path);

            // Get the data from the file.
            std::shared_ptr<ByteArray> hData { get(offset, size) };

            // Write the data to the stream.
            fout.writeRaw(*hData);
            fout.close();
            return;
        }

        void close()
        {
            stream.close();
        }

        bool isOpen()
        {
            return stream.isOpen();
        }

        void setCompressed(const bool& isCompressed)
        {
            flags = isCompressed ? 0 : static_cast<uint32_t>(Flag::UNCOMPRESSED);
        }
    };

    /** \brief Wrapper for decompression object
     *  \return Decompression object made for h1a
     */
    
}

#endif // DECOMPRESSIONOBJECT
