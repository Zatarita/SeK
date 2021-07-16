#ifndef TEXTUREENTRY
#define TEXTUREENTRY

#include <EStream.h>
#include "../common.h"

class TextureEntry
{
	BitmapDimensions dimensions;
	uint32_t faceCount, mipmapCount;

	TextureFormat Format;

public:
	ByteArray pixelData;
	TextureEntry() = default;

	TextureEntry(ByteArray& data)
	{
		SysIO::ByteReader stream(data);

		stream.pad(0x10);
		stream >> dimensions.width >> dimensions.height >> dimensions.depth >> faceCount;
		stream.pad(0x6);
		stream >> Format;
		stream.pad(0x6);
		stream >> mipmapCount;
		stream.pad(0x6);
		pixelData = stream.readRaw(data.size() - stream.tell() - 0x6); // Last 6 bytes are footer treated as padding
		stream.pad(0x6);
	}
};


#endif //TEXTUREENTRY
