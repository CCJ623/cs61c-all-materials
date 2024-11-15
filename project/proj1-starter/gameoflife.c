/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
//Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat the top row as adjacent to the bottom row
//and the left column as adjacent to the right column.
Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule)
{
	//YOUR CODE HERE
	Color* neighbors[8];
	size_t neighbor_cnt = 0;
	for (int i = row - 1; i <= row + 1; ++i)
	{
		for (int j = col - 1; j <= col + 1; ++j)
		{
			if (i == row && j == col)
				continue;
			if (i < 0 || i >= image->rows || j < 0 || j >= image->cols)
				continue;
			neighbors[neighbor_cnt] = &image->image[i][j];
			++neighbor_cnt;
		}
	}
	Color* result = malloc(sizeof(Color));
	result->R = result->G = result->B = 0;
	for (size_t index = 7; index < 8; --index)
	{
		result->R <<= 1;
		result->G <<= 1;
		result->B <<= 1;
		size_t live_cnt_R = 0, live_cnt_G = 0, live_cnt_B = 0;
		for (size_t i = 0; i != neighbor_cnt; ++i)
		{
			live_cnt_R += (neighbors[i]->R >> index) & 1;
			live_cnt_G += (neighbors[i]->G >> index) & 1;
			live_cnt_B += (neighbors[i]->B >> index) & 1;
		}
		result->R += (rule >> (((image->image[row][col].R >> index) & 1) * 9 + live_cnt_R)) & 1;
		result->G += (rule >> (((image->image[row][col].G >> index) & 1) * 9 + live_cnt_G)) & 1;
		result->B += (rule >> (((image->image[row][col].B >> index) & 1) * 9 + live_cnt_B)) & 1;
	}
	return result;
}

//The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
//You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule)
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
			Color* temp = evaluateOneCell(image, row, col, rule);
			memcpy(&result->image[row][col], temp, sizeof(Color));
			free(temp);
		}
	}
	return result;
}

/*
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/
int main(int argc, char **argv)
{
	//YOUR CODE HERE
	uint32_t rule;
	if (argc == 3)
	{
		rule = strtol(argv[2], NULL, 16);
	}
	if (argc != 3 || rule < 0x00000 || 0x3FFFF < rule)
	{
		printf("usage: ./gameOfLife filename rule\n");
		printf("filename is an ASCII PPM file (type P3) with maximum value 255.\n");
		printf("rule is a hex number beginning with 0x; Life is 0x1808.");
		exit(-1);
	}
	Image* origin_image = readData(argv[1]);
	Image* new_image = life(origin_image, rule);
	writeData(new_image);
	freeImage(origin_image);
	freeImage(new_image);
}
