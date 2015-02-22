#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
//#include <freetype.h>

#include "write_e100_image.h"

int main(int argc, char *argv[])
{
	//Font file path and size from arguments
	char *fontFile	= argv[1];
	int fontSize = atoi(argv[2]);

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
	error = FT_Set_Char_Size(fontFace, 0, fontSize * 64, 300, 300);
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
	char block = 219;
	char none = 32;
	FT_GlyphSlot  slot = fontFace->glyph;

	char tag = 

	while (!renderedAll)
	{
		FT_UInt glyph_index = FT_Get_Char_Index(fontFace, curChar);
		error = FT_Load_Glyph(fontFace, glyph_index, FT_LOAD_MONOCHROME); //Load character in monochrome
		if (error)
			continue;

		error = FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL);
		if (error)
			continue;

		//Character bitmap stats
		std::cout << "bitmap_left: " << slot->bitmap_left
			<< "\nbitmap_top: " << slot->bitmap_top
			<< "\nrows: " << slot->bitmap.rows
			<< "\nwidth: " << slot->bitmap.width<< std::endl;

		//Draw character to console
		/*for (int y = 0; y < slot->bitmap.rows; y++)
		{
			for (int x = 0; x < slot->bitmap.width; x++)
			{
				bool out = slot->bitmap.buffer[y*slot->bitmap.width + x];
				if (out)
					std::cout << block;
				else
					std::cout << none;
			}
			std::cout << std::endl;
		}*/

		//Write file using my new sparkly function
		char 
		bool writeError = writeImageAsE100(slot->bitmap.buffer, slot->bitmap.width, slot->bitmap.rows, 0, char file[], char tag[]);

		curChar++;
		if (curChar >= '0')
			renderedAll = 1;
	}

	system("pause");
	return 0;
}