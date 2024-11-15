/************************************************************************
**
** NAME:        steganography.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**				Justin Yokota - Starter Code
**				YOUR NAME HERE
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This should not affect Image, and should allocate space for a new Color.
Color *evaluateOnePixel(Image *image, int row, int col)
{
	//YOUR CODE HERE
	Color* result = malloc(sizeof(Color));
	uint8_t color = image->image[row][col].B & 1;
	if (color == 1)
	{
		result->R = result->G = result->B = 255;
	}
	else
	{
		result->R = result->G = result->B = 0;

	}
	return result;
}

//Given an image, creates a new image extracting the LSB of the B channel.
Image *steganography(Image *image)
{
	//YOUR CODE HERE
	Image* result = malloc(sizeof(Image));
	result->rows = image->rows;
	result->cols = image->cols;
	result->image = malloc(result->rows * sizeof(Color*));
	for (size_t row = 0; row != result->rows; ++row)
	{
		result->image[row] = malloc(result->cols * sizeof(Color));
		for (size_t col = 0; col != result->cols; ++col)
		{
			Color* color = evaluateOnePixel(image, row, col);
			memcpy(&result->image[row][col], color, sizeof(Color));
			free(color);
		}
	}
	return result;
}

/*
Loads a file of ppm P3 format from a file, and prints to stdout (e.g. with printf) a new image, 
where each pixel is black if the LSB of the B channel is 0, 
and white if the LSB of the B channel is 1.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a file of ppm P3 format (not necessarily with .ppm file extension).
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!
*/
int main(int argc, char **argv)
{
	//YOUR CODE HERE
	if (argc != 2)
	{
		exit(-1);
	}
	Image* origin_image = readData(argv[1]);
	Image* decode_image = steganography(origin_image);
	writeData(decode_image);
	freeImage(origin_image);
	freeImage(decode_image);
}
