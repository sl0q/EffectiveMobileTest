#include "BMPReader.h"

BMPReader::BMPReader()
{
}

BMPReader::~BMPReader()
{
}

uint32_t BMPReader::MakeStrideAligned(uint32_t alignStride)
{
	// Add 1 to the row_stride until it is divisible with align_stride
	uint32_t newStride = rowStride;
	while (newStride % alignStride != 0)
		newStride++;

	return newStride;
}

void BMPReader::OpenBMP(const std::string& fileName)
{
	std::ifstream file{ fileName, std::ios_base::binary };
	if (!file.is_open())
		throw std::runtime_error("Unable to open the input image file.");

	file.read((char*)&bmpFileHeader, sizeof(BITMAPFILEHEADER));
	if (bmpFileHeader.bfType != 0x4d42)
		throw std::runtime_error("Unrecognized file format.");

	file.read((char*)&bmpInfoHeader, sizeof(BITMAPINFOHEADER));

	file.seekg(bmpFileHeader.bfOffBits, file.beg);

	switch (bmpInfoHeader.biBitCount)
	{
	case 24:
		bytesPerPixel = 3;
		break;
	case 32:
		bytesPerPixel = 4;
		break;
	default:
		throw std::runtime_error("Only 24-bit and 32-bit .bmp files are supported.");
	}

	if (bmpInfoHeader.biHeight < 0) {
		throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");
	}

	pixelDataSize = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * bmpInfoHeader.biBitCount / 8;
	pixelData.reset(new BYTE[pixelDataSize]);

	// Check if we need to take into account row padding
	if (bmpInfoHeader.biWidth % 4 == 0) {
		file.read((char*)pixelData.get(), pixelDataSize);
		bmpFileHeader.bfSize += pixelDataSize;

	}
	else {
		rowStride = bmpInfoHeader.biWidth * bmpInfoHeader.biBitCount / 8;
		uint32_t newStride = MakeStrideAligned(4);
		size_t paddingRowSize = newStride - rowStride;
		auto paddingRow = std::make_unique<uint8_t[]>(newStride - rowStride);

		for (int y = 0; y < bmpInfoHeader.biHeight; ++y) {
			file.read((char*)(pixelData.get() + rowStride * y), rowStride);	// read data
			file.read((char*)paddingRow.get(), paddingRowSize);	// skip padding row
		}
		bmpFileHeader.bfSize += pixelDataSize + bmpInfoHeader.biHeight * paddingRowSize;
	}
}

void BMPReader::DisplayBMP()
{
	HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	LONG bkBlack = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,	// set background to black and text to white
		bkWhite = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;		// set background to white; text will be black

	for (int y = bmpInfoHeader.biHeight - 1; y >= 0; --y)
	{
		for (int x = 0; x < bmpInfoHeader.biWidth; ++x)
		{
			auto index = bmpInfoHeader.biWidth * y * bytesPerPixel + x * bytesPerPixel;

			BYTE blue = pixelData[index];
			BYTE green = pixelData[index + 1];
			BYTE red = pixelData[index + 2];
			LONG color = (red << 16) | (green << 8) | (blue);
			// alpha channel is not processed

			if (color > 0x007f7f7f)
				SetConsoleTextAttribute(stdOut, bkWhite);
			else
				SetConsoleTextAttribute(stdOut, bkBlack);
			WriteConsole(stdOut, L"  ", 2, NULL, NULL);
		}

		WriteConsole(stdOut, "\n", 1, NULL, NULL);
	}

	SetConsoleTextAttribute(stdOut, bkBlack);
}

void BMPReader::CloseBMP()
{
	SecureZeroMemory(&bmpFileHeader, sizeof(BITMAPFILEHEADER));
	SecureZeroMemory(&bmpInfoHeader, sizeof(BITMAPINFOHEADER));
	pixelData.reset();
	pixelDataSize = 0;
	bytesPerPixel = 0;
	rowStride = 0;
}
