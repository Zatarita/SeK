#include "libSaber/imeta.h"

Imeta::Imeta(std::string_view path)
{
	if (!path.empty())
		this->loadArchive(path);
};

std::string_view Imeta::getFileExtension(const ImetaEntry::Format& format) 
{
	return FILE_EXTENSION;
}

void Imeta::saveArchive(std::string path)
{
	SysIO::EndianWriter stream{ LEndianWriter(path) };

	stream << static_cast<uint64_t>(this->getChildCount());
	this->writeEntryHeaders(stream);
	this->padFile(stream, HEADER_SIZE - stream.tell());
}

std::unique_ptr<ImetaEntry> Imeta::operator[](std::string name)
{
	if (!this->hasItem(name)) return nullptr;

	return std::make_unique<ImetaEntry>(this->fileEntries[name]);
}