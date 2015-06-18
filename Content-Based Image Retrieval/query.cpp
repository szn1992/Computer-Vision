// Copyright 2014 Zhuonan Sun 12/5/2014
// HW6
// 1130849
// szn1992@cs.washington.edu

#include <utility>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <string>
#include "utils.h"
#include "math.h"
#include <vector>
#include <algorithm>

// sort the images and distances list, print the result to a new file
// the format of command should be:
// query <unordered_txt> <ordered_txt>

using namespace std;

// compare two pairs image name and its distance to the query image
bool cmp(const pair<string, float> &lhs, const pair<string, float> &rhs)
{
	return lhs.second < rhs.second;
}

int main( int argc, char **argv ) {
	if (argc != 3)
	{
        fprintf( stderr, "query <unordered_txt> <ordered_txt>\n");
        exit( 1 );
    }

	string image_name;
	float distance;
	
	// pair of image name and its distance to the query image
	pair<string, float> name_and_distance;
	
	// results containing all pairs
	vector < pair<string, float> > results;
	
	ifstream infile(argv[1]);
	
	// get the unordered results
	if (infile.is_open()) {
		while (infile >> image_name >> distance) {
			name_and_distance.first = image_name;
			name_and_distance.second = distance;
			
			results.push_back(name_and_distance);
		}
	}
	
	// sort the unordered results
	sort(results.begin(), results.end(), cmp);
	
	FILE *f = fopen(argv[2], "w");
	for (vector< pair<string, float> >::iterator i = results.begin(); i != results.end(); ++i) {
		fprintf(f, "%s %f\n", (i->first).c_str(), i->second);
	}
	
	// close the file
	fclose(f);
	
	return 0;
}