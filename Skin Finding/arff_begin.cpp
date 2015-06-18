#include <iostream>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <string>
#include "utils.h"
#include "math.h"

using namespace std;

int main( int argc, char **argv )
{
	fstream myfile;
	myfile.open (argv[1], std::fstream::in);
	myfile.clear();
	FILE* f;
	f = fopen(argv[1], "w");

	fprintf(f, "@relation skin\n");
	fprintf(f, "@attribute 0 numeric\n");
	fprintf(f, "@attribute 1 numeric\n");
	fprintf(f, "@attribute class {0,1}\n");
	fprintf(f, "@data\n");
	
	// close the file
	fclose(f);
}
