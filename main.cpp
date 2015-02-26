#include <iostream>
#include <string>
#include <fstream>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "write_e100_image.h"

bool initFT(FT_Library &lib, FT_Face &face, const int size, const char *file);
bool renderChar(const char curChar, bool *&charArr, FT_Face &face, FT_GlyphSlot &slot);

int main(int argc, char *argv[])
{
	//Font file path and size from arguments
	char *fontFile	= argv[1];
	int fontSize = atoi(argv[2]);
	std::cout << fontFile << " " << fontSize << std::endl;

	//FreeType required variables
	FT_Library	fontLib;
	FT_Face		fontFace;
	FT_Error	error;
	if (initFT(fontLib, fontFace, fontSize, fontFile))
		return 1;

	//Start stepping through all chars and writing
	std::cout << "Parsing alpha-numeric characters to images..." << std::endl;

	//Extract font name
	std::string sFilePath(fontFile);
	unsigned int fileNameLoc = sFilePath.find_last_of('\\') + 1;
	if (fileNameLoc == 1)
		fileNameLoc = sFilePath.find_last_of('/') + 1;
	unsigned int typeLoc = sFilePath.find_last_of('.');
	std::string tag = sFilePath.substr(fileNameLoc, (sFilePath.size() - fileNameLoc) - (sFilePath.size() - typeLoc));
	tag += argv[2];

	//Loop through all alphanumerics
	bool *character = NULL;
	char curChar = '0';
	FT_GlyphSlot  slot = fontFace->glyph;
	while (curChar <= 'z')
	{
		renderChar(curChar, character, fontFace, slot);

		//Tag vars
		std::string charTag;
		std::string charAdTag;
		if (curChar >= 'a')
			charTag = tag + "_l" + curChar;
		else
			charTag = tag + "_" + curChar;
		charAdTag = charTag + "_ad";
		std::string fileName = ".\\font\\" + charTag + ".e";

		//Write file using my new sparkly function
		if (writeImageAsE100(character, slot->bitmap.width, slot->bitmap.rows, fileName.c_str(), charTag.c_str(), charAdTag.c_str()))
		{
			std::cout << "Write failed, terminating!" << std::endl;
			return 1;
		}

		delete[] character;

		if (curChar == '9')
			curChar = 'A';
		else if (curChar == 'Z')
			curChar = 'a';
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

	//Loop through all alphanumerics and write include
	char i = '0';
	while (i <= 'z')
	{
		std::string filename;
		if (i >= 'a')
			filename = tag + "_l" + i + ".e";
		else
			filename = tag + "_" + i + ".e";
		indexFile << "#include " << filename << std::endl;

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

//Initializes FreeType, loads selected font
//Returns false for no problem loading, true for problem loading
bool initFT(FT_Library &lib, FT_Face &face, const int size, const char *file)
{
	//Initialize FreeType library
	FT_Error error = FT_Init_FreeType(&lib);
	if (error)
	{
		std::cout << "An error occured initializing the FreeType font library!" << std::endl;
		return true;
	}

	//Load font into FreeType thingy
	error = FT_New_Face(lib, file, 0, &face);
	if (error == FT_Err_Unknown_File_Format)
	{
		std::cout << "Font appears to be of an unknown format!" << std::endl;
		return true;
	}
	else if (error)
	{
		std::cout << "Other error opening font!" << std::endl;
		return true;
	}
	std::cout << "Font loaded successfully!" << std::endl;

	//Set font size
	error = FT_Set_Char_Size(face, 0, size * 64, 96, 96);
	if (error)
	{
		std::cout << "Font error when setting font size!" << std::endl;
		return true;
	}
	std::cout << "Font size successfully set!" << std::endl;

	return false;
}

//Transfers FreeType render to boolean array
//Returns true for error, false for success
bool renderChar(const char curChar, bool *&charArr, FT_Face &face, FT_GlyphSlot &slot)
{
	//Load character index
	FT_UInt glyph_index = FT_Get_Char_Index(face, curChar);
	FT_Error error;
	error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT); //Load character in monochrome
	if (error)
		return true;
	error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
	if (error)
		return true;

	//Get character area, if 0 return with error
	int charSize = slot->bitmap.width*slot->bitmap.rows;
	if (charSize == 0)
	{
		std::cout << "Font area is zero!";
		return true;
	}
	//Create array for character
	charArr = new bool[charSize];

	//Vars for loop
	unsigned int x = 0;
	unsigned int y = 0;
	int bufferIndex = 0;
	int j = 7;
	int index = 0;
	
	while(y < slot->bitmap.rows)
	{
		//Get pixel by shifting, then anding with 1 to get bit at slot
		charArr[index] = (slot->bitmap.buffer[bufferIndex] >> j) & 1;
		j--;
		x++;
		index++;

		//Reset j, increment buffer index
		if (j < 0)
		{
			j = 7;
			bufferIndex++;
		}

		//Increment y, align bufferIndex
		if (x >= slot->bitmap.width)
		{
			x = 0;
			y++;
			bufferIndex = slot->bitmap.pitch*y;
			j = 7;
		}
	}
	return false;
}