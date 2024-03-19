#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;

// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

// https://docs.microsoft.com/pl-pl/previous-versions/dd183376(v=vs.85)
typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

struct tagBITMAPFILEHEADER readHeader(FILE *file)
{
    struct tagBITMAPFILEHEADER bitmapHeader;
    fread(&bitmapHeader.bfType, sizeof(WORD), 1, file);
    fread(&bitmapHeader.bfSize, sizeof(DWORD), 1, file);
    fread(&bitmapHeader.bfReserved1, sizeof(WORD), 1, file);
    fread(&bitmapHeader.bfReserved2, sizeof(WORD), 1, file);
    fread(&bitmapHeader.bfOffBits, sizeof(DWORD), 1, file);
    return bitmapHeader;
}

struct tagBITMAPINFOHEADER readInfoHeader(FILE *file)
{
    struct tagBITMAPINFOHEADER bitmapInfoHeader;
    fread(&bitmapInfoHeader.biSize, sizeof(DWORD), 1, file);
    fread(&bitmapInfoHeader.biWidth, sizeof(LONG), 1, file);
    fread(&bitmapInfoHeader.biHeight, sizeof(LONG), 1, file);
    fread(&bitmapInfoHeader.biPlanes, sizeof(WORD), 1, file);
    fread(&bitmapInfoHeader.biBitCount, sizeof(WORD), 1, file);
    fread(&bitmapInfoHeader.biCompression, sizeof(DWORD), 1, file);
    fread(&bitmapInfoHeader.biSizeImage, sizeof(DWORD), 1, file);
    fread(&bitmapInfoHeader.biXPelsPerMeter, sizeof(LONG), 1, file);
    fread(&bitmapInfoHeader.biYPelsPerMeter, sizeof(LONG), 1, file);
    fread(&bitmapInfoHeader.biClrUsed, sizeof(DWORD), 1, file);
    fread(&bitmapInfoHeader.biClrImportant, sizeof(DWORD), 1, file);
    return bitmapInfoHeader;
}

void parseFile(char *filename)
{
    FILE *file = fopen(filename, "r");
    struct tagBITMAPFILEHEADER bitmapHeader = readHeader(file);
    struct tagBITMAPINFOHEADER bitmapInfoHeader = readInfoHeader(file);

    printf("BITMAPFILEHEADER:\n");
    printf("\t%-18s %04X\n", "bfType:", bitmapHeader.bfType);
    printf("\t%-18s %d\n", "bfSize:", bitmapHeader.bfSize);
    printf("\t%-18s %d\n", "bfReserved1:", bitmapHeader.bfReserved1);
    printf("\t%-18s %d\n", "bfReserved2:", bitmapHeader.bfReserved2);
    printf("\t%-18s %d\n", "bfOffBits:", bitmapHeader.bfOffBits);

    printf("BITMAPINFOHEADER:\n");
    printf("\t%-18s %04X\n", "biSize:", bitmapInfoHeader.biSize);
    printf("\t%-18s %d\n", "biWidth:", bitmapInfoHeader.biWidth);
    printf("\t%-18s %d\n", "biHeight:", bitmapInfoHeader.biHeight);
    printf("\t%-18s %d\n", "biPlanes:", bitmapInfoHeader.biPlanes);
    printf("\t%-18s %d\n", "biBitCount:", bitmapInfoHeader.biBitCount);
    printf("\t%-18s %d\n", "biCompression:", bitmapInfoHeader.biCompression);
    printf("\t%-18s %d\n", "biSizeImage:", bitmapInfoHeader.biSizeImage);
    printf("\t%-18s %d\n", "biXPelsPerMeter:", bitmapInfoHeader.biXPelsPerMeter);
    printf("\t%-18s %d\n", "biYPelsPerMeter:", bitmapInfoHeader.biYPelsPerMeter);
    printf("\t%-18s %d\n", "biClrUsed:", bitmapInfoHeader.biClrUsed);
    printf("\t%-18s %d\n", "biClrImportant:", bitmapInfoHeader.biClrImportant);

    fclose(file);
}

int main(int argc, char *argv[])
{
    parseFile(argv[1]);
}