/*
    This file is a part of SeK: https://github.com/Zatarita/SeK
    last edit: Zatarita - 06/14/2021
*/

#include "include/EndianStream/sys_io.h"

namespace SysIO
{
	ByteOrder getSystemEndianness()
	{
		// Create a temp short equal to one. Then create a char array in the same memory address
		short one = 0x0001;
		unsigned char* rawData = new(&one) unsigned char[2];
		// If the first byte of the short in raw memory is 1 our system is little endian; else, big
		return rawData[0] ? ByteOrder::Little : ByteOrder::Big;

		// NOTE * This function may throw compiler warning stating we're using
		// unitialized memory. This is false. The memory was initialized when we created "one"
	}
}
