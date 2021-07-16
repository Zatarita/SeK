#ifndef IMETA
#define IMETA

#include <map>
#include <memory>

#include "saber_file.h"
#include "imeta_entry.h"

class Imeta : public SaberFile<CEADecObj, uint64_t, ImetaEntry, ImetaEntry::Format>
{
	static_assert(std::is_base_of<SysIO::StreamTypeObject, CEADecObj>(), "DecObj_t must be a supported stream type.");

	static inline const std::string FILE_EXTENSION{ ".imeta_entry" };

	/// Used to map the ipak format to the imeta format. Called when ipak hands IpakEntry to Imeta to generate ImetaEntry for it.
	static inline const std::map<TextureFormat, ImetaEntry::Format> imetaConversionTable = {
		{ TextureFormat::A8L8,     ImetaEntry::Format::A8L8 },
		{ TextureFormat::OXT1,     ImetaEntry::Format::OXT1 },
		{ TextureFormat::AXT1,     ImetaEntry::Format::AXT1 },
		{ TextureFormat::DXT3,     ImetaEntry::Format::DXT3 },
		{ TextureFormat::DXT5,     ImetaEntry::Format::DXT5 },
		{ TextureFormat::DXT5A,    ImetaEntry::Format::DXT5A },
		{ TextureFormat::DXN,      ImetaEntry::Format::DXN },
		{ TextureFormat::A8R8G8B8, ImetaEntry::Format::A8R8G8B8 },
		{ TextureFormat::X8R8G8B8, ImetaEntry::Format::X8R8G8B8 }
	};
protected:
	static inline const uint32_t HEADER_SIZE { 0x290008 };

public:
	Imeta(std::string_view path = "");

	virtual std::string_view getFileExtension(const ImetaEntry::Format& format) override;
	virtual void saveArchive(std::string path) override;

	std::unique_ptr<ImetaEntry> operator[](std::string name);
};

#endif // !IMETA