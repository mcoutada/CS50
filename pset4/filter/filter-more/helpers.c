#include "helpers.h"
#include <math.h> // this is needed for round, sqrt, pow
// Convert image to grayscale

void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int avrg = (image[i][j].rgbtRed + image[i][j].rgbtBlue + image[i][j].rgbtGreen);
            image[i][j].rgbtRed = round(avrg / 3.0); // divide by 3.0 to cast the operation to float
            image[i][j].rgbtBlue = round(avrg / 3.0);
            image[i][j].rgbtGreen = round(avrg / 3.0);
        }
    }
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int sepiaR = round(.393 * image[i][j].rgbtRed + .769 * image[i][j].rgbtGreen + .189 * image[i][j].rgbtBlue);
            int sepiaG = round(.349 * image[i][j].rgbtRed + .686 * image[i][j].rgbtGreen + .168 * image[i][j].rgbtBlue);
            int sepiaB = round(.272 * image[i][j].rgbtRed + .534 * image[i][j].rgbtGreen + .131 * image[i][j].rgbtBlue);

            image[i][j].rgbtRed = sepiaR < 255 ? sepiaR : 255;
            image[i][j].rgbtGreen = sepiaG < 255 ? sepiaG : 255;
            image[i][j].rgbtBlue = sepiaB < 255 ? sepiaB : 255;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE temp;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++) //we loop until half the image
        {
            temp = image[i][j];
            image[i][j] = image[i][(width - 1) - j]; // minus 1 because we start counting from 0
            image[i][(width - 1) - j] = temp;
        }
    }
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // create a new pixel data structure with all the attributes needed for this function
    typedef struct
    {
        float rSum;
        float gSum;
        float bSum;
        float cnt;
    } RGBTRIPLEBLUR;

    RGBTRIPLEBLUR temp[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            temp[i][j].rSum = 0;
            temp[i][j].gSum = 0;
            temp[i][j].bSum = 0;
            temp[i][j].cnt = 0;

            // k and l are used to loop the surroundings of the pixel, they can be -1, 0 or 1, that way we get the 9 pixels surrounding the current pixel (included)
            for (int k = -1; k < 2; k++)
            {
                for (int l = -1; l < 2; l++)
                {
                    if (i + k >= 0 && i + k < height && j + l >= 0 && j + l < width)
                    {
                        temp[i][j].rSum += image[i + k][j + l].rgbtRed;
                        temp[i][j].gSum += image[i + k][j + l].rgbtBlue;
                        temp[i][j].bSum += image[i + k][j + l].rgbtGreen;
                        temp[i][j].cnt++;
                    }
                }
            }
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtRed = round(temp[i][j].rSum / temp[i][j].cnt);
            image[i][j].rgbtBlue = round(temp[i][j].gSum / temp[i][j].cnt);
            image[i][j].rgbtGreen = round(temp[i][j].bSum / temp[i][j].cnt);
        }
    }
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Gx and Gy kernels for the Sobel operator, per exercise statement
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // create a new pixel data structure with all the attributes needed for this function
    typedef struct
    {
        float rx;
        float gx;
        float bx;
        float ry;
        float gy;
        float by;
    } RGBTRIPLEEDGE;

    RGBTRIPLEEDGE temp[height][width];

    // loop all pixels
    // notice that we don't need an extra outer layer of black pixels, the idea of that is that as being black = 0, 0*something = 0, so it doesn't affect the sobel operator
    // but we will use the same pixel surrounding scan that we used in blur, that is already prepared to detect an out of bounds pixel
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            temp[i][j].rx = 0;
            temp[i][j].gx = 0;
            temp[i][j].bx = 0;
            temp[i][j].ry = 0;
            temp[i][j].gy = 0;
            temp[i][j].by = 0;

            for (int k = -1; k < 2; k++)
            {
                for (int l = -1; l < 2; l++)
                {
                    if (i + k >= 0 && i + k < height && j + l >= 0 && j + l < width)
                    {
                        // in a 3x3 matrix, the center is at index (1,1), that is why we use 1 + k and 1 + l, the pixel being processed is at the center of the Gx and Gy matrix
                        temp[i][j].rx += Gx[1 + k][1 + l] * image[i + k][j + l].rgbtRed;
                        temp[i][j].gx += Gx[1 + k][1 + l] * image[i + k][j + l].rgbtGreen;
                        temp[i][j].bx += Gx[1 + k][1 + l] * image[i + k][j + l].rgbtBlue;

                        temp[i][j].ry += Gy[1 + k][1 + l] * image[i + k][j + l].rgbtRed;
                        temp[i][j].gy += Gy[1 + k][1 + l] * image[i + k][j + l].rgbtGreen;
                        temp[i][j].by += Gy[1 + k][1 + l] * image[i + k][j + l].rgbtBlue;
                    }
                }
            }
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int edgeR = round(sqrt(pow(temp[i][j].rx, 2) + pow(temp[i][j].ry, 2)));
            int edgeG = round(sqrt(pow(temp[i][j].gx, 2) + pow(temp[i][j].gy, 2)));
            int edgeB = round(sqrt(pow(temp[i][j].bx, 2) + pow(temp[i][j].by, 2)));

            image[i][j].rgbtRed = edgeR < 255 ? edgeR : 255;
            image[i][j].rgbtGreen = edgeG < 255 ? edgeG : 255;
            image[i][j].rgbtBlue = edgeB < 255 ? edgeB : 255;
        }
    }
}