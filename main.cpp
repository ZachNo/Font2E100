#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "write_e100_image.h"

int main(int argc, char *argv[])
{
	//Font file path and size from arguments
	char *fontFile	= argv[1];
	int fontSize = atoi(argv[2]);

	std::cout << fontFile << " " << fontSize << std::endl;

	//FreeType required variables
	FT_Library	fontLib;
	FT_Face		fontFace;

	//Initialize FreeType library
	FT_Error error = FT_Init_FreeType(&fontLib);
	if (error)
	{
		std::cout << "An error occured initializing the FreeType font library!" << std::endl;
		return 1;
	}

	//Load font into FreeType thingy
	error = FT_New_Face(fontLib, fontFile, 0, &fontFace);
	if (error == FT_Err_Unknown_File_Format)
	{
		std::cout << "Font appears to be of an unknown format!" << std::endl;
		return 1;
	}
	else if (error)
	{
		std::cout << "Other error opening font!" << std::endl;
		return 1;
	}
	std::cout << "Font loaded successfully!" << std::endl;

	//Set font size
	error = FT_Set_Char_Size(fontFace, 0, fontSize * 64, 96, 96);
	if (error)
	{
		std::cout << "Font error when setting font size!" << std::endl;
		return 1;
	}
	std::cout << "Font size successfully set!" << std::endl;

	//Start stepping through all chars and writing
	std::cout << "Parsing alpha-numeric characters to images..." << std::endl;

	bool renderedAll = 0;
	char curChar = '0';
	unsigned char block = 219;
	char none = 32;
	FT_GlyphSlot  slot = fontFace->glyph;

	//Extract font name
	std::string sFilePath(fontFile);
	unsigned int fileNameLoc = sFilePath.find_last_of('\\') + 1;
	if (fileNameLoc == 1)
		fileNameLoc = sFilePath.find_last_of('/') + 1;
	unsigned int typeLoc = sFilePath.find_last_of('.');
	std::string tag = sFilePath.substr(fileNameLoc, (sFilePath.size() - fileNameLoc) - (sFilePath.size() - typeLoc));
	tag += argv[2];

	std::cout << tag << std::endl;
	bool *character = NULL;
	while (!renderedAll)
	{
		FT_UInt glyph_index = FT_Get_Char_Index(fontFace, curChar);
		error = FT_Load_Glyph(fontFace, glyph_index, FT_LOAD_DEFAULT); //Load character in monochrome
		if (error)
			continue;

		error = FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_MONO);
		if (error)
			continue;

		int charSize = slot->bitmap.width*slot->bitmap.rows;
		if (charSize == 0)
		{
			std::cout << "Font area is zero!";
			return 1;
		}
		character = new bool[charSize];

		//Character bitmap stats
		/*std::cout << "bitmap_left: " << slot->bitmap_left
			<< "\nbitmap_top: " << slot->bitmap_top
			<< "\nrows: " << slot->bitmap.rows
			<< "\nwidth: " << slot->bitmap.width
			<< "\npitch: " << slot->bitmap.pitch << std::endl;*/
		
		//Display bit array
		/*for (int i = 0; i < (slot->bitmap.rows * slot->bitmap.pitch); i++)
		{
			if (i % slot->bitmap.pitch == 0)
				std::cout << std::endl;
			std::bitset<8> row(slot->bitmap.buffer[i]);
			std::cout << row << " ";
		}
		std::cout << std::endl;*/

		//Draw character to console
		unsigned int x = 0;
		int bufferIndex = 0;
		int j = 0;
		int index = 0;
		for (unsigned int y = 0; y < slot->bitmap.rows;)
		{
			unsigned char anded = 0;
			switch (j)
			{
			case 7: anded = 1; break;
			case 6: anded = 2; break;
			case 5: anded = 4; break;
			case 4: anded = 8; break;
			case 3: anded = 16; break;
			case 2: anded = 32; break;
			case 1: anded = 64; break;
			case 0: anded = 128; break;
			}

			bool pixel = slot->bitmap.buffer[bufferIndex] & anded;
			character[index] = pixel;

			/*if (pixel)
				std::cout << block;
			else
				std::cout << none;*/

			j++;
			x++;
			index++;
			if (j >= 8)
			{
				j = 0;
				bufferIndex++;
			}
			if (x >= slot->bitmap.width)
			{
				x = 0;
				y++;
				bufferIndex = slot->bitmap.pitch*y;
				j = 0;
				//std::cout << std::endl;
			}
		}

		//std::cout << index << std::endl;

		//Write file using my new sparkly function
		std::string charTag;
		if (curChar == ' ')
			charTag = tag + "_SP";
		else if (curChar >= 'a')
			charTag = tag + "_l" + curChar;
		else
			charTag = tag + "_" + curChar;
		std::string fileName = ".\\font\\" + charTag + ".e";


		bool writeError = writeImageAsE100(character, slot->bitmap.width, slot->bitmap.rows, fileName.c_str(), charTag.c_str());
		if (writeError)
		{
			std::cout << "Write failed, terminating!" << std::endl;
			return 1;
		}
		
		delete[] character;

		if (curChar == '9')
			curChar = 'A';
		else if (curChar == 'Z')
			curChar = 'a';
		else if (curChar == 'z')
			renderedAll = 1;
		else
			curChar++;
	}

	//Write file #including all the font files
	std::ofstream indexFile;
	std::string indexFileName = ".\\font\\" + tag + ".e";
	indexFile.open(indexFileName);
	if (!indexFile.good())
	{
		std::cout << "Writing to index file failed!" << std::endl;
		return 1;
	}

	for (char i = '0'; i <= 'z';)
	{
		std::string filename;
		if (i >= 'a')
			filename = tag + "_l" + i + ".e";
		else
			filename = tag + "_" + i + ".e";
		indexFile << "#include \"" << filename << "\"" << std::endl;

		if (i == '9')
			i = 'A';
		else if (i == 'Z')
			i = 'a';
		else
			i++;
	}

	indexFile.close();

	std::cout << "Font exported sucessfully" << std::endl;

	system("pause");
	return 0;
}