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

#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include "utils.h"
#include "math.h"
#include <vector>
#include <map>
#include <assert.h> 

using namespace std;

int K = 6;
double fraction_threshold = 0.20;	// threshold of percentage of npeak/Kpeak
									// in terms of pixels, used to find the best
									// K in the smarter algorithm
double dist_threshold;

// k-means clustering algorithm
void k_means_clustering(float*** image_input, float** k_means, 
						int** label, int height, int width, 
						float** rgb_sum, int* coord_count);

// check if C(k) = C(k+1)
bool final_k_means_found(float** k_means, float** k_means_prev);

// check if the rgb is a peak in histogram
bool is_real_peak(int*** rgb_histogram, int r, int g, int b, int max);

int main( int argc, char **argv )
{
	int width, height;
	FILE* f;
	float*** image_input;
	int** label;
	if ( argc != 3 && argc != 4)
	{
		fprintf( stderr, "usage: %s <input_image>\n", argv[0] );
		exit( 1 );
	}

	image_input=ppm_mat(argv[2],
						width,height);     //read image to a float image
	label=imatrix(height,width);
	for (int i=0;i<height;i++)
		for (int j=0;j<width;j++)
			 label[i][j]=0;


	// write your code here.  you may also add functions
	// outside of main if you wish.

	int method;				// which method is chosen

	switch ( argv[1][0] )
	{
	case '1': method = 1; break;	// randomly select K means
	case '2': method = 2; break;	// find seeds from image pixels
	case '3': method = 3; break;	// find seeds from peaks of rgb histogram
	case '4': method = 4; break;	// smarter algorithm
	default: fprintf( stderr, "1|2|3|4 argument must be an int from 1-4.\n");
			 exit(1);
	}

	if (method == 4 || method == 3) {
	 	if (argc == 4) {
			dist_threshold = atoi(argv[3]);	// a range used to 
											// check if the pixel is a peak	
		} else {
			fprintf( stderr, "add dist_threshold to the end of the arguments\n");
		}
	}

	
	float**rgb_sum;	// keep a record of r sum, g sum, and b sum in each cluster
	int* coord_count;		// counts the amound of coordinates in each cluster
	float** k_means;		// randomly selected K means
	float** k_means_prev;	// store the initial k means


	if (method != 4) {	// if method 4 is not chosen, 
						// assign these variable as normal
		rgb_sum = new float*[K];
		coord_count = new int[K];
		k_means = new float*[K];
		k_means_prev = new float*[K];

		for(int i = 0; i < K; i++) {
			rgb_sum[i] = new float[3];
			k_means[i] = new float[3];
			k_means_prev[i] = new float[3];
		}
	}


	float r,g,b;			// rgb of each of k means
	int means_count = 0;	// count of the means
	int row, col;			// rows and columns of the pixel in image input
	float r1, g1, b1;		// rgb of pixel of image input
	bool qualified = true;	// if the pixel is qualified to be a mean


	srand (time(NULL));

	if (method == 1) {	// =============== method 1 ======================
		for (int i = 0; i < K; i++) {
			r = rand() % 256;
			g = rand() % 256;
			b = rand() % 256;
			k_means[i][0] = r;
			k_means[i][1] = g;
			k_means[i][2] = b;
			k_means_prev[i][0] = r;
			k_means_prev[i][1] = g;
			k_means_prev[i][2] = b;
			rgb_sum[i][0] = r;
			rgb_sum[i][1] = g;
			rgb_sum[i][2] = b;
			coord_count[i] = 1;
		}	

	} else if (method == 2) {	// =============== method 2 ======================
		while (means_count != K) {
			qualified = true;
			row = rand() % height;
			col = rand() % width;

			r1 = image_input[row][col][0];
			g1 = image_input[row][col][1];
			b1 = image_input[row][col][2];
		
			for (int i = 0; i < means_count; i++) {
				r = k_means[i][0];
				g = k_means[i][1];
				b = k_means[i][2];

				// the pixel has the same rgb as one of the means
				// unqualified. will not increment the count of the means
				if (abs(r - r1) < 0.000001 && 
					abs(g - g1) < 0.000001 && abs(b - b1) < 0.000001) {
					qualified = false;
					break;
				}
			}

			// if the the pixel is qualified, assign its rgb to be a mean
			if (qualified) {
				means_count++;
				k_means[means_count - 1][0] = r1;
				k_means[means_count - 1][1] = g1;
				k_means[means_count - 1][2] = b1;
				k_means_prev[means_count - 1][0] = r1;
				k_means_prev[means_count - 1][1] = g1;
				k_means_prev[means_count - 1][2] = b1;
				rgb_sum[means_count - 1][0] = r1;
				rgb_sum[means_count - 1][1] = g1;
				rgb_sum[means_count - 1][2] = b1;
				coord_count[means_count - 1] = 1;
			}
		}

	} else  {	// ===================method 3 and 4 ======================

		// histogram of rgb of the input image
		int*** rgb_histogram = new int**[256];

		int k_means_votes[11];	// specifically to count votes of 
								// each temporarily qualified mean

		//build rgb_histogram
		for (int i = 0; i < 256; i++) {
			rgb_histogram[i] = new int*[256];
			for (int j = 0; j < 256; j++) {
				rgb_histogram[i][j] = new int[256];
			}
		}
			
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				r1 = image_input[i][j][0];
				g1 = image_input[i][j][1];
				b1 = image_input[i][j][2];
				rgb_histogram[((int)r1)][((int)g1)][((int)b1)]++;
			}
		}
	
		if (method == 3) {	// ========== method 3 ========================================
			int peaks_record[30000][3];
			int peak_no = 0;
			for (int i = 0; i < 256; i++) {
				for (int j = 0; j < 256; j++) {
					for (int k = 0; k < 256; k++) {
						if (rgb_histogram[i][j][k] != 0) {
							if (is_real_peak(rgb_histogram, 
								i, j, k, rgb_histogram[i][j][k])) {

								peaks_record[peak_no][0] = i;
								peaks_record[peak_no][1] = j;
								peaks_record[peak_no][2] = k;
								peak_no++;
							}
						}
					}
				}		
			}

			int local_peak;
			for (int i = 0; i < K; i++) {
				local_peak = rand() % peak_no;
				r = peaks_record[local_peak][0];
				g = peaks_record[local_peak][1];
				b = peaks_record[local_peak][2];
				k_means[i][0] = r;
				k_means[i][1] = g;
				k_means[i][2] = b;
				k_means_prev[i][0] = r;
				k_means_prev[i][1] = g;
				k_means_prev[i][2] = b;
				rgb_sum[i][0] = r;
				rgb_sum[i][1] = g;
				rgb_sum[i][2] = b;
				coord_count[i] = 1;
			}

		} else {	// ===================== method 4 ======================
			// find the K-Max best peaks
			K = 11;
			
			k_means_prev = new float*[K];	// stores K-Max means
			for(int i = 0; i < K; i++) {
				k_means_prev[i] = new float[3];
			}

			int max = 0;

			// find the first peak
			for (int i = 0; i < 256; i++) {
				for (int j = 0; j < 256; j++) {
					for (int k = 0; k < 256; k++) {
						if (rgb_histogram[i][j][k] > max) {
							max = rgb_histogram[i][j][k];
							r = i;
							g = j;
							b = k;
						}
					}
				}
			}

			k_means_prev[0][0] = r;
			k_means_prev[0][1] = g;
			k_means_prev[0][2] = b;
			means_count = 1;

			int bigger_max;
			int sum_k_means_votes = max;	// sum of K max means' votes
			fprintf(stderr, "%d \n", max);
			// find the rest K-max - 1 peaks
			while (means_count != K) {
				bigger_max = max;
				max = 0;
				for (int i = 0; i < 256; i++) {
					for (int j = 0; j < 256; j++) {
						for (int k = 0; k < 256; k++) {
							if (rgb_histogram[i][j][k] > max &&
								rgb_histogram[i][j][k] < bigger_max) {
								if (is_real_peak(rgb_histogram, i, j, k,
									 			rgb_histogram[i][j][k])) {

									max = rgb_histogram[i][j][k];
									r = i;
									g = j;
									b = k;
								}
							}
						}
					}
				}
				sum_k_means_votes += max;
				fprintf(stderr, "%d \n", max);
				k_means_votes[means_count] = max;
				k_means_prev[means_count][0] = r;
				k_means_prev[means_count][1] = g;
				k_means_prev[means_count][2] = b;
				means_count++;
			}

			int num_qualified = 0;	// number of qualified means
	
			// sum of the first n peaks' votes, n = 0 at first
			int sum_n_peaks_votes = 0;

			// evaluate first n of K means, if the sum of them are greater
			// than the fraction threshold, first n of k means become the
			// new k means 
			for (int i = 0; i < K; i++) {
				sum_n_peaks_votes += k_means_votes[i];
				if ((sum_n_peaks_votes / ((double)sum_k_means_votes)) > 
													fraction_threshold) {
					num_qualified = i + 1;
					break;
				}
			}

			// build k means based on the evaluation result
			k_means = new float*[num_qualified];
			for(int i = 0; i < num_qualified; i++)
				k_means[i] = new float[3];

			for (int i = 0; i < num_qualified; i++) {
				k_means[i][0] = k_means_prev[i][0];
				k_means[i][1] = k_means_prev[i][1];
				k_means[i][2] = k_means_prev[i][2];
			}

			k_means_prev = new float*[num_qualified];
			rgb_sum = new float*[num_qualified];
			for(int i = 0; i < num_qualified; i++) {
				k_means_prev[i] = new float[3];
				rgb_sum[i] = new float[3];
			}

			coord_count = new int[num_qualified];

			// update rgb_sum and k_means_prev as well
			for (int i = 0; i < num_qualified; i++) {
				k_means_prev[i][0] = k_means[i][0];
				k_means_prev[i][1] = k_means[i][1];
				k_means_prev[i][2] = k_means[i][2];
				rgb_sum[i][0] = k_means[i][0];
				rgb_sum[i][1] = k_means[i][1];
				rgb_sum[i][2] = k_means[i][2];
				coord_count[i]++;
			}

			// update value of K, instead of use K-max
			K = num_qualified;
			fprintf(stderr, "%d \n", K);
		}
	}

	// calculate new k means for the first time
	k_means_clustering(image_input, k_means, label, height,
							 width, rgb_sum, coord_count);

	// while Ck(ic) != Ck(ic+1) for all k, keep updating until
	// they are equal
	while (!final_k_means_found(k_means, k_means_prev)) {
		for (int i = 0; i < K; i++) {
			k_means_prev[i][0] = k_means[i][0];
			k_means_prev[i][1] = k_means[i][1];
			k_means_prev[i][2] = k_means[i][2];
		}

		k_means_clustering(image_input, k_means, label, height,
								 width, rgb_sum, coord_count);
	}

	mat_pgm(stdout,label,width,height);//write int matrix to image
	return 0;
}


