// Copyright 2014 Zhuonan Sun 10/24/2014
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

// the format using this program should be:
// prediction <result_txt> <lines_amount> <label> <start_index> > <prediction_image>

// produces a prediction image based on the test result from WEKA
int main( int argc, char **argv )
{
	if ( argc != 5)
	{
		fprintf( stderr, "<result_txt> <lines_amount> <label> <start_index> > <prediction_image>\n");
		exit( 1 );
	}
	FILE * f;
	int** label;

	int width, height;

	label = pgm_mat(argv[3], width, height);

	int s_index = atoi(argv[4]);	// starting index on the skin_record
	int x = 0;	// position on the skin_record
	int a, b, c, d;	// varibles before "skin" in one line 
	int skin;	// if one cluster is skin
	int skin_record[atoi(argv[2])];	// record of all the clusters
	float e, pd;	// error and distribution, not used in this program
	ifstream infile(argv[1]);

	// open the file, update the skin record
	if (infile.is_open()) {
		while (infile >> a >> b >> c >> d >> skin >> e >> pd) {
			skin_record[x] = skin; 
			x++;
		}
	} else {
		cout << "Unable to open file" << endl;
	}

	int** prediction = imatrix(height, width);

	// write the prediction to each pixel
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			skin = skin_record[label[i][j] + s_index];
			if (skin == 0)			
				prediction[i][j] = 0;
			else
				prediction[i][j] = 255;
		}
	}

	mat_pgm(stdout,prediction,width,height);	//write int matrix to image

}
