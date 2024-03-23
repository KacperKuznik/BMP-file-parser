#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

void copyHeader(FILE *file, FILE *outputFile, DWORD length)
{
    unsigned char *header = (unsigned char *)malloc(length);
    if (header == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    fseek(file, 0, SEEK_SET);
    fread(header, sizeof(unsigned char), length, file);
    if (outputFile != NULL)
    {
        fwrite(header, sizeof(unsigned char), length, outputFile);
    }
    free(header);
}

void printHistogram(int blueArr[], int greenArr[], int redArr[], int height, int width)
{
    float max = height * width;
    printf("Blue:\n");
    for (int i = 0; i < 16; i++)
    {
        printf("%d-%d: %.2f%%\n", i * 16, (i + 1) * 16 - 1, (blueArr[i] / max) * 100);
    }
    printf("Green:\n");
    for (int i = 0; i < 16; i++)
    {
        printf("%d-%d: %.2f%%\n", i * 16, (i + 1) * 16 - 1, (greenArr[i] / max) * 100);
    }
    printf("Red:\n");
    for (int i = 0; i < 16; i++)
    {
        printf("%d-%d: %.2f%%\n", i * 16, (i + 1) * 16 - 1, (redArr[i] / max) * 100);
    }
}

int binToInt(char bits[]){
    int value = 0;
    for(int i = 0; i < 8; i++){
        value += bits[i] & 1 ? pow(2, i) : 0;
    }
    return value;
}

void intToBin(int value, char* bits){
    for(int i = 0; i < 8; i++){
        bits[i] = value & 1;
        value >>= 1;
    }
}

char * decodeText(FILE* file, struct tagBITMAPFILEHEADER header, struct tagBITMAPINFOHEADER infoHeader){
    int rowLength = ((infoHeader.biBitCount * infoHeader.biWidth + 31) / 32) * 4;
    int padding = rowLength - (infoHeader.biBitCount * infoHeader.biWidth / 8);
    unsigned char *row = (unsigned char *)malloc(rowLength * sizeof(unsigned char));

    unsigned char hiddenChar[8];
    unsigned char* decodedMessage = malloc(sizeof(unsigned char) * 256);
    int charCount = 0;
    int charIndex = 0;
    int messageLength = -1;

    for (int rowIndex = 0; rowIndex < infoHeader.biHeight; rowIndex++)
    {
        fseek(file, rowIndex * rowLength + header.bfOffBits, SEEK_SET);
        fread(row, sizeof(unsigned char), rowLength, file);

        for (int idx = 0; idx < rowLength - padding; idx++)
        {
            hiddenChar[charIndex] = row[idx];
            charIndex++;
            if(charIndex >= 8 && (messageLength == -1 || charCount < messageLength)){
                // decoding length
                if(messageLength == -1){
                    messageLength = binToInt(hiddenChar);
                    for(int i = 0; i < 8; i++){
                        printf("%d", hiddenChar[i] & 1, binToInt(hiddenChar));
                    }
                    printf(" -> %d\n", messageLength);
                }
                // decoding character
                else{
                    unsigned char c = (char)binToInt(hiddenChar);
                    decodedMessage[charCount] = c;
                    charCount++;
                    for(int i = 0; i < 8; i++){
                        printf("%d", hiddenChar[i] & 1, binToInt(hiddenChar));
                    }
                    printf(" -> %c\n", (char)c);
                }
                memset(hiddenChar, '\0', 8);
                charIndex = 0;
            }
        }
    }
    free(row);
    return decodedMessage;
}

void encodeText(FILE* file, char* text, struct tagBITMAPFILEHEADER header, struct tagBITMAPINFOHEADER infoHeader){
    int rowLength = ((infoHeader.biBitCount * infoHeader.biWidth + 31) / 32) * 4;
    int padding = rowLength - (infoHeader.biBitCount * infoHeader.biWidth / 8);
    unsigned char *row = (unsigned char *)malloc(rowLength * sizeof(unsigned char));

    int textIndex = 0;
    char encodeChar[8];
    intToBin(strlen(text), encodeChar);
    int encodeIndex = 0;

    for (int rowIndex = 0; rowIndex < infoHeader.biHeight; rowIndex++)
    {
        fseek(file, rowIndex * rowLength + header.bfOffBits, SEEK_SET);

        fread(row, sizeof(unsigned char), rowLength, file);

        for (int idx = 0; idx < rowLength - padding; idx++)
        {

            if(textIndex < strlen(text)){
                unsigned char* value = malloc(sizeof(unsigned char) + 1);
                printf("before readpointer: %d\n", ftell(file));
                fread(value, sizeof(unsigned char), 1, file);
                printf("after readpointer: %d\n", ftell(file));
                fseek(file, -1, SEEK_CUR);
                printf("%d  ", value[0]);
                printf("after fseek pointer: %d\n", ftell(file));
                if ((encodeChar[encodeIndex] & 1) != (value[0] & 1)) {
                    value[0] ^= 1;
                }
                printf("%d\n", value[0]);
                fwrite(value, sizeof(unsigned char), 1, file);
                printf("after write pointer: %d\n", ftell(file));
                encodeIndex++;
                if(encodeIndex >= 8){
                    intToBin((int)text[textIndex], encodeChar);
                    textIndex++;
                    encodeIndex = 0;
                 }
                free(value);
            }

        }
    }
    free(row);
}

void parseFile(char *filename, char *output, char text[])
{
    FILE *file = fopen(filename, "r+b");
    FILE *outputFile = fopen(output, "wb");

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    struct tagBITMAPFILEHEADER bitmapHeader = readHeader(file);
    struct tagBITMAPINFOHEADER bitmapInfoHeader = readInfoHeader(file);

    printf("BITMAPFILEHEADER:\n");
    printf("\t%-18s %04X\n", "bfType:", bitmapHeader.bfType);
    printf("\t%-18s %d\n", "bfSize:", bitmapHeader.bfSize);
    printf("\t%-18s 0X%X\n", "bfReserved1:", bitmapHeader.bfReserved1);
    printf("\t%-18s 0X%X\n", "bfReserved2:", bitmapHeader.bfReserved2);
    printf("\t%-18s %d\n", "bfOffBits:", bitmapHeader.bfOffBits);

    printf("BITMAPINFOHEADER:\n");
    printf("\t%-18s %d\n", "biSize:", bitmapInfoHeader.biSize);
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

    if (bitmapInfoHeader.biCompression == 0 && bitmapInfoHeader.biBitCount == 24)
    {

        if (outputFile != NULL)
        {
            copyHeader(file, outputFile, bitmapHeader.bfOffBits);
        }
        int rowLength = ((bitmapInfoHeader.biBitCount * bitmapInfoHeader.biWidth + 31) / 32) * 4;
        int padding = rowLength - (bitmapInfoHeader.biBitCount * bitmapInfoHeader.biWidth / 8);
        unsigned char *row = (unsigned char *)malloc(rowLength * sizeof(unsigned char));

        int blueArray[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int greenArray[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int redArray[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        fseek(file, bitmapHeader.bfOffBits, SEEK_SET);
        if(outputFile != NULL){
            fseek(outputFile, bitmapHeader.bfOffBits, SEEK_SET);
        }

        // changing to gray
        for (int rowIndex = 0; rowIndex < bitmapInfoHeader.biHeight; rowIndex++)
        {

            fseek(file, rowIndex * rowLength + bitmapHeader.bfOffBits, SEEK_SET);
            if(outputFile != NULL){
                fseek(outputFile, rowIndex * rowLength + bitmapHeader.bfOffBits, SEEK_SET);
            }
            fread(row, sizeof(unsigned char), rowLength, file);

            for (int idx = 0; idx < rowLength - padding; idx += 3)
            {
                int blue = row[idx];
                int green = row[idx + 1];
                int red = row[idx + 2];

                blueArray[blue / 16]++;
                greenArray[green / 16]++;
                redArray[red / 16]++;

                if (outputFile != NULL)
                {
                    unsigned char gray = (unsigned char)((blue + green + red) / 3);
                    unsigned char grayData[3] = {gray, gray, gray};
                    fwrite(grayData, sizeof(unsigned char), 3, outputFile);
                }
            }
            
            if(outputFile != NULL){
                unsigned char paddingData[1] = {0};
                for (int i = 0; i < padding; i++)
                {
                    fwrite(paddingData, sizeof(unsigned char), 1, outputFile);
                }
            }

        }
        printHistogram(blueArray, greenArray, redArray, bitmapInfoHeader.biHeight, bitmapInfoHeader.biWidth);
        
        fseek(file, bitmapHeader.bfOffBits, SEEK_SET);
        if(outputFile != NULL && text != NULL){
            fseek(outputFile, bitmapHeader.bfOffBits, SEEK_SET);
        }


        char c;
        printf("Decode hidden message? [Y/n]\n");
        scanf("%c", &c);
        if(c == 'y' || c == 'Y'){
            char* decodedMessage = decodeText(file, bitmapHeader, bitmapInfoHeader);
            printf("Decoded message: %s\n", decodedMessage);
            free(decodedMessage);
        }
        if(outputFile != NULL && text != NULL){
            encodeText(outputFile, text, bitmapHeader, bitmapInfoHeader);
            printf("Encoded message\n");
        }
    }
    fclose(file);
    fclose(outputFile);
}

int main(int argc, char *argv[])
{
    parseFile(argv[1], argv[2], argv[3]);
}