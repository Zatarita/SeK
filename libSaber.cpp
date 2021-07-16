// libSaber.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "libSaber/ipak.h"
#include "libSaber/s3dpak.h"
#include "libSaber/imeta.h"


int main()
{
	S3dpak test("backup/a10.s3dpak");
	test.deleteFile("TexturesInfo");
	test.saveArchive("Extract/a10.s3dpak");

	return 0;
}   
