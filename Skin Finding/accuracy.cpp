// Copyright 2014 Zhuonan Sun 10/31/2014
// 1130849
// szn1992@cs.washington.edu
 
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <string>
#include "utils.h"
#include "math.h"

using namespace std;

// format: accuracy <prediction_image> <groundtruth> <output>

// write the accuracy of prediction image on the output

int main( int argc, char **argv )
{
	if ( argc != 4)
	{
		fprintf( stderr, "accuracy <prediction_image> <groundtruth> <output>\n");
		exit( 1 );
	}

	int width, height;
	int** prediction;
	float*** groundtruth;
	
	// get prediction and groundtruth images
	prediction = pgm_mat(argv[1], width, height);
	groundtruth = ppm_mat(argv[2], width, height);

	double correct = 0;	// count of correct pixels

	// check every pixel of the prediction images,
	// if the pixel has the same color as groundtruth, increment
	// the count of correct pixels
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (prediction[i][j] == 0 && groundtruth[i][j][0] == 0)
				correct++;
			else if (prediction[i][j] == 255 && groundtruth[i][j][0] == 255)
				correct++;
		}
	}
	// calculate accuracy
	double accuracy = correct / (width * height); 

	ofstream ofs;
	ofs.open(argv[3], ofstream::out | ofstream::app);

	// write the data to the file
	ofs << accuracy << endl;
}
