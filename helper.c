#include "helper.h"
typedef struct {
    int width;
    int height;
    unsigned char *data;
} textureImage;

typedef struct
{
	GLubyte	* imageData;									/* Image Data (Up To 32 Bits) */
	GLuint	bpp;											/* Image Color Depth In Bits Per Pixel */
	GLuint	width;											/* Image Width */
	GLuint	height;											/* Image Height */
	GLuint	texID;											/* Texture ID Used To Select A Texture */
	GLuint	type;											/* Image Type (GL_RGB, GL_RGBA) */
} Texture;	


typedef struct
{
	GLubyte Header[12];									/* TGA File Header */
} TGAHeader;

typedef struct
{
	GLubyte		header[6];								/* First 6 Useful Bytes From The Header */
	GLuint		bytesPerPixel;							/* Holds Number Of Bytes Per Pixel Used In The TGA File */
	GLuint		imageSize;								/* Used To Store The Image Size When Setting Aside Ram */
	GLuint		temp;									/* Temporary Variable */
	GLuint		type;
	GLuint		Height;									/* Height of Image */
	GLuint		Width;									/* Width ofImage */
	GLuint		Bpp;									/* Bits Per Pixel */
} TGA;


TGAHeader tgaheader;									/* TGA header */
TGA tga;												/* TGA image data */


GLubyte uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	/* Uncompressed TGA Header */
GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	/* Compressed TGA Header */
BOOL LoadUncompressedTGA(Texture *,const char *,FILE *);	/* Load an Uncompressed file */
BOOL LoadCompressedTGA(Texture *,const char *,FILE *);		/* Load a Compressed file */


BOOL LoadTGA(Texture * texture,const char * filename)				/* Load a TGA file */
{
	FILE * fTGA;													/* File pointer to texture file */
	fTGA = fopen(filename, "rb");									/* Open file for reading */

	if(fTGA == NULL)												/* If it didn't open.... */
	{
		printf("Error could not open texture file");				/* Display an error message */
		return False;												/* Exit function */
	}

	if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)			/* Attempt to read 12 byte header from file */
	{
		printf("Error could not read file header");					/* If it fails, display an error message */
		if(fTGA != NULL)												/* Check to seeiffile is still open */
		{
			fclose(fTGA);												/* If it is, close it */
		}
		return False;													/* Exit function */
	}

	if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)	/* See if header matches the predefined header of */
	{																/* an Uncompressed TGA image */
		LoadUncompressedTGA(texture, filename, fTGA);					/* If so, jump to Uncompressed TGA loading code */
	}
	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)	/* See if header matches the predefined header of */
	{																	/* an RLE compressed TGA image */
		LoadCompressedTGA(texture, filename, fTGA);						/* If so, jump to Compressed TGA loading code */
	}
	else																/* If header matches neither type */
	{
		printf("Error TGA file be type 2 or type 10 ");				/* Display an error */
		fclose(fTGA);
		return False;												/* Exit function */
	}
	return True;														/* All went well, continue on */
}

BOOL LoadUncompressedTGA(Texture * texture, const char * filename, FILE * fTGA)	/* Load an uncompressed TGA (note, much of this code is based on NeHe's */
{
	GLuint cswap;															/* TGA Loading code nehe.gamedev.net) */
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)				/* Read TGA header */
	{
		printf("Error could not read info header");							/* Display error */
		if(fTGA != NULL)													/* if file is still open */
		{
			fclose(fTGA);													/* Close it */
		}
		return False;														/* Return failular */
	}

	texture->width  = tga.header[1] * 256 + tga.header[0];					/* Determine The TGA Width	(highbyte*256+lowbyte) */
	texture->height = tga.header[3] * 256 + tga.header[2];					/* Determine The TGA Height	(highbyte*256+lowbyte) */
	texture->bpp	= tga.header[4];										/* Determine the bits per pixel */
	tga.Width		= texture->width;										/* Copy width into local structure */
	tga.Height		= texture->height;										/* Copy height into local structure */
	tga.Bpp			= texture->bpp;											/* Copy BPP into local structure */

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))	/* Make sure all information is valid */
	{
		printf("Error invalid texture information");						/* Display Error */
		if(fTGA != NULL)													/* Check if file is still open */
		{
			fclose(fTGA);													/* If so, close it */
		}
		return False;														/* Return failed */
	}

	if(texture->bpp == 24)													/* If the BPP of the image is 24... */
	{
		texture->type	= GL_RGB;											/* Set Image type to GL_RGB */
	}
	else																	/* Else if its 32 BPP */
	{
		texture->type	= GL_RGBA;											/* Set image type to GL_RGBA */
	}

	tga.bytesPerPixel	= (tga.Bpp / 8);									/* Compute the number of BYTES per pixel */
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		/* Compute the total amout ofmemory needed to store data */
	texture->imageData	= (GLubyte*)malloc(tga.imageSize);							/* Allocate that much memory */

	if(texture->imageData == NULL)											/* If no space was allocated */
	{
		printf("Error could not allocate memory for image");				/* Display Error */
		fclose(fTGA);														/* Close the file */
		return False;														/* Return failed */
	}

	if(fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)	/* Attempt to read image data */
	{
		printf("Error could not read image data");							/* Display Error */
		if(texture->imageData != NULL)										/* If imagedata has data in it */
		{
			free(texture->imageData);										/* Delete data from memory */
		}
		fclose(fTGA);														/* Close file */
		return False;														/* Return failed */
	}

	/* Byte Swapping Optimized By Steve Thomas */
	for(cswap = 0; cswap < (unsigned int)tga.imageSize; cswap += tga.bytesPerPixel)
	{
		
		//texture->imageData[cswap] ^= texture->imageData[cswap+2] ^=
	//		texture->imageData[cswap] ^= texture->imageData[cswap+2];
		//texture->imageData[cswap+2] ^= texture->imageData[cswap] ^=
		//texture->imageData[cswap+2] ^= texture->imageData[cswap];
	}

	fclose(fTGA);															/* Close file */
	return True;															/* Return success */
}

