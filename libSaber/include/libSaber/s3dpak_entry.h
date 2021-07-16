#ifndef S3DPAKENTRY
#define S3DPAKENTRY
#include "saber_generic_entry.h"


class s3dpakEntry : public SaberGenericEntry<uint32_t>
{
    static inline const uint64_t ENTRY_PADDING{};

public:
    enum class Format : uint32_t // * = PC; x = Xbox
    {
        SceneData = 0,              // *   x
        Data = 1,                   // *   x
        CacheBlock = 2,             // *   x
        Shader = 3,                 // *   x
        ShaderCache = 4,            //     x
        TexturesInfo = 5,           // *   x
        Textures = 6,               //     x
        TexturesMips64 = 7,         //     x
        SoundData = 8,              // *   x
        Sounds = 9,                 //------
        WaveBanks_mem = 10,         // *   x
        WaveBanks_strm_file = 11,   // *   x
        Templates = 12,             // *   x
        VoiceSplines = 13,          //------
        Strings = 14,               // *   x
        Ragdolls = 15,              //------
        Scene = 16,                 // *   x
        Hkx = 17,                   // *   
        Gfx = 18,                   // *   x
        TexturesDistanceFile = 19,  //     x
        CheckPointTexFile = 20,     //------
        LoadingScreenGfx = 21,      //------
        SceneGrs = 22,              // *   x
        SceneScr = 23,              //------
        SceneAnimbin = 24,          //------
        SceneRain = 25,             // *   x
        SceneCDT = 26,              // *   x
        SceneSM = 27,               // *   x
        SceneSLO = 28,              //------
        SceneVis = 29,              // *   x
        AnimStream = 30,            //     x
        AnimBank = 31               //------
    } format;

    void writeHeader(SysIO::EndianWriter& stream) override
    {
        stream << offset << size << static_cast<uint32_t>(name.size());
        stream.writeString(name);
        stream << format << ENTRY_PADDING;
    }

    void readHeader(SysIO::ByteReader& stream) override
    {
        stream >> offset >> size;
        name = stream.getString(stream.read<uint32_t>());
        stream >> format;
        stream.seek(stream.tell() + sizeof(ENTRY_PADDING));
    }

    uint32_t getHeaderSize() override
    {
        return sizeof(offset) + sizeof(size) + sizeof(uint32_t) + name.size() + sizeof(format) + sizeof(ENTRY_PADDING);
    }

    void setFormat(const Format& newFormat)
    {
        format = newFormat;
    }
};

#endif
