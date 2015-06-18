// Copyright 2014 Zhuonan Sun 12/5/2014
// HW6
// 1130849
// szn1992@cs.washington.edu

// format: attributes <input_image> <labeled_segmentation_img> attributes.txt

#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include "utils.h"
#include <math.h>

using namespace std;

int size_threshold = 5000;

int main( int argc, char **argv ) {
	if ( argc != 4)
	{
		fprintf( stderr, "invalid format\n");
		exit( 1 );
	}
	
	int width, height;
	float*** image_input = ppm_mat(argv[1], width, height); 

	int** labeled_seg_img = pgm_mat(argv[2], width,height);     //read image to a int image

	int regions_amount = 1;

	// get the amount of regions
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// if the label number is greater or equals to the amount of 
			// regions, update the amount of regions
			if (labeled_seg_img[i][j] >= regions_amount)
				regions_amount = labeled_seg_img[i][j] + 1;
		}
	}

	// construct a 2d array to store all attributes
	float** attributes = new float*[regions_amount];
	for (int i = 0; i < regions_amount; i++) {
		attributes[i] = new float[13];
	}

	// Convert image_input to grey
	int** greyImage = new int*[height];
	for (int i = 0; i < height; i++) {
		greyImage[i] = new int[width];
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			greyImage[i][j] = (image_input[i][j][0] + image_input[i][j][1] +
														image_input[i][j][2]) / 3;
		}
	}

	// build co-occurrence matrix
	float*** co_matrix = new float**[regions_amount];
	for (int i = 0; i < regions_amount; i++) {
		co_matrix[i] = new float*[256];
		for (int j = 0; j < 256; j++)
			co_matrix[i][j] = new float[256];
	}

	// record of total co-occurrences in each region 
	float* region_occurrence = new float[regions_amount];

	int region;		// number of region
	int r,g,b;		// r,g,b of a pixel

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// get the region number
			region = labeled_seg_img[i][j];
			
			// get r,g,b of the pixel
			r = image_input[i][j][0];
			g = image_input[i][j][1];
			b = image_input[i][j][2];
			
			attributes[region][0]++;	// update size;
			attributes[region][1] += r;	// update sum of r	
			attributes[region][2] += g;	// update sum of g
			attributes[region][3] += b;	// update sum of b
			attributes[region][7] += i;	// update sum of row
			attributes[region][8] += j;	// update sum of column

			if (i < attributes[region][9])	// update row of upper-left corner
				attributes[region][9] = i;	// of region's bounding box
			if (j < attributes[region][10])	// update column of upper-left
				attributes[region][10] = j;	// corner of region's bounding box

			if (i > attributes[region][11])	// update row of lower-right corner
				attributes[region][11] = i;	// of region's bounding box
			if (j > attributes[region][12]) // update column of lower_right
				attributes[region][12] = j;	// corner of region's bounding box
			// update co-occurrence matrix of the region
			if (i < height - 1 && j < width - 1 &&
				labeled_seg_img[i+1][j+1] == region) {
				co_matrix[region][greyImage[i][j]][greyImage[i+1][j+1]]++;
				region_occurrence[region]++;
			}

		}
	}

	// normalize co-occurrence matrix
	// and compute energy, entropy, and contrast
	for (int r = 0; r < regions_amount; r++) {
		for (int i = 0; i < 256; i++) {
			for (int j = 0; j < 256; j++) {
				co_matrix[r][i][j] /= region_occurrence[r];
				attributes[r][4] += (co_matrix[r][i][j] * co_matrix[r][i][j]);
				if (co_matrix[r][i][j] != 0.0f) {
					attributes[r][5] -= co_matrix[r][i][j] * log2(co_matrix[r][i][j]);
				}
				attributes[r][6] += (i - j) * (i - j) * co_matrix[r][i][j];
			}
		}
	}

	// change sums of attributes to means of attributes
	for (int i = 0; i < regions_amount; i++) {
		// change sum of r,g,b to mean of r,g,b
		attributes[i][1] /= attributes[i][0];	// mean r
		attributes[i][2] /= attributes[i][0];	// mean g
		attributes[i][3] /= attributes[i][0];	// mean b
		
		// change sum of row, column to mean of row, column(centroid)
		attributes[i][7] /= attributes[i][0];
		attributes[i][8] /= attributes[i][0];
	}

	// open the file to be written
	FILE* f = fopen(argv[3], "w");

	// write the data to the file
	for (int i = 0; i < regions_amount; i++) {

		// if one region's size is less than the threshold, it will not
		// be considered a major region. Ignore
		if (attributes[i][0] < size_threshold)
			continue;

		fprintf(f, "%f", attributes[i][0]);
		for (int j = 1; j < 13; j++) {
			fprintf(f, " %f", attributes[i][j]);
		}
		fprintf(f, "\n");
	}

	// close the file
	fclose(f);
	
	return 0;
}