BOOL LoadCompressedTGA(Texture * texture,const char * filename, FILE * fTGA)		/* Load COMPRESSED TGAs */
{
	GLuint pixelcount	= tga.Height * tga.Width;							/* Nuber of pixels in the image */
	GLuint currentpixel	= 0;												/* Current pixel being read */
	GLuint currentbyte	= 0;												/* Current byte */
	GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);			/* Storage for 1 pixel */

	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)				/* Attempt to read header */
	{
		printf("Error could not read info header");							/* Display Error */
		if(fTGA != NULL)													/* If file is open */
		{
			fclose(fTGA);													/* Close it */
		}
		return False;														/* Return failed */
	}

	texture->width  = tga.header[1] * 256 + tga.header[0];					/* Determine The TGA Width	(highbyte*256+lowbyte) */
	texture->height = tga.header[3] * 256 + tga.header[2];					/* Determine The TGA Height	(highbyte*256+lowbyte) */
	texture->bpp	= tga.header[4];										/* Determine Bits Per Pixel */
	tga.Width		= texture->width;										/* Copy width to local structure */
	tga.Height		= texture->height;										/* Copy width to local structure */
	tga.Bpp			= texture->bpp;											/* Copy width to local structure */

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))	/*Make sure all texture info is ok */
	{
		printf("Error Invalid texture information");						/* If it isnt...Display error */
		if(fTGA != NULL)													/* Check if file is open */
		{
			fclose(fTGA);													/* Ifit is, close it */
		}
		return False;														/* Return failed */
	}

	tga.bytesPerPixel	= (tga.Bpp / 8);									/* Compute BYTES per pixel */
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		/* Compute amout of memory needed to store image */
	texture->imageData	= (GLubyte *)malloc(tga.imageSize);					/* Allocate that much memory */

	if(texture->imageData == NULL)											/* If it wasnt allocated correctly.. */
	{
		printf("Error could not allocate memory for image");				/* Display Error */
		fclose(fTGA);														/* Close file */
		return False;														/* Return failed */
	}

	do
	{
		GLubyte chunkheader = 0;											/* Storage for "chunk" header */

		if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0)				/* Read in the 1 byte header */
		{
			printf("Error could not read RLE header");						/*Display Error */
			if(fTGA != NULL)												/* If file is open */
			{
				fclose(fTGA);												/* Close file */
			}
			if(texture->imageData != NULL)									/* If there is stored image data */
			{
				free(texture->imageData);									/* Delete image data */
			}
			return False;													/* Return failed */
		}

		if(chunkheader < 128)												/* If the ehader is < 128, it means the that is the number of RAW color packets minus 1 */
		{
			short counter;													/* that follow the header */
			chunkheader++;													/* add 1 to get number of following color values */
			for(counter = 0; counter < chunkheader; counter++)				/* Read RAW color values */
			{
				if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) /* Try to read 1 pixel */
				{
					printf("Error could not read image data");				/* IF we cant, display an error */

					if(fTGA != NULL)										/* See if file is open */
					{
						fclose(fTGA);										/* If so, close file */
					}

					if(colorbuffer != NULL)									/* See if colorbuffer has data in it */
					{
						free(colorbuffer);									/* If so, delete it */
					}

					if(texture->imageData != NULL)										/* See if there is stored Image data */
					{
						free(texture->imageData);										/* If so, delete it too */
					}

					return False;														/* Return failed */
				}
																						/* write to memory */
				texture->imageData[currentbyte		] = colorbuffer[2];				    /* Flip R and B vcolor values around in the process */
				texture->imageData[currentbyte + 1	] = colorbuffer[1];
				texture->imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												/* if its a 32 bpp image */
				{
					texture->imageData[currentbyte + 3] = colorbuffer[3];				/* copy the 4th byte */
				}

				currentbyte += tga.bytesPerPixel;										/* Increase thecurrent byte by the number of bytes per pixel */
				currentpixel++;															/* Increase current pixel by 1 */

				if(currentpixel > pixelcount)											/* Make sure we havent read too many pixels */
				{
					printf("Error too many pixels read : %s\n", filename);								/* if there is too many... Display an error! */

					if(fTGA != NULL)													/* If there is a file open */
					{
						fclose(fTGA);													/* Close file */
					}

					if(colorbuffer != NULL)												/* If there is data in colorbuffer */
					{
						free(colorbuffer);												/* Delete it */
					}

					if(texture->imageData != NULL)										/* If there is Image data */
					{
						free(texture->imageData);										/* delete it */
					}

					return False;														/* Return failed */
				}
			}
		}
		else																			/* chunkheader > 128 RLE data, next color  reapeated chunkheader - 127 times */
		{
			short counter;
			chunkheader -= 127;															/* Subteact 127 to get rid of the ID bit */
			if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)		/* Attempt to read following color values */
			{
				printf("Error could not read from file");			/* If attempt fails.. Display error (again) */

				if(fTGA != NULL)														/* If thereis a file open */
				{
					fclose(fTGA);														/* Close it */
				}

				if(colorbuffer != NULL)													/* If there is data in the colorbuffer */
				{
					free(colorbuffer);													/* delete it */
				}

				if(texture->imageData != NULL)											/* If thereis image data */
				{
					free(texture->imageData);											/* delete it */
				}

				return False;															/* return failed */
			}

			for(counter = 0; counter < chunkheader; counter++)					/* copy the color into the image data as many times as dictated */
			{																			/* by the header */
				texture->imageData[currentbyte		] = colorbuffer[2];					/* switch R and B bytes areound while copying */
				texture->imageData[currentbyte + 1	] = colorbuffer[1];
				texture->imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												/* If TGA images is 32 bpp */
				{
					texture->imageData[currentbyte + 3] = colorbuffer[3];				/* Copy 4th byte */
				}
				currentbyte += tga.bytesPerPixel;										/* Increase current byte by the number of bytes per pixel */
				currentpixel++;															/* Increase pixel count by 1 */
				if(currentpixel > pixelcount)											/* Make sure we havent written too many pixels */
				{
					printf("Error too many pixels read");								/* if there is too many... Display an error! */
					if(fTGA != NULL)													/* If there is a file open */
					{
						fclose(fTGA);													/* Close file */
					}
					if(colorbuffer != NULL)												/* If there is data in colorbuffer */
					{
						free(colorbuffer);												/* Delete it */
					}
					if(texture->imageData != NULL)										/* If there is Image data */
					{
						free(texture->imageData);										/* delete it */
					}
					return False;														/* Return failed */
				}
			}
		}
	}
	while(currentpixel < pixelcount);													/* Loop while there are still pixels left */
	fclose(fTGA);																		/* Close the file */
	return True;																		/* return success */
}

