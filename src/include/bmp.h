#ifndef BMP_H
#define BMP_H

#include <stdint.h>
#include <stdbool.h>

#include "./polynomials.h"

typedef enum {
    LSB2,
    LSB4,
} InsertionMode;

// Use __attribute__((__packed__)) to prevent the compiler from adding padding between struct fields
typedef struct __attribute__((__packed__)) {
    uint16_t type;             // Magic number that identifies the file type
    uint32_t fileSize;         // Size of the file in bytes
    uint16_t reserved1;        // Reserved for future use
    uint16_t reserved2;        // Reserved for future use
    uint32_t offset;           // Offset from the beginning of the file to the bitmap data
    uint32_t headerSize;       // Size of this header in bytes
    uint32_t width;            // Image width in pixels in pixels
    uint32_t height;           // Image height in pixels in pixels
    uint16_t planes;           // Number of color planes
    uint16_t bitCount;         // Number of bits per pixel
    uint32_t compression;      // Compression methods used
    uint32_t imageSize;        // Size of the image in bytes
    uint32_t xPixelsPerMeter;  // Horizontal resolution of the image in pixels per meter
    uint32_t yPixelsPerMeter;  // Vertical resolution of the image in pixels per meter
    uint32_t colorsUsed;       // Number of colors in the palette  
    uint32_t colorsImportant;  // Minimum number of important colors
} BmpHeader;                   // 54 bytes

typedef struct {
    BmpHeader header;          // File header (54 bytes)
    uint8_t *metadata;         // Image metadata (header.offset - 54 bytes)
    uint8_t *pixels;           // Image pixels (header.fileSize - header.offset = header.width x header.height x (header.bitCount / 8) bytes)
} BmpImage;

BmpImage *bmpRead(const char *filename);

void bmpWrite(const char *filename, BmpImage *image);

void bmpFree(BmpImage *image);

void embedShadow(const char *imagePath, InsertionMode mode, uint8_t *shadow, uint16_t shadowNumber, uint32_t t);

// Extract all the sub-shadows v_ij from a BMP image
uint8_t* extractSubShadows(const char *imagePath, InsertionMode insertionMode, uint32_t t, uint16_t *shadowNumber);

// Recover the secret image from `f` and `g` polynomials, and indicate whether cheaters were detected or not
bool recoverSecretImage(BmpImage *imagePath, Polynomial *f[], Polynomial *g[], uint32_t t, uint8_t k);

#endif
