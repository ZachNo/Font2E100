#include <iostream>
#include <fstream>
using namespace std;

#include "write_e100_image.h"

//monochromic
bool writeImageAsE100(const bool *image, int width, int height, const char file[], const char tag[], const char adTag[])
{
	ofstream outFile;
	outFile.open(file);
	if (!outFile.good())
		return true;

	//Write address of label, width and height
	outFile << adTag << "\t.data\t" << tag << endl;
	outFile << tag << "\t.data\t" << width << endl;
	outFile << "\t.data\t" << height << endl;

	//Loop through all data, assemble words
	int count = width * height;
	int j = 0;
	for (int i = 0; i < count; i++)
	{
		long pixel = 0;
		for (j = 0; j < 32; j++)
		{
			pixel = pixel << 1;
			if (i < count)
				pixel = pixel | image[i];
			i++;
		}
		i--; //Fixed off by 1
		outFile << "\t.data\t" << pixel << endl;
	}

	outFile.close();
	return false;
}