//========================================================================================
// 
// File Name    : bmp.h
// Description  : JPEG decoder testbench 
// Release Date : 21/07/2017
// Author       : PolyU, UT Dallas DARClab
//                Shuangnan Liu, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 21/07/2017   1.0         PolyU, UTD DARClab  Reconstructs BMP file
//=======================================================================================

#ifndef BMP_H_
#define BMP_H_

#include <stdio.h>
#include <stdlib.h>

/*
 * File Structure
 *
 *  --------------
 * | File Header
 *  --------------
 * | Image Header
 *  --------------
 * | Color Table
 *  --------------
 * | Pixel Data
 *  --------------
 *
 * All of the integer values are stored in little-endian format.
 * e.g. 2835
 * big-endian:    0x00_00_0B_13
 * little-endian: 0x13_0B_00_00
 */

/*
 * File Header
 */
#pragma pack(push, 1) // structure byte alignment
typedef struct BMPFILEHEADER
{
    unsigned short int type;       // Contains the characters "BM" that
                                   // identify the file type
    unsigned       int size;       // File size
    unsigned short int reserved1;  // Unused, must be 0
    unsigned short int reserved2;  // Unused, must be 0
    unsigned       int off_bits;   // Offset to start of pixel data
}BmpFileHeader;
#pragma pack(pop)

/*
 * Image Header (DIB Header)
 *
 * It comes in two distinct formats.
 * 1. BMPCOREHEADER represents the OS/2 BMP format
 * 2. BMPINFOHEADER is the much more common Windows format
 *
 * There is no version field in the BMP definitions.
 * The only way to distinguish is examining the structure's size field.
 * The size of BMPCOREHEADER is 12 bytes; the size of BMPINFOHEADER is
 * at least 40 bytes.
 *
 * If the file uses BMPCOREHEADER, the pixel data cannot be compressed.
 */
typedef struct BMPINFOHEADER
{
    unsigned       int size;   // Header size, must be at least 40
                   int width;  // Image width
                   int height; // Image height, a negative value specifies that
                               // the pixel data is ordered from the top down
                               // rather than the normal bottom up.
                               // Image with a negative value may not be compressed.
    unsigned short int planes; // Must be 1
    unsigned short int bit_count;   // Bits per pixel--1, 4, 8, 16, 24, or 32
    unsigned       int compression; // Compression type: BI_RGB = 0,
                                    // BI_RLE8 = 1, BI_RLE4 = 2, or BI_BITFIELDS = 3
    unsigned       int size_image;  // Image size--May be zero if not compressed
    unsigned       int x_pixels_per_meter; // Preferred resolution in
                                           // pixels per meter horizontally
    unsigned       int y_pixels_per_meter; // Preferred resolution in
                                           // pixels per meter vertically
    unsigned       int color_used; // Number of entries in the color map
                                   // that are actually used
    unsigned       int color_important; // Number of significant colors;
                                        // 0 means all colors are important
}BmpInfoHeader;

typedef struct BMPCOREHEADER
{
    unsigned       int size;       // Header size, must be 12
    unsigned short int width;      // Image width
    unsigned short int height;     // Image height
    unsigned short int planes;     // Must be 1
    unsigned short int bit_count;  // Bit count--1, 4, 8, or 24
}BmpCoreHeader;

/* 
 * Color Palette
 */
typedef struct COLORTABLE4 // Windows format
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char alpha; // must be 0
}ColorTable4;

typedef struct COLORTABLE3 // OS/2 format
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
}ColorTable3;

/*
 * Magic Numbers
 */
#define BMP_BM 0x4D42 // "BM"
#define PIXELS_PER_METER 2835 // 72 DPI * 39.3701 inches
#define NUM_OF_COLORS 256

/*
 * Functions
 */
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

inline void
write_to_bmp(const char* fname,
             BmpFileHeader *file_header,
             BmpInfoHeader *info_header,
             ColorTable4 *palette,
             // unsigned char *palette,
             unsigned char *image_data)
{
    FILE *fp = NULL;
    fp = fopen(fname, "wb");
    if (fp == NULL)
    {
        perror("Cannot open the BMP output file.");
        exit(0);
    }

    // Write headers
    fwrite(file_header, sizeof(BmpFileHeader), 1, fp);
    fwrite(info_header, info_header->size, 1, fp);

    // Write color table, if exists
    if (palette != NULL)
    {
        int size = file_header->off_bits - sizeof(BmpFileHeader) - info_header->size;
        fwrite(palette, 1, size, fp);
    }

    // Write image data
    fwrite(image_data, 1, info_header->size_image, fp);

    fclose(fp);    
}

inline void
read_bmp(BmpFileHeader *file_header,
         BmpInfoHeader *info_header,
         unsigned char **palette,
         unsigned char **image_data,
         const char* fname)
{
    FILE *fp = NULL;
    fp = fopen(fname, "rb");
    if (fp == NULL)
    {
        perror("Cannot open the BMP input file.");
        exit(0);
    }

    // Read headers
    fread(file_header, 1, sizeof(BmpFileHeader), fp);
    fread(info_header, 1, sizeof(BmpInfoHeader), fp);

    // Read color table, if exists
    if (info_header->bit_count < 24)
    {
        int size = file_header->off_bits - sizeof(BmpFileHeader) - info_header->size;
        *palette = (unsigned char *)malloc(size);
        fread(*palette, 1, size, fp);
    }

    // Read image data
    *image_data = (unsigned char *)malloc(info_header->size_image);
    fread(*image_data, 1, info_header->size_image, fp);

    fclose(fp);
}

inline void
display_headers(BmpFileHeader *file_header,
                BmpInfoHeader *info_header)
{
    // File header
    printf("The image size is: %u bytes\n", file_header->size);
    // Info header
    printf("The image dimension is %u (width) x %d (height)\n",
            info_header->width, info_header->height);
    printf("Each pixel is %u bits\n", info_header->bit_count);
}

inline void
create_file_header(BmpFileHeader *file_header,
                   unsigned int size,
                   unsigned int offset)
{
    file_header->type = BMP_BM;
    file_header->size = size;
    file_header->reserved1 = 0;
    file_header->reserved2 = 0;
    file_header->off_bits = offset;
}

inline void
create_info_header(BmpInfoHeader *info_header,
                   int width,
                   int height,
                   unsigned short int bit_count)
{
    info_header->size = sizeof(BmpInfoHeader);
    info_header->width = width;
    info_header->height = height;
    info_header->planes = 1;
    info_header->bit_count = bit_count;
    info_header->compression = 0;
    info_header->size_image = width * height;
    info_header->x_pixels_per_meter = PIXELS_PER_METER;
    info_header->y_pixels_per_meter = PIXELS_PER_METER;
    info_header->color_used = NUM_OF_COLORS;
    info_header->color_important = 0;
}

inline void
create_palette(ColorTable4 *palette)
{
    for (int i = 0; i < NUM_OF_COLORS; i++)
    {
        palette[i].blue  = i;
        palette[i].green = i;
        palette[i].red   = i;
        palette[i].alpha = 0;
    }
}

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* BMP_H_ */
