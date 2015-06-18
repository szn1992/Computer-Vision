// Copyright 2014 Zhuonan Sun 10/24/2014
// 1130849
// szn1992@cs.washington.edu

/*
 * write your k-means code in this file.  when executed, it should
 * read in a PPM image and and write a PGMty value is its cluster label.  you might want to read in
 * other data, like the number of clusters, and you might want to 
 * output some extra information about each cluster (for example,
 * its mean color).  you could also try transforming the input image
 * to a different color space before running k-means.
 */

// format: kmeans <K> <input_image> <kmeans_txt>

#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include "utils.h"
#include "math.h"

using namespace std;

int K;

// k-means clustering algorithm
void k_means_clustering(float*** image_input, float** k_means, 
						int** label, int height, int width, 
						float** rg_sum, int* coord_count);

// check if C(k) = C(k+1)
bool final_k_means_found(float** k_means, float** k_means_prev);

int main( int argc, char **argv )
{
	int width, height;
	FILE* f;
	float*** image_input;
	int** label;
	if ( argc != 4)
	{
		fprintf( stderr, "usage: %s <input_image>\n", argv[0] );
		exit( 1 );
	}

	// K = atoi(argv[1]);
	K = 12;
	image_input=ppm_mat(argv[2],
					width,height);     //read image to a float image
	label=imatrix(height,width);
	for (int i=0;i<height;i++)
		for (int j=0;j<width;j++)
			 label[i][j]=0;


	// write your code here.  you may also add functions
	// outside of main if you wish.
	
	float**rg_sum;	// keep a record of r sum, g sum, and b sum in each cluster
	int* coord_count;		// counts the amound of coordinates in each cluster
	float** k_means;		// randomly selected K means
	float** k_means_prev;	// store the initial k means


	rg_sum = new float*[K];
	coord_count = new int[K];
	k_means = new float*[K];
	k_means_prev = new float*[K];

	for(int i = 0; i < K; i++) {
		rg_sum[i] = new float[2];
		k_means[i] = new float[2];
		k_means_prev[i] = new float[2];
	}


	float r,g,b;			// rgb of each of k means
	int means_count = 0;	// count of the means
	int row, col;			// rows and columns of the pixel in image input
	float r1, g1, b1;		// rgb of pixel of image input
	bool qualified = true;	// if the pixel is qualified to be a mean
	float n_r, n_g;	// normalized r and g used to to remove luminance
					// from the color representation

	srand (time(NULL));
	while (means_count != K) {		// method 2 **********
		qualified = true;
		row = rand() % height;
		col = rand() % width;

		r1 = image_input[row][col][0];
		g1 = image_input[row][col][1];
		b1 = image_input[row][col][2];

		if (r1 == 0 && g1 == 0 && b1 == 0) {
			n_r = 0;
			n_g = 0;
		} else {
			n_r = r1 / (r1 + g1 + b1);
			n_g = g1 / (r1 + g1 + b1);
		}

		for (int i = 0; i < means_count; i++) {
			r = k_means[i][0];
			g = k_means[i][1];

		}

		// if the the pixel is qualified, assign its rgb to be a mean
		if (qualified) {
			means_count++;
			k_means[means_count - 1][0] = n_r;
			k_means[means_count - 1][1] = n_g;
			k_means_prev[means_count - 1][0] = n_r;
			k_means_prev[means_count - 1][1] = n_g;
			rg_sum[means_count - 1][0] = n_r;
			rg_sum[means_count - 1][1] = n_g;
			coord_count[means_count - 1] = 1;
		}
	}

	// calculate new k means for the first time
	k_means_clustering(image_input, k_means, label, height, 
						width, rg_sum, coord_count);

	// while Ck(ic) != Ck(ic+1) for all k, keep updating until
	// they are equal
	while (!final_k_means_found(k_means, k_means_prev)) {
		for (int i = 0; i < K; i++) {
			k_means_prev[i][0] = k_means[i][0];
			k_means_prev[i][1] = k_means[i][1];
			k_means_prev[i][2] = k_means[i][2];
		}
		k_means_clustering(image_input, k_means, label, height,
								 width, rg_sum, coord_count);
	}

	mat_pgm(stdout,label,width,height);//write int matrix to image

	f = fopen(argv[3], "w");

	// write the data to the file
	for (int i = 0; i < K; i++) {
		n_r = k_means[i][0];
		n_g = k_means[i][1];
		fprintf(f, "%f %f\n", n_r, n_g);
	}

	// close the file
	fclose(f);
	return 0;
}

bool final_k_means_found(float** k_means, float** k_means_prev) {
	for (int i = 0; i < K; i++) {
		if(abs((k_means_prev[i][0] - k_means[i][0]) * 1000) > 1 ||
			abs((k_means_prev[i][1] - k_means[i][1]) * 1000) > 1) {
			return false;
		}
	}

	return true;
}

void k_means_clustering(float*** image_input, float** k_means, 
						int** label, int height, int width, 
						float** rg_sum, int* coord_count) {

	float r,g;			// rgb of k means
	float dist = 0;
	double min = 100000.0;	// use a large number at first in order
						// to get the real mininum
	int min_mean = 0;	// the mean which has the minimum distance to
						// the pixel of image input
	float r1,g1, b1;		// r,g,b of image_input
	float n_r, n_g;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			min = 100000;	// refresh minimum

			// rgb of the current pixel
			r1 = image_input[i][j][0];
			g1 = image_input[i][j][1];
			b1 = image_input[i][j][2];
			if (r1 == 0 && g1 == 0 && b1 == 0) {
				n_r = 0;
				n_g = 0;
			} else {
				n_r = r1 / (r1 + g1 + b1);
				n_g = g1 / (r1 + g1 + b1);
			}

			for (int k = 0; k < K; k++) {
				r = k_means[k][0];
				g = k_means[k][1];
				dist = sqrt(pow(r - n_r, 2) + pow(g - n_g, 2));
				if (dist < min) {
					min = dist;
					min_mean = k;
				}		
			}

			// increment the count of coordinates in the proper cluster
			// by one
			coord_count[min_mean]++;

			// update rgb_sum of the cluster
			rg_sum[min_mean][0] += n_r;
			rg_sum[min_mean][1] += n_g;

			// assign new rgb values to the mean
			k_means[min_mean][0] = rg_sum[min_mean][0] / coord_count[min_mean];
			k_means[min_mean][1] = rg_sum[min_mean][1] / coord_count[min_mean];

			// mark which cluster the pixel is in
			label[i][j] = min_mean;
		}
	}
}
