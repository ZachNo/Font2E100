#include <iostream>
#include <fstream>
using namespace std;

#include "write_e100_image.h"

//Char array of image data, image width, image height
//Filename, e100 label as tag
bool writeImageAsE100(const unsigned char *image, int width, int height, const char file[], const char tag[])
{
	ofstream outFile;
	outFile.open(file);
	if (!outFile.good())
	{
		//cout << "Writing to output file failed!" << endl;
		return 1;
	}

	outFile << tag << "\t.data\t" << width << endl;
	outFile << "\t.data\t" << height << endl;
	int count = width * height;
	for (int i = 0; i < count; i++)
	{
		unsigned long pixel = 0;
		for (int j = 0; j < 32; j++)
		{
			pixel = pixel << 1;
			pixel = pixel | (bool)image[i];
			i++;
		}
		outFile << "\t.data\t" << pixel << endl;
	}

	outFile.close();
	//cout << "Image written successfully!" << endl;
	return 0;
}

//monochromic
bool writeImageAsE100(const bool *image, int width, int height, const char file[], const char tag[])
{
	ofstream outFile;
	outFile.open(file);
	if (!outFile.good())
	{
		//cout << "Writing to output file failed!" << endl;
		return 1;
	}

	outFile << tag << "\t.data\t" << width << endl;
	outFile << "\t.data\t" << height << endl;
	int count = width * height;
	for (int i = 0; i < count; i++)
	{
		unsigned long pixel = 0;
		for (int j = 0; j < 32; j++)
		{
			pixel = pixel << 1;
			pixel = pixel | image[i];
			i++;
		}
		outFile << "\t.data\t" << pixel << endl;
	}

	outFile.close();
	//cout << "Image written successfully!" << endl;
	return 0;
}