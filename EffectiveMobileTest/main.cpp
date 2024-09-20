#include <iostream>

#include "BMPReader.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage:\n"
				  << "\tdrawBmp.exe [bmp image path]" << std::endl;
	}
	std::string filename(argv[1]);

    BMPReader bmpReader;
	if (!bmpReader.OpenBMP(filename))
		return 1;

	bmpReader.DisplayBMP();
	bmpReader.CloseBMP();
}