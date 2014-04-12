// BitMapObject.cpp
#include "bitmapobject.h"

BitMapObject::BitMapObject()
{
	hdcMemory = NULL;
	hbmNewBitMap = NULL;
	hbmOldBitMap = NULL;
	iWidth = 0;
	iHeight = 0;
}

BitMapObject::~BitMapObject()
{
	// If the hdcMemory hasn't been destroyed then do so
	if(hdcMemory)
		Destroy();
}

void BitMapObject::Load(HDC hdcCompatible, LPCTSTR lpszFilename)
{
	// If hdcMemory isn't null then make it null
	if(hdcMemory)
		Destroy();

	// Create memory dc
	hdcMemory = CreateCompatibleDC(hdcCompatible);
	// Load the bitmap
	hbmNewBitMap = (HBITMAP)LoadImage(NULL, lpszFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	// Shove the image into the dc
	hbmOldBitMap = (HBITMAP)SelectObject(hdcMemory, hbmNewBitMap);
	// Grab the bitmap's properties
	BITMAP bmp;
	GetObject(hbmNewBitMap, sizeof(BITMAP), (LPVOID)&bmp);
	// Grab the width and height
	iWidth = bmp.bmWidth;
	iHeight = bmp.bmHeight;
}

void BitMapObject::Create(HDC hdcCompatible, int width, int height)
{
	// If hdcMemory isn't null than make it null
	if(hdcMemory)
		Destroy();

	// Create memory dc
	hdcMemory = CreateCompatibleDC(hdcCompatible);
	// Create the bitmap
	hbmNewBitMap = CreateCompatibleBitmap(hdcCompatible, width, height);
	// Shove the image into the dc
	hbmOldBitMap = (HBITMAP)SelectObject(hdcMemory, hbmNewBitMap);
	// Change the width and height
	iWidth = width;
	iHeight = height;
}

void BitMapObject::Destroy()
{
	// Restore old bitmap
	SelectObject(hdcMemory, hbmOldBitMap);
	// Delete new bitmap
	DeleteObject(hbmNewBitMap);
	// Delete device context
	DeleteDC(hdcMemory);
	// Set members to 0/NULL
	hdcMemory = NULL;
	hbmNewBitMap = NULL;
	hbmOldBitMap = NULL;
	iWidth = 0;
	iHeight = 0;
}

BitMapObject::operator HDC()
{
	// Return the hdcMemory
	return(hdcMemory);
}

int BitMapObject::GetWidth()
{
	// Return the width
	return(iWidth);
}

int BitMapObject::GetHeight()
{
	// Return the height
	return(iHeight);
}