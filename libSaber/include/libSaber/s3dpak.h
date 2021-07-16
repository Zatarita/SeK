#ifndef S3DPAK
#define S3DPAK
#include "saber_file.h"
#include "s3dpak_entry.h"

class S3dpak : public SaberFile<CEADecObj, uint32_t, s3dpakEntry, s3dpakEntry::Format>
{
    static inline std::string extensions[]{ // Empty entries already have a file extension
        ".scenedata",           "",
        ".cacheblock",          "",
        ".shadercache",
        ".texturesinfo",
        ".texture",
        ".texturesmips64",      "",
        ".sound",
        ".wavebanks_mem",
        ".wavebanks_strm_file",
        ".tpl",
        ".voicespline",
        ".string",              "", "", "", "",
        ".texturesdistancefile",
        ".checkpointtexfile",
        ".loadingscreengfx",    "", "", "", "", "", "", "", "",
        ".animstream",
        ".animbank",
    };

public:
    S3dpak(const std::string& path = "")
    {
        if (!path.empty())
            this->loadArchive(path);
    };

    std::string_view getFileExtension(const s3dpakEntry::Format& format) override
    {
        return extensions[ static_cast<int>(format) ];
    }

    void saveArchive(std::string path) override
    {
        SysIO::EndianWriter stream{ LEndianWriter(path) };

        stream << static_cast<uint32_t>( fileEntries.size() );
        this->calculateOffsets( this->calculateHeaderSize() );
        this->writeEntryHeaders(stream);
        this->writeData(stream);
    }

    const ByteArray& operator[](std::string name)
    {
        return getFile(name);
    }
};

#endif // !S3DPAK