# Font2E100
### Creates E100 memory arrays for a font
Renders the alphanumeric characters of a font to E100 memory array files.
### Requirements:
* FreeType2
* a *fonts* folder in the location of the executable

### Usage:
The compiled program is controlled via command-line.
The first argument is the file location of the font you want to render.
The second is the point size you want the font to be.
Example of 12pt Arial:
>Font2E100.exe C:\Windows\Fonts\arial.ttf 12

The program will then output a file for every letter and number with an index file.
Just #include the index in your E100 program to have the font data.

### Technical Stuff
The font data is stored in a consecutive array of bits. There are 32 bits stored in each word specified in the E100 array.

So to unpack the data, just loop through each word and get each bit at a time starting from the LMB and load a new word when you unpack all the bits.
There are no breaks given in between lines, the end user has the responsibility to start a new line once the width has been met.
