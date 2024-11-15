/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename) 
{
	//YOUR CODE HERE
	Image* image = malloc(sizeof(Image));
	char str[3];
	int temp;
	FILE* file = fopen(filename, "r");
	fscanf(file, "%s %d %d %d", str, &image->cols, &image->rows, &temp);
	image->image = malloc(image->rows * sizeof(Color*));
	for (size_t row = 0; row != image->rows; ++row)
	{
		image->image[row] = malloc(image->cols * sizeof(Color));
		for (size_t col = 0; col != image->cols; ++col)
		{
			Color pixel;
			fscanf(file, "%d %d %d", &pixel.R, &pixel.G, &pixel.B);
			memcpy(&image->image[row][col], &pixel, sizeof(Color));
		}
	}
	fclose(file);
	return image;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
	//YOUR CODE HERE
	printf("%s\n%d %d\n%d\n", "P3", image->cols, image->rows, 255);
	for (size_t row = 0; row != image->rows; ++row)
	{
		size_t end_col = image->cols - 1;
		for (size_t col = 0; col != end_col; ++col)
		{
			Color* color = &image->image[row][col];
			printf("%3d %3d %3d", color->R, color->G, color->B);
			printf("   ");
		}
		Color* color = &image->image[row][end_col];
		printf("%3d %3d %3d", color->R, color->G, color->B);
		printf("\n");
	}
}

//Frees an image
void freeImage(Image *image)
{
	//YOUR CODE HERE
	for (size_t row = 0; row != image->rows; ++row)
	{
		free(image->image[row]);
	}
	free(image->image);
	free(image);
}