#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include "utils.h"
#include "math.h"

// format: merge <label_image> threshold

using namespace std;

int main ( int argc, char **argv )
{
   if ( argc != 3)
    {
        fprintf( stderr, "merge <label_image> threshold\n", argv[0] );
        exit( 1 );
    }

   int width, height;
   
   int** label;
   
   label = pgm_mat(argv[1], width, height);
   
   int K = 0;
   
   // get the amount of labels
   for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
         if (label[i][j] >= K)
            K = label[i][j] + 1;
      }
   }

   int* labels_counts = new int[K];
   
   // get all counts of regions
   for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
         labels_counts[label[i][j]]++;
      }
   }
  
   /*
   // test
   for (int i = 0; i < K; i++) {
	   fprintf(stderr, "%d\n", labels_counts[i]);
   }
   */

   int threshold = atoi(argv[2]);

   int region = 0;
   int** new_label = new int*[height];
   for (int i = 0; i < height; i++) {
	   new_label[i] = new int[width];
   }

   // record the label which is below threshold
   for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
		region = label[i][j];
		// ignore edge cases first
		if (i == 0 || j==0 || i== (height-1) || j==(width-1))
			continue;
			
		if (labels_counts[region] < threshold) {
			if (labels_counts[region] < 0) {	// corrected label is the absolute value
												// of the region's count
				new_label[i][j] = abs(labels_counts[region]);
				continue;
			}
			
			// if count of its neighbor region is greater threshold
			// mark the region with negative label of that region
			if (labels_counts[label[i-1][j]] >= threshold ) {			// top
				labels_counts[region] = 0 - label[i-1][j];
				new_label[i][j] = label[i-1][j];
			} else if (labels_counts[label[i][j-1]] >= threshold) {		// left
				labels_counts[region] = 0 - label[i][j-1];
				new_label[i][j] = label[i][j-1];
			} else if (labels_counts[label[i+1][j]] >= threshold) {		// bottom
				labels_counts[region] = 0 - label[i+1][j];
				new_label[i][j] = label[i+1][j];
			} else if (labels_counts[label[i][j+1]] >= threshold) {		// right
				labels_counts[region] = 0 - label[i][j+1];
				new_label[i][j] = label[i][j+1];
			}	
		}
      }
   }
   
	// solve edge cases
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			region = label[i][j];
			if (i == 0 || j == 0 || i == (height - 1) || j == (width - 1)) {
				if (labels_counts[region] < 0) {
					new_label[i][j] = abs(labels_counts[region]);
				}
			}
		}
	}

	
	K = 0;
	// test
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (new_label[i][j] >= K)
				K = new_label[i][j] + 1;
		}
	}
	fprintf(stderr, "%d \n", K);
	
	for (int i = 0; i < K; i++) {
		fprintf(stderr, "%d\n", labels_counts[i]);
	}

	mat_pgm(stdout,new_label,width,height);		//write int matrix to the input image

	delete[] labels_counts;
	for (int i = 0; i < height; i++) {
		delete[] new_label[i];
	}
	delete[] new_label;
}