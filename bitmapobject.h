//BitMapObject.h

#ifndef BITMAPOBJECT_H
#define BITMAPOBJECT_H
#pragma once

// Need this for windows stuff

#include <Windows.h>

class BitMapObject
{
private:
	// Memory dc
	HDC hdcMemory;
	// New Bitmap!
	HBITMAP hbmNewBitMap;
	// Old Bitmap
	HBITMAP hbmOldBitMap;
	// Width and height as integers
	int iWidth;
	int iHeight;

public:
	// Constructor
	BitMapObject();

	// Destructor
	~BitMapObject();

	// Loads bitmap from a file
	void Load(HDC hdcCompatible, LPCTSTR lpszFilename);

	// Creates a blank bitmap
	void Create(HDC hdcCompatible, int width, int height);

	// Destroys bitmap and dc
	void Destroy();

	// Return width
	int GetWidth();

	// Return Height
	int GetHeight();

	// Converts to HDC
	operator HDC();
};

#endif