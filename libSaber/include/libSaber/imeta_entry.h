#ifndef IMETAENTRY
#define IMETAENTRY

#include <map>

#include "saber_generic_entry.h"
#include "common.h"

class ImetaEntry : public SaberGenericEntry<uint32_t>
{
public:
	/// Imeta has unique format types.
	enum class Format : uint32_t
	{
		A8L8 = 0x30,
		OXT1 = 0x46,
		AXT1 = 0x46,
		DXT3 = 0x49,
		DXT5 = 0x4c,
		DXT5A = 0x4f,
		DXN = 0x52,
		A8R8G8B8 = 0x5a,
		X8R8G8B8 = 0x5a
	};

private:
	/// Imeta entries are fixed size.
	static inline const uint32_t ENTRY_SIZE		    { 0x148 };
	/// Size of imeta entries only account for pixel data. This is used to include the metadata in the size.
	static inline const uint32_t META_DATA_SIZE		{ 0x40 };

	BitmapDimensions dimensions{};
	uint32_t		 mipmapCount{};
	Format			 format;

	/// FaceCount must be 1 or 6. There are no other options
	enum class FaceCount : uint32_t
	{
		STANDARD = 1,
		CUBEMAP  = 6
	} faceCount{};


protected:
	/// Imeta entries' names are fixed length
	static inline const uint32_t NAME_LEN{ 0x100 };

public:
	ImetaEntry();

	/**
	 * \brief
	 * Imeta doesn't have data to assign.
	 * Over riding the function to an empty function allows ipak to re-redefine it
	 */
	virtual void setData(const ByteArray& newData) override {};

	/** @brief
	 *  Writes header information about the entry.
	 *  @param stream : Stream to write the data to
	 */
	void writeHeader(SysIO::EndianWriter& stream) override;

	/** @brief
	 *  Reads header information using a stream created from the header chunk of a file.
	 *  @param stream : Stream to read the data from
	 */
	void readHeader(SysIO::ByteReader& stream) override;

	/** @brief
	 *  Calcualtes (if needed) and returns the size of the entry. Used for offset calculations
	 *  @return uint32_t : entry size
	 */
	uint32_t getHeaderSize() override;
};

#endif