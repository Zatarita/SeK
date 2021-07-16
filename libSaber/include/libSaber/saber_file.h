#ifndef SABERFILE
#define SABERFILE
#include <iostream>
#include <string_view>
#include <memory>
#include <map>
#include <string>
#include <type_traits>

#include "EStream.h"
#include "MccCompress.h"

template <class DecObj_t, class childCount_t, class entry_t, class format_t>
class SaberFile
{
    static_assert(std::is_integral<childCount_t>(), "SaberFile Child Count Type Must be an Integer.");
    static_assert(std::is_enum<format_t>(), "SaberFile Format Type Must be an Enum.");

protected:
    /// Arbitrary buffer size that's big enough to hold at least one entry of any type
    const uint32_t MAXIMUM_ENTRY_HEADER_SIZE{0x200};

    std::shared_ptr<DecObj_t> decompressionObject{};

    std::map< std::string, entry_t > fileEntries;

    const childCount_t getChildCount()
    {
        // If we've already loaded an archive return the existing size
        if (fileEntries.size()) return fileEntries.size();

        // Try to read the child count. If failed, assume uncompressed.
        ByteArray childCountRaw;
        childCountRaw = *decompressionObject->get(0, sizeof(childCount_t));
        return SysIO::ByteReader::endianGet<childCount_t>(childCountRaw, 0);
    }

    ByteArray getFirstChildData()
    {
        return *decompressionObject->get(sizeof(childCount_t), MAXIMUM_ENTRY_HEADER_SIZE);
    }

    const size_t getEndOfHeader()
    {
        // Load a chunk of raw data from the start of the file
        ByteArray firstEntryRaw = this->getFirstChildData();
        SysIO::ByteReader firstEntryReader(firstEntryRaw);

        // Load the first entry with that data
        entry_t firstEntry;
        firstEntry.readHeader(firstEntryReader);

        // Return the offset to the first entries data (starting at the end of the header)
        return firstEntry.getOffset();
    }

    void loadChildren(const childCount_t& numChildren, SysIO::ByteReader& headerStream)
    {
        for (childCount_t i = 0; i < numChildren; ++i)
        {
            // Load the child
            entry_t newEntry;
            newEntry.readHeader(headerStream);
            // Map it to its name
            fileEntries[ newEntry.getName() ] = newEntry;
        }
    }
    
    virtual void readHeader()
    {
        // Read the child count
        childCount_t      childCount = this->getChildCount();
        size_t            endOfHeader = this->getEndOfHeader();

        // Read the raw header data.
        ByteArray         headerRaw = *decompressionObject->get(sizeof(childCount_t), endOfHeader);
        SysIO::ByteReader headerStream(headerRaw);

        // Load the children from the raw header data.
        loadChildren(childCount, headerStream);
    }

    uint32_t calculateHeaderSize()
    {
        // Calculate how many bytes each entry takes up as a whole (plus the child count at the beginning)
        uint32_t size{sizeof(childCount_t)};
        for (auto& file : fileEntries)
            size += file.second.getHeaderSize();
        return size;
    }

    void calculateOffsets(uint32_t offset)
    {
        // Load all of the archive into memory
        this->expandArchive();

        // set the offset, and add the size of the data to get the next offset.
        for (auto& file : fileEntries)
        {
            file.second.setOffset(offset);
            offset += file.second.getSize();
        }
    }

    void writeEntryHeaders(SysIO::EndianWriter& stream)
    {
        // Write each header entry to a file. Part of saveArchive pipeline
         for (auto& file : fileEntries)
             file.second.writeHeader(stream);
    }

    void writeData(SysIO::EndianWriter& stream)
    {
        // Write the data for each entry to file. Part of the saveArchive pipeline
        for (auto& file : fileEntries)
            stream.writeRaw(file.second.getData(*decompressionObject) );
    }

    void padFile(SysIO::EndianWriter& stream, uint32_t size)
    {
        stream.writeRaw( ByteArray(size, {}) );
    }

    virtual std::string_view getFileExtension(const format_t&) = 0;
public:
    virtual void saveArchive(std::string path) = 0;

    void loadArchive(std::string_view path)
    {
        fileEntries.clear();
        decompressionObject.reset( new DecObj_t(path) );
        this->readHeader();
    }

    void expandArchive()
    {
        if (!decompressionObject) return;
        // Parse all the data in the archive. Called when offsets change
        for (auto& file : fileEntries)
            file.second.getData(*decompressionObject);
    }

    // returns the data in the s3dpak entry to reduce std::optional exposure.
    const ByteArray& getFile(std::string name)
    {
        if (!decompressionObject) 
            return EMPTY_ARRAY;

        if (!hasItem(name))
            return EMPTY_ARRAY;

        return fileEntries[name].getData(*decompressionObject);
    }

    bool extractFile(std::string path, std::string item)
    {
        if (! this->hasItem(item) ) return false;

        ByteArray data = getFile(item);
        if (data.empty()) return false;

        auto fout = LEndianWriter(path);
        fout.writeRaw(data);

        return true;
    }

    bool newFile(std::string name, const format_t& format, const ByteArray& rawData = EMPTY_ARRAY)
    {
        if (this->hasItem(name)) return false;

        fileEntries[name] = entry_t();
        this->setFileData(name, format, rawData);
        return true;
    }

    bool newFile(std::string name, const format_t& format, std::string_view path)
    {
        if (this->hasItem(name)) return false;

        fileEntries[name] = entry_t();
        this->setFileData(name, format, ByteArrayFromFile(path));
        return true;
    }

    void setFileData(std::string name, const format_t& format, const ByteArray& rawData)
    {
        if (!this->hasItem(name)) return;

        fileEntries[name].setData(rawData);
        fileEntries[name].setName(name);
        setFileFormat(name, format);
        return;
    }

    void setFileData(std::string name, const ByteArray& rawData)
    {
        if (!this->hasItem(name)) return;

        fileEntries[name].setData(rawData);
        return;
    }

    bool setFileFormat(std::string name, const format_t& format)
    {
        if (! this->hasItem(name) ) return false;

        fileEntries[name].setFormat(format);
        return true;
    }

    bool deleteFile(std::string name)
    {
        if (! this->hasItem(name) ) return false;

        fileEntries.erase(name);
        return true;
    }

    bool saveAll(const std::string& folder)
    {
        for (auto& file : fileEntries)
        {
            std::string extension{ static_cast<std::string>( getFileExtension(file.second.format) ) };
            std::string path     { folder + "/" + file.first + extension };

            std::cout << "\t" + path << std::endl;
            if (!extractFile(path, file.first) )
                return false;
        }
        return true;
    }

    std::shared_ptr<DecObj_t> getDecompressionObject()
    {
        return decompressionObject;
    }

    bool hasItem(const std::string& item)
    {
        return static_cast<bool>( fileEntries.count(item) );
    }

    std::vector<std::string> getNames()
    {
        std::vector<std::string> ret;
        for (auto entry : fileEntries)
            ret.push_back(entry.first);
        return ret;
    }

    template <class CompObj_t>
    bool compress(std::string uncomp_path, std::string comp_path)
    {
        // Validate
        if (!std::is_base_of<Compression::DecompressionTypeObject, DecObj_t>())
            return false;

        // Compress
        CompObj_t compressObj; // If you see no appropriate constructor you're using the wrong type. compression objects only
        compressObj.compressFile(uncomp_path, comp_path);

        // Cleanup
        remove(uncomp_path.c_str());

        return true;
    }
};


#endif // !SABERFILE
