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

#define HEADERLENGTH 54;

void createHeader(FILE *file, int width, int height)
{
    BITMAPFILEHEADER bmfHeader;
    bmfHeader.bfType = 0x4D42;
    bmfHeader.bfSize = (width * height) + HEADERLENGTH;
    bmfHeader.bfReserved1 = 0;
    bmfHeader.bfReserved2 = 0;
    bmfHeader.bfOffBits = HEADERLENGTH;

    BITMAPINFOHEADER bmiHeader;
    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiHeader.biWidth = width;
    bmiHeader.biHeight = height;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 1;
    bmiHeader.biCompression = 0;
    bmiHeader.biSizeImage = bmiHeader.biWidth * bmiHeader.biHeight;
    bmiHeader.biXPelsPerMeter = 2835;
    bmiHeader.biYPelsPerMeter = 2835;
    bmiHeader.biClrUsed = 0;
    bmiHeader.biClrImportant = 0;

    printf("BITMAPFILEHEADER:\n");
    printf("\t%-18s %04X\n", "bfType:", bmfHeader.bfType);
    printf("\t%-18s %d\n", "bfSize:", bmfHeader.bfSize);
    printf("\t%-18s 0X%X\n", "bfReserved1:", bmfHeader.bfReserved1);
    printf("\t%-18s 0X%X\n", "bfReserved2:", bmfHeader.bfReserved2);
    printf("\t%-18s %d\n", "bfOffBits:", bmfHeader.bfOffBits);

    printf("BITMAPINFOHEADER:\n");
    printf("\t%-18s %d\n", "biSize:", bmiHeader.biSize);
    printf("\t%-18s %d\n", "biWidth:", bmiHeader.biWidth);
    printf("\t%-18s %d\n", "biHeight:", bmiHeader.biHeight);
    printf("\t%-18s %d\n", "biPlanes:", bmiHeader.biPlanes);
    printf("\t%-18s %d\n", "biBitCount:", bmiHeader.biBitCount);
    printf("\t%-18s %d\n", "biCompression:", bmiHeader.biCompression);
    printf("\t%-18s %d\n", "biSizeImage:", bmiHeader.biSizeImage);
    printf("\t%-18s %d\n", "biXPelsPerMeter:", bmiHeader.biXPelsPerMeter);
    printf("\t%-18s %d\n", "biYPelsPerMeter:", bmiHeader.biYPelsPerMeter);
    printf("\t%-18s %d\n", "biClrUsed:", bmiHeader.biClrUsed);
    printf("\t%-18s %d\n", "biClrImportant:", bmiHeader.biClrImportant);

    fwrite(&bmfHeader.bfType, sizeof(WORD), 1, file);
    fwrite(&bmfHeader.bfSize, sizeof(DWORD), 1, file);
    fwrite(&bmfHeader.bfReserved1, sizeof(WORD), 1, file);
    fwrite(&bmfHeader.bfReserved2, sizeof(WORD), 1, file);
    fwrite(&bmfHeader.bfOffBits, sizeof(DWORD), 1, file);

    fseek(file, 14, SEEK_SET);

    fwrite(&bmiHeader.biSize, sizeof(DWORD), 1, file);
    fwrite(&bmiHeader.biWidth, sizeof(LONG), 1, file);
    fwrite(&bmiHeader.biHeight, sizeof(LONG), 1, file);
    fwrite(&bmiHeader.biPlanes, sizeof(WORD), 1, file);
    fwrite(&bmiHeader.biBitCount, sizeof(WORD), 1, file);
    fwrite(&bmiHeader.biCompression, sizeof(DWORD), 1, file);
    fwrite(&bmiHeader.biSizeImage, sizeof(DWORD), 1, file);
    fwrite(&bmiHeader.biXPelsPerMeter, sizeof(LONG), 1, file);
    fwrite(&bmiHeader.biYPelsPerMeter, sizeof(LONG), 1, file);
    fwrite(&bmiHeader.biClrUsed, sizeof(DWORD), 1, file);
    fwrite(&bmiHeader.biClrImportant, sizeof(DWORD), 1, file);
}

void parseFile(char *outputFilename)
{
    FILE *file = fopen(outputFilename, "wb");

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }
    int width = 256;
    int height = 256;

    createHeader(file, width, height);

    for (int i = 0; i < width * height / 2; i++)
    {
        char color[1] = {i % 250};
        fwrite(color, sizeof(unsigned char), 1, file);
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    parseFile(argv[1]);
}