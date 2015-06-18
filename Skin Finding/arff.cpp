#include <iostream>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <string>
#include "utils.h"
#include "math.h"

using namespace std;

// the format using this program should be:
// arff <image_input> <label> <groundtruth> <kmeans> <output_arff>

int K;	// same K used in kmeans.cpp

int main( int argc, char **argv )
{
	int width, height;
	FILE* f;
	float*** image_input;
	int** label;
	float*** groundtruth;

	if ( argc != 7)
	{
		fprintf( stderr, "arff <image_input> <label> <groundtruth> <kmeans> <output_arff>\n");
		exit( 1 );
	}
	// K = atoi(argv[1]);
	K = 12;
	image_input=ppm_mat(argv[2],		// read input image to a float image
						width,height);

	label=pgm_mat(argv[3],				// read label image to an int image
						width,height);

	groundtruth=ppm_mat(argv[4],
						width,height);	// read groundtruth 
										// image to an int image
	float k_means[K][2];
	int k = 0;	// represents which mean is being read
	float n_r, n_g;	// normalized r and g
	ifstream infile(argv[5]);

	if (infile.is_open()) {
		while (infile >> n_r >> n_g) {
			k_means[k][0] = n_r;
			k_means[k][1] = n_g;
			k++;
		}
	} else {
		cout << "Unable to open file" << endl;
	}

	int skins[K];	// votes on if each cluster is skin or non skin
	double label_histogram[K];
	// traverse over the image of label/input
	// record votes of skin in each cluster
	// increment the vote if the pixel is skin, decrement otherwise
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			label_histogram[label[i][j]]++;
			if (groundtruth[i][j][0] == 255)	// skin
				skins[label[i][j]]++;
		}
	}

	// checks the votes of each cluster
	// if votes are over 0, the cluster is skin, and set
	// the number to '1'. Otherwise, the cluster is non-skin
	// and set the number to'0'
	for (int i = 0; i < K; i++) {
		if (skins[i] / label_histogram[i] >= 0.55)
			skins[i] = 1;
		else
			skins[i] = 0;
	}

	// open the output arff file
	ofstream ofs;
	ofs.open(argv[6], ofstream::out | ofstream::app);

	// write the data to the file
	for (int i = 0; i < K; i++) {
		n_r = k_means[i][0];
		n_g = k_means[i][1];
		ofs << n_r << " " << n_g << " " <<  skins[i] << endl;
	}
}
