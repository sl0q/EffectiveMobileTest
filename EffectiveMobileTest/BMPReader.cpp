#include "BMPReader.h"

BMPReader::BMPReader()
{
}

BMPReader::~BMPReader()
{
}

bool BMPReader::OpenBMP(const std::string& fileName)
{
	std::wstring winstrFileName(fileName.begin(), fileName.end());
	
	hBitmap = (HBITMAP)LoadImage(NULL, (LPCWSTR)winstrFileName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hBitmap == NULL) {
		MessageBox(NULL, L"OpenBMP::LoadImage Failed.", L"Error", MB_ICONERROR);
		return false;
	}

	if (!GetObject(hBitmap, sizeof(BITMAP), &qBitmap)) {
		MessageBox(NULL, L"OpenBMP::GetObject Failed.", L"Error", MB_ICONERROR);
		return false;
	}

	HDC hdc = CreateCompatibleDC(NULL);
	SelectObject(hdc, hBitmap);

	// Set up the BITMAPINFO structure
	BITMAPINFO bmpInfo;
	SecureZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = qBitmap.bmWidth;
	bmpInfo.bmiHeader.biHeight = qBitmap.bmHeight;
	bmpInfo.bmiHeader.biPlanes = qBitmap.bmPlanes;
	bmpInfo.bmiHeader.biBitCount = qBitmap.bmBitsPixel;
	bmpInfo.bmiHeader.biCompression = BI_RGB;


	switch (bmpInfo.bmiHeader.biBitCount)
	{
	case 24:
		bytesPerPixel = 3;
		break;
	case 32:
		bytesPerPixel = 4;
		break;
	default:
		MessageBox(NULL, L"Only 24-bit and 32-bit BMP files are supported.", L"Error", MB_ICONERROR);
		return false;
	}

	int size = qBitmap.bmWidth * qBitmap.bmHeight * bytesPerPixel;
	pBits.reset(new BYTE[size]);


	if (GetDIBits(hdc, hBitmap, 0, qBitmap.bmHeight, pBits.get(), &bmpInfo, DIB_RGB_COLORS) == NULL)
	{
		MessageBox(NULL, L"OpenBMP::GetDIBits Failed", L"Error", MB_ICONERROR);
		return false;
	}

	DeleteDC(hdc);

	return true;
}

void BMPReader::DisplayBMP()
{
	HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	LONG bkBlack = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,	// set background to black and text to white
		bkWhite = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;		// set background to white; text will be black

	for (int y = qBitmap.bmHeight - 1; y >= 0; --y)
	{
		for (int x = 0; x < qBitmap.bmWidth; ++x)
		{
			auto index = qBitmap.bmWidth * y * bytesPerPixel + x * bytesPerPixel;

			BYTE blue = pBits[index];
			BYTE green = pBits[index + 1];
			BYTE red = pBits[index + 2];
			LONG color = (red<<16) | (green<<8) | (blue);
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
	if (hBitmap != NULL)
	{
		DeleteObject(hBitmap);
		hBitmap = NULL;
	}
	SecureZeroMemory(&qBitmap, sizeof(BITMAP));
	pBits.reset();
	bytesPerPixel = 0;
}
