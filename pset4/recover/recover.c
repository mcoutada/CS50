#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        return 1; // return 1 if it's not executed with exactly one command-line argument (per exercise statement)
    }

    FILE *inFile = fopen(argv[1], "r"); // pointer var to open card.raw in read mode

    if (inFile == NULL)
    {
        return 1; // return 1 if card.raw can't be opened (either doesn't exist, we don't have rights, is locked/can't be read, etc.) (per exercise statement)
    }

    // var to store the chunks. Unsigned = positive, hence values range == [0 to 255] (otherwise range = [-127 to 128])
    unsigned char chnkBuff[512];
    char jpgName[8];  // jpgName is 8 chars long, ###.jpg + NULL terminator
    int fileNoID = 0; // simple counter to generate the jpgNames
    FILE *outFile;    // pointer to a file, we will use this var to write the jpg files

    // fread reads a chunk of card.raw and saves it into chnkBuff, if that chunk is of 512 bytes, it will return 1, if less (last chunk of the file) it returns 0
    while (fread(chnkBuff, sizeof(chnkBuff), 1, inFile) == 1)
    {
        // see if the first 4 bytes of the chunk in chnkBuff are a jpg header
        if (chnkBuff[0] == 0xff && chnkBuff[1] == 0xd8 && chnkBuff[2] == 0xff && (chnkBuff[3] & 0xe0) == 0xe0)
        {
            if (fileNoID > 0) // close the previous file (unless it's the first one and there's no previous one to close)
            {
                fclose(outFile);
            }
            // save into the jpgName var, a string consisting in the file No ID right padded with 0s up to 3 positions, i.e fileNoID = 5 --> jpgName = 005.jpg
            sprintf(jpgName, "%03i.jpg", fileNoID);
            outFile = fopen(jpgName, "w"); // open the new file in write mode
            fileNoID++;
        }
        // take the 512 bytes read from inFile (and saved into chnkBuff) and write it into outFile
        // note: if you want to to run this script with cards.raw (instead of check50), you will need to execute the command below only if (fileNoID > 0), this is because cards.raw comes with some garbage until reaching the first header, I recommend you to download it and open it with an hex editor (see if you find the easter egg!)
        fwrite(chnkBuff, sizeof(chnkBuff), 1, outFile);
    }

    fclose(inFile);  // close card.raw
    fclose(outFile); // close last jpg file
}