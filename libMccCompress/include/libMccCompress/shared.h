#ifndef SHARED
#define SHARED

#include <cstdint>
#include <vector>
#include <algorithm>

namespace Compression
{
    class MCCCompressionObject {};
    class CompressionTypeObject : public MCCCompressionObject {};
    class DecompressionTypeObject : public MCCCompressionObject {};

    // Identifies formatting, and stores chunk size
    enum class ChunkType : uint32_t
    {
        H2A = 0x8000,
        H1A = 0x20000,
        H2AM = 0x40000
    };

    /*Represented in mixed endian.*/
    static inline std::vector<uint16_t> POSSIBLE_ZLIB_HEADERS {
        0x1D08,   0x5B08,   0x9908,   0xD708,    0x1918,   0x5718,   0x9518,   0xD318,
        0x1528,   0x5328,   0x9128,   0xCF28,    0x1138,   0xF438,   0x8D38,   0xCB38,
        0x0D48,   0x4B48,   0x8948,   0xC748,    0x0958,   0x4758,   0x8558,   0xC358,
        0x0568,   0x4368,   0x8168,   0xDE68,    0x0178,   0x5E78,   0x9C78,   0xDA78
    };

    static inline bool verifyZlib(const uint16_t& header)
    {
        return std::count(POSSIBLE_ZLIB_HEADERS.begin(), POSSIBLE_ZLIB_HEADERS.end(), header);
    }
}

#endif