/*
   Class Name:

      CTgaFile

   Description:

      load tga file
*/

#pragma once

class CTgaFile
{
public:
	int width, height;
	int byteperpixel;
	char* data;

public:
	CTgaFile();
	~CTgaFile();

	bool Load(wchar_t* szFile);
};

