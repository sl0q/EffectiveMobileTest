#pragma once

#include <fstream>
#include <string>
#include <memory>
#include <Windows.h>

class BMPReader
{
private:
	BITMAPFILEHEADER bmpFileHeader{ NULL };
	BITMAPINFOHEADER bmpInfoHeader{ NULL };

	std::unique_ptr<BYTE[]> pixelData;

	size_t pixelDataSize{ 0 };
	int bytesPerPixel{ 0 };
	uint32_t rowStride{ 0 };

	uint32_t MakeStrideAligned(uint32_t alignStride);

public:
	BMPReader();
	~BMPReader();
	void OpenBMP(const std::string& fileName);
	void DisplayBMP();
	void CloseBMP();
};