GLuint load_sequence_from_disk_image(char *image, GLuint itexture){
	Texture texture;
	if (LoadTGA(&texture, image))
	{
			glBindTexture(GL_TEXTURE_RECTANGLE_NV, itexture);
			glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_BGRA, texture.width, texture.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture.imageData);
			glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			if (texture.imageData)						/* If Texture Image Exists */
			{
				free(texture.imageData);					/* Free The Texture Image Memory */
			}
	}
	
	
	return texture.texID;
}

GLuint load_image(char *image, int gui_c){
	Texture texture;
	if (LoadTGA(&texture, image))
	{
			glGenTextures(1, &texture.texID);				/* Create The Texture */
			glBindTexture(GL_TEXTURE_RECTANGLE_NV, texture.texID);
			glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, texture.type, texture.width, texture.height, 0, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
			glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			if (texture.imageData)						/* If Texture Image Exists */
			{
				free(texture.imageData);					/* Free The Texture Image Memory */
			}
	}
	
	
	return texture.texID;
}

GLuint load_texture2d_image(char *image, int gui_c){
	Texture texture;
	if (LoadTGA(&texture, image))
	{
			glGenTextures(1, &texture.texID);				/* Create The Texture */
			glBindTexture(GL_TEXTURE_2D, texture.texID);
			glTexImage2D(GL_TEXTURE_2D, 0, texture.type, texture.width, texture.height, 0, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			if (texture.imageData)						/* If Texture Image Exists */
			{
				free(texture.imageData);					/* Free The Texture Image Memory */
			}
	}
	
	
	return texture.texID;
}

void set_wireframe_render(void){
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
}
void set_solid_render(void){
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);	
}
void set_color_from_rgba(int r, int g, int b, int a){
	glColor4f((r/2.56)/100,(g/2.56)/100, (b/2.56)/100, (a/2.56)/100);
}
void set_color_from_rgb(int r, int g, int b){
	glColor3f((r/2.56)/100,(g/2.56)/100, (b/2.56)/100);
}

