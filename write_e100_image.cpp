#include <iostream>
#include <fstream>
using namespace std;

#include "write_e100_image.h"

//Char array of image data, image width, image height
//0=monochromic, 1=color
//Filename
bool writeImageAsE100(unsigned char image[], int width, int height, bool colorOrMono, char file[], char tag[])
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
		outFile << "\t.data\t" << image[i] << endl;

	outFile.close();
	//cout << "Image written successfully!" << endl;
	return 0;
}