#include "helpers.h"
#include <math.h> // this is needed for round
// Convert image to grayscale
// Convert image to grayscale
// Convert image to grayscale
// Convert image to grayscale
// Convert image to grayscale
// Convert image to grayscale

void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int avrg = (image[i][j].rgbtRed + image[i][j].rgbtBlue + image[i][j].rgbtGreen);
            image[i][j].rgbtRed = round(avrg / 3.0);
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
            int sepiaRed = round(.393 * image[i][j].rgbtRed + .769 * image[i][j].rgbtGreen + .189 * image[i][j].rgbtBlue);
            int sepiaGreen = round(.349 * image[i][j].rgbtRed + .686 * image[i][j].rgbtGreen + .168 * image[i][j].rgbtBlue);
            int sepiaBlue = round(.272 * image[i][j].rgbtRed + .534 * image[i][j].rgbtGreen + .131 * image[i][j].rgbtBlue);

            image[i][j].rgbtRed = sepiaRed < 255 ? sepiaRed : 255;
            image[i][j].rgbtGreen = sepiaGreen < 255 ? sepiaGreen : 255;
            image[i][j].rgbtBlue = sepiaBlue < 255 ? sepiaBlue : 255;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE temp;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            temp = image[i][j];
            image[i][j] = image[i][(width - 1) - j];
            image[i][(width - 1) - j] = temp;
        }
    }
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    typedef struct
    {
        float redSum;
        float greenSum;
        float blueSum;
        float cnt;
    } RGBTRIPLESUM;

    RGBTRIPLESUM temp[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            temp[i][j].redSum = 0;
            temp[i][j].greenSum = 0;
            temp[i][j].blueSum = 0;
            temp[i][j].cnt = 0;

            for (int k = -1; k < 2; k++)
            {
                for (int l = -1; l < 2; l++)
                {
                    if (i + k >= 0 && i + k < height && j + l >= 0 && j + l < width)
                    {
                        temp[i][j].redSum += image[i + k][j + l].rgbtRed;
                        temp[i][j].greenSum += image[i + k][j + l].rgbtBlue;
                        temp[i][j].blueSum += image[i + k][j + l].rgbtGreen;
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
            image[i][j].rgbtRed = round(temp[i][j].redSum / temp[i][j].cnt);
            image[i][j].rgbtBlue = round(temp[i][j].greenSum / temp[i][j].cnt);
            image[i][j].rgbtGreen = round(temp[i][j].blueSum / temp[i][j].cnt);
        }
    }
}