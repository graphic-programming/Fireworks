/*
   Class Name:

      CTgaFile

   Description:

      load tga file
*/

#include "stdafx.h"
#include "tgafile.h"

// constructor
CTgaFile::CTgaFile()
{
	width		 = 0;
	height		 = 0;
	byteperpixel = 0;
	data		 = NULL;
}

// destructor
CTgaFile::~CTgaFile()
{
	if (data != NULL) delete[] data;
}

// Truevision TGA
// https://en.wikipedia.org/wiki/Truevision_TGA
bool CTgaFile::Load(wchar_t* szFile)
{
	FILE *pFile = 0;
	errno_t err;
	unsigned char header[12];
	unsigned char uncompressheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	// open file
	if( (err = _wfopen_s( &pFile, szFile, L"rb" )) != 0 ) return false;

	// read header
	fread(header, sizeof(header), 1, pFile);

	// we only want to read uncompressed data
	if (memcmp(uncompressheader, header, sizeof(uncompressheader)) != 0)
	{
		fclose(pFile);
		return false;
	}

	//
	short w, h;
	char bitcount, dontcare;
	fread(&w, sizeof(short), 1, pFile);       // Image width (2 bytes)
	fread(&h, sizeof(short), 1, pFile);       // Image height (2 bytes)
	fread(&bitcount, sizeof(char), 1, pFile); // Pixel depth (1 byte)
	fread(&dontcare, sizeof(char), 1, pFile); // Image descriptor (1 byte)

	width        = w;
	height       = h;
	byteperpixel = bitcount / 8;

	// read image data
	int size = byteperpixel * width * height;
	if(data != NULL) delete[] data;
	data = new char[size];
	if(data == NULL)
	{
		fclose(pFile);
		return false;
	}

	fread(data, sizeof(char), size, pFile);

	// close file
	fclose(pFile);

   return true;
}