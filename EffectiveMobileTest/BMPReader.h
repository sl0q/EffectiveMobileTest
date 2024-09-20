#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <Windows.h>

class BMPReader
{
private:
	HBITMAP hBitmap{NULL};
	BITMAP qBitmap{NULL};
	std::unique_ptr<BYTE[]> pBits;
	int bytesPerPixel{ 0 };

public:
	BMPReader();
	~BMPReader();
	bool OpenBMP(const std::string& fileName);
	void DisplayBMP();
	void CloseBMP();
};