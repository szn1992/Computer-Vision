// Copyright 2014 Zhuonan Sun 12/5/2014
// HW6
// 1130849
// szn1992@cs.washington.edu

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <string>
#include "utils.h"
#include "math.h"
#include <set>

// the format of command should be:
// computeDist <attributes1_txt> <attributes2_txt> method <target_txt>

using namespace std;

int main( int argc, char **argv ) {
	if (argc != 5)
	{
		fprintf( stderr, "Bad format \n");
		exit( 1 );
    }

	// method to compute the distance between I1 and I2
	int method = atoi(argv[3]);

	if(method != 1 && method != 2) {
		fprintf( stderr, "method should be 1 or 2\n");
		exit( 1 );
	}

	// 13 attributes
	float size;
	float r,g,b;
	float energy,entropy,contrast;
	float c_r,c_c;
	float upperLeft_r,upperLeft_c,lowerRight_r,lowerRight_c;

	int regions_amount_1 = 0;
	int regions_amount_2 = 0;

	// open the file, count the amount of regions(lines) in two images
	ifstream infile(argv[1]);
	string line1;
	if (infile.is_open()) {
		while (getline(infile, line1))
			regions_amount_1 ++;
	} else {
		cout << "Unable to open file" << endl;
	}

	infile.close();
	infile.clear();
	infile.open(argv[2]);
	string line2;
	if (infile.is_open()) {
		while (getline(infile, line2))
			regions_amount_2 ++;
	} else {
		cout << "Unable to open file" << endl;
	}
	infile.close();
	infile.clear();


	// construct arrays of attributes for image1 and image2
	float** attributes1 = new float*[regions_amount_1];
	for (int i = 0; i < regions_amount_1; i++) {
		attributes1[i] = new float[13];
	}

	float** attributes2 = new float*[regions_amount_2];
	for (int i = 0; i < regions_amount_2; i++) {
		attributes2[i] = new float[13];
	}
	
	int region = 0;	// region
	// fill both attributes arrays
	infile.open(argv[1]);
	if (infile.is_open()) {
		while (infile >> size >> r >> g >> b >> energy >> entropy >>
				contrast >> c_r >> c_c >> upperLeft_r >> upperLeft_c >>
										lowerRight_r >> lowerRight_c) {
			attributes1[region][0] = size;
			attributes1[region][1] = r;
			attributes1[region][2] = g;
			attributes1[region][3] = b;
			attributes1[region][4] = energy;
			attributes1[region][5] = entropy;
			attributes1[region][6] = contrast;
			attributes1[region][7] = c_r;
			attributes1[region][8] = c_c;
			attributes1[region][9] = upperLeft_r;
			attributes1[region][10] = upperLeft_c;
			attributes1[region][11] = lowerRight_r;
			attributes1[region][12] = lowerRight_c;
			region++;	// next region
		}
	} else {
		cout << "Unable to open file" << endl;
	}
	
	infile.close();
	infile.clear();

	infile.open(argv[2]);
	region = 0;

	if (infile.is_open()) {
		while (infile >> size >> r >> g >> b >> energy >> entropy >>
						contrast >> c_r >> c_c >> upperLeft_r >> upperLeft_c >>
						lowerRight_r >> lowerRight_c) {
			attributes2[region][0] = size;
			attributes2[region][1] = r;
			attributes2[region][2] = g;
			attributes2[region][3] = b;
			attributes2[region][4] = energy;
			attributes2[region][5] = entropy;
			attributes2[region][6] = contrast;
			attributes2[region][7] = c_r;
			attributes2[region][8] = c_c;
			attributes2[region][9] = upperLeft_r;
			attributes2[region][10] = upperLeft_c;
			attributes2[region][11] = lowerRight_r;
			attributes2[region][12] = lowerRight_c;
			region++;	// next region
		}
	} else {
		cout << "Unable to open file" << endl;
	}

	infile.close();
	infile.clear();

	int relation_1_to_2[regions_amount_1];
	float min_diff = 1000000;
	int matched_region = 0;
	float dist_sqr = 0;
	float reldist = 0;
	float diff = 0;
	set<int> matched;
	// check every region in image 1
	for (int i = 0; i < regions_amount_1; i++) {
		// clear all variables
		min_diff = 1000000;
		matched_region = 0;
		
		for (int j = 0; j < regions_amount_2; j++) {
			dist_sqr = 0;
			if (matched.find(j) != matched.end())	// the region from image 2 has been matched
				continue;
		//	fprintf(stderr, "hello\n");
			// compute the distance
			for (int k = 0; k < 13; k++) {
				//fprintf(stderr, "%f %f %d %d\n", attributes1[i][k], attributes2[j][k], i, j);
				diff = attributes1[i][k] - attributes2[j][k];
			//	fprintf(stderr, "%f\n", dist_sqr);
				if (method == 1) {	// method 1: no weight
					dist_sqr += diff * diff;
				} else {	// method 2: with weight
					if (k == 0) {
						dist_sqr += pow(attributes1[i][k] - attributes2[j][k], 2) * 0.00001;
					}
					if (k == 1 || k == 2 || k == 3) {
						dist_sqr += pow(attributes1[i][k] - attributes2[j][k], 2) * 0.005;
					}
					if (k == 4 || k == 5 || k == 6) {
						dist_sqr += pow(attributes1[i][k] - attributes2[j][k], 2) * 0.005;
					}
					if (k >= 7 && k <= 12) {
						dist_sqr += pow(attributes1[i][k] - attributes2[j][k], 2) * 1;
					}
				}
			}
		//	fprintf(stderr, "----------------------\n");
			// find the best match based on distance
			if (sqrt(dist_sqr) < min_diff) {
				min_diff = sqrt(dist_sqr);
				matched_region = j;
			}

		}
		// fill the relation table
		// update the matched set
		relation_1_to_2[i] = matched_region;
		matched.insert(matched_region);
		
		// update relation distance
		reldist += min_diff;

	}

	// punish the relation distance if there is any unmatched region
	// in image 1 or 2
	reldist += abs(regions_amount_1 - regions_amount_2) * 100;

	// write the distance to a txt file
	ofstream ofs;
	ofs.open(argv[4], ofstream::out | ofstream::app);
	ofs << argv[2] << " " << reldist << endl;


	return 0;
}
                                            