bool is_real_peak(int*** rgb_histogram, int r, int g, int b, int max) {
	int r_lbound, g_lbound, b_lbound;
	int r_hbound, g_hbound, b_hbound;

	if (r - dist_threshold < 0)
		r_lbound = 0;
	else
		r_lbound = r - dist_threshold;

	if (r + dist_threshold > 255)
		r_hbound = 255;
	else
		r_hbound = r + dist_threshold;
	
	if (g - dist_threshold < 0)
		g_lbound = 0;
	else
		g_lbound = g - dist_threshold;

	if (g + dist_threshold > 255)
		g_hbound = 255;
	else
		g_hbound = g + dist_threshold;

	if (b - dist_threshold < 0)
		b_lbound = 0;
	else
		b_lbound = b - dist_threshold;

	if (b + dist_threshold > 255)
		b_hbound = 255;
	else
		b_hbound = b + dist_threshold;

	for (int i = r_lbound; i < r_hbound; i++) {
		for (int j = g_lbound; j < g_hbound; j++) {
			for (int k = b_lbound; k < b_hbound; k++) {
				if (i == r && j == g && k == b)
					continue;
				if (max <= rgb_histogram[i][j][k]) {		
					return false;
				}
			}
		}
	}
	return true;
}

bool final_k_means_found(float** k_means, float** k_means_prev) {
	for (int i = 0; i < K; i++) {
		if(abs(k_means_prev[i][0] - k_means[i][0]) > 0.0000001 ||
			abs(k_means_prev[i][1] - k_means[i][1]) > 0.0000001 ||
			abs(k_means_prev[i][2] - k_means[i][2]) > 0.0000001) {
			return false;
		}
	}

	return true;
}

