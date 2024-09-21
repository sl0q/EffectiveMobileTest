#include <iostream>

#include "BMPReader.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage:\n"
				  << "\tdrawBmp.exe [bmp image path]" << std::endl;
		return 1;
	}

	std::string filename(argv[1]);
    BMPReader bmpReader;

	try {
		bmpReader.OpenBMP(filename);
	}
	catch (const std::runtime_error &err )
	{
		std::cout << "Failed to open file\n\t"
			<< "ERROR:\t" << err.what() << std::endl;
		bmpReader.CloseBMP();
		return 1;
	}
	catch (const std::exception& err)
	{
		std::cout << "Unknown error\n\t"
				<< "ERROR:\t" << err.what() << std::endl;
		bmpReader.CloseBMP();
		return 1;
	}

	bmpReader.DisplayBMP();
	bmpReader.CloseBMP();

	return 0;
}