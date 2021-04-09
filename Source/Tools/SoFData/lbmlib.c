// lbmlib.c

#include "cmdlib.h"
#include "lbmlib.h"

/*
============================================================================

TARGA IMAGE

============================================================================
*/

typedef struct _TargaHeader
{
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;

int fgetLittleShort (FILE *f)
{
	byte	b1, b2;

	b1 = fgetc(f);
	b2 = fgetc(f);

	return((short)(b1 + (b2 << 8)));
}

int fgetLittleLong (FILE *f)
{
	byte	b1, b2, b3, b4;

	b1 = fgetc(f);
	b2 = fgetc(f);
	b3 = fgetc(f);
	b4 = fgetc(f);

	return(b1 + (b2 << 8) + (b3 << 16) + (b4 << 24));
}


/*
=============
LoadTGA
=============
*/
void LoadTGA(char *name, byte **pixels, int *width, int *height)
{
	int				columns, rows, numPixels;
	byte			*pixbuf;
	byte			*rowBuf;
	int				row, column;
	FILE			*fin;
	byte			*targa_rgba;
	TargaHeader		targa_header;
	unsigned char	red, green, blue, alphabyte;
	unsigned char	packetHeader, packetSize, j;
	int				flip;
	int				mirror;
	int				rowOffset;
	int				pixDirection;

	fin = fopen(name, "rb");
	if (!fin)
		Error ("Couldn't read %s", name);

	targa_header.id_length = fgetc(fin);
	targa_header.colormap_type = fgetc(fin);
	targa_header.image_type = fgetc(fin);
	
	targa_header.colormap_index = fgetLittleShort(fin);
	targa_header.colormap_length = fgetLittleShort(fin);
	targa_header.colormap_size = fgetc(fin);
	targa_header.x_origin = fgetLittleShort(fin);
	targa_header.y_origin = fgetLittleShort(fin);
	targa_header.width = fgetLittleShort(fin);
	targa_header.height = fgetLittleShort(fin);
	targa_header.pixel_size = fgetc(fin);
	targa_header.attributes = fgetc(fin);
	flip = (targa_header.attributes & 0x020) == 0;
	mirror = (targa_header.attributes & 0x010) != 0;

	if ((targa_header.image_type != 2) && (targa_header.image_type != 10)) 
		Error ("LoadTGA: Only type 2 and 10 targa RGB images supported\n");

	if (targa_header.colormap_type || ((targa_header.pixel_size != 32) && (targa_header.pixel_size != 24)))
		Error ("Texture_LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n");

	columns = targa_header.width;
	rows = targa_header.height;
	numPixels = columns * rows;

	if (width)
		*width = columns;
	if (height)
		*height = rows;

	if(!pixels)
		return;

	targa_rgba = malloc(numPixels * 4);
	*pixels = targa_rgba;

	if (flip)
	{
		pixbuf = targa_rgba + ((rows - 1) * columns * 4);
		rowOffset = -columns * 4;
	}
	else
	{
		pixbuf = targa_rgba;
		rowOffset = columns * 4;
	}
	if (mirror)
	{
		pixDirection = -4;
		pixbuf += ((columns - 1) * 4);
	}
	else
	{
		pixDirection = 4;
	}

	if (targa_header.id_length)
		fseek(fin, targa_header.id_length, SEEK_CUR);  // skip TARGA image comment
	
	if (targa_header.image_type == 2)
	{													// Uncompressed, RGB images
		for(row = 0; row < rows; row++)
		{
			rowBuf = pixbuf;
			for(column = 0; column < columns; column++)
			{
				switch (targa_header.pixel_size)
				{
					case 24:
						blue = getc(fin);
						green = getc(fin);
						red = getc(fin);
						rowBuf[0] = red;
						rowBuf[1] = green;
						rowBuf[2] = blue;
						rowBuf[3] = 255;
						rowBuf += pixDirection;
					break;
					case 32:
						blue = getc(fin);
						green = getc(fin);
						red = getc(fin);
						alphabyte = getc(fin);
						rowBuf[0] = red;
						rowBuf[1] = green;
						rowBuf[2] = blue;
						rowBuf[3] = alphabyte;
						rowBuf += pixDirection;
					break;
				}
			}
			pixbuf += rowOffset;
		}
	}
	else if(targa_header.image_type == 10)
	{												// Runlength encoded RGB images
		for(row = 0; row < rows; row++)
		{
			rowBuf = pixbuf;
			for(column = 0; column < columns; )
			{
				packetHeader = getc(fin);
				packetSize = 1 + (packetHeader & 0x7f);
				if (packetHeader & 0x80)
				{									// run-length packet
					switch (targa_header.pixel_size)
					{
						case 24:
					 		blue = getc(fin);
					 		green = getc(fin);
					 		red = getc(fin);
					 		alphabyte = 255;
					 	break;
						case 32:
					 		blue = getc(fin);
					 		green = getc(fin);
					 		red = getc(fin);
					 		alphabyte = getc(fin);
					 	break;
					}
	
					for(j = 0; j < packetSize; j++)
					{
						rowBuf[0] = red;
						rowBuf[1] = green;
						rowBuf[2] = blue;
						rowBuf[3] = alphabyte;
						rowBuf += pixDirection;
						column++;
						if(column == columns)
						{									// run spans across rows
							column = 0;
							row++;
							if (row >= rows)
								goto breakOut;
							pixbuf += rowOffset;
							rowBuf = pixbuf;
						}
					}
				}
				else
				{			                            // non run-length packet
					for(j = 0; j < packetSize; j++)
					{
						switch (targa_header.pixel_size)
						{
							case 24:
								blue = getc(fin);
								green = getc(fin);
								red = getc(fin);
								rowBuf[0] = red;
								rowBuf[1] = green;
								rowBuf[2] = blue;
								rowBuf[3] = 255;
								rowBuf += pixDirection;
							break;
							case 32:
								blue = getc(fin);
								green = getc(fin);
								red = getc(fin);
								alphabyte = getc(fin);
								rowBuf[0] = red;
								rowBuf[1] = green;
								rowBuf[2] = blue;
								rowBuf[3] = alphabyte;
								rowBuf += pixDirection;
							break;
						}
						column++;
						if (column == columns)
						{							// pixel packet run spans across rows
							column = 0;
							row++;
							if (row >= rows)
								goto breakOut;
							pixbuf += rowOffset;
							rowBuf = pixbuf;
						}						
					}
				}
			}
			breakOut:;
			pixbuf += rowOffset;
		}
	}
	fclose(fin);
}

/*
==============
LoadAnyImage

Return Value:
	false: paletted texture
	true:  true color RGBA image (no palette)
==============
*/
qboolean LoadAnyImage (char *name, unsigned int **pixels, int *width, int *height)
{
	char	ext[128];

	ExtractFileExtension (name, ext);

	if(!Q_strcasecmp (ext, "tga"))
	{
		LoadTGA(name, (byte **)pixels, width, height);
		return true;
	}
	else
	{
		Error ("%s doesn't have a known image extension", name);
	}

	return false;
}