void k_means_clustering(float*** image_input, float** k_means, 
						int** label, int height, int width, 
						float** rgb_sum, int* coord_count) {

	float r,g,b;		// rgb of k means
	float dist = 0;
	int min = 10000;	// use a large number at first in order
						// to get the real mininum
	int min_mean = 0;	// the mean which has the minimum distance to
						// the pixel of image input
	float r1,g1,b1;		// r,g,b of image_input
	
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			min = 100000;	// refresh minimum

			// rgb of the current pixel
			r1 = image_input[i][j][0];
			g1 = image_input[i][j][1];
			b1 = image_input[i][j][2];
			for (int k = 0; k < K; k++) {
				r = k_means[k][0];
				g = k_means[k][1];
				b = k_means[k][2];
				dist = sqrt(pow(r-r1,2) + pow(g-g1,2) + pow(b-b1,2));
				if (dist < min) {
					min = dist;
					min_mean = k;
				}
			}

			// increment the count of coordinates in the proper cluster
			// by one
			coord_count[min_mean]++;

			// update rgb_sum of the cluster
			rgb_sum[min_mean][0] += r1;
			rgb_sum[min_mean][1] += g1;
			rgb_sum[min_mean][2] += b1;

			// assign new rgb values to the mean
			k_means[min_mean][0] = rgb_sum[min_mean][0]/coord_count[min_mean];
			k_means[min_mean][1] = rgb_sum[min_mean][1]/coord_count[min_mean];
			k_means[min_mean][2] = rgb_sum[min_mean][2]/coord_count[min_mean];

			// mark which cluster the pixel is in
			label[i][j] = min_mean;
		}
	}
}
