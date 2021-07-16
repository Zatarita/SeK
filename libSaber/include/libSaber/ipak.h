#ifndef IPAK
#define IPAK

#include <string_view>

#include "ipak_entry.h"
#include "imeta.h"

class Ipak : public Imeta
{
	static inline const uint32_t IPAK_FOOTER_PAD{ 0x200000 };
	static inline const std::string FILE_EXTENSION{ ".ipak_entry" };

	std::map< std::string, IpakEntry > TextureCache;

public:
	Ipak(const std::string& path) : Imeta(path)
	{}

	void loadEntry(const std::string& name)
	{
		if (! fileEntries.count(name) ) return;

		ByteArray rawData = fileEntries[name].getData(*decompressionObject);
		TextureCache[ name ] = IpakEntry(rawData);
	}

	void loadAll()
	{
		for (const auto& entry : fileEntries)
			loadEntry( entry.first );
	}

	void saveArchive(std::string path) override
	{
		SysIO::EndianWriter stream{ LEndianWriter(path + "_tmp") };

		stream << static_cast<uint64_t>(fileEntries.size());
		this->calculateOffsets(HEADER_SIZE);
		this->writeEntryHeaders(stream);
		this->padFile(stream, HEADER_SIZE - stream.tell());
		this->writeData(stream);
		this->padFile(stream, IPAK_FOOTER_PAD);
		stream.close();
		//this->compress<CEACompObj>(path + "_tmp", path);
	}

	std::string_view getFileExtension(const ImetaEntry::Format& format) override
	{
		return FILE_EXTENSION;
	}

	std::shared_ptr<IpakEntry> operator[](std::string_view index)
	{
		std::string name(index);					// It's easier to just create a new string and resize

		if (! this->hasItem(name) ) return nullptr; // If the name doesn't exist we cant return it
		loadEntry(name);							// If it's not in the cache, load the data

		return std::make_shared<IpakEntry>( TextureCache[name] );
	}
};

#endif		
