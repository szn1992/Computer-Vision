// Copyright 2014 Zhuonan Sun CSE455 10/14/2014
// 1130849
// szn1992@cs.washington.edu

/* findcirc [input_image] [radius]
* Reads input image and finds circles with the given radius. Puts a red dot on the center of found circles.
* Result is produced in stdout. To save in an image file use "findcirc [input_image] [radius] > [output_image]"
*/

#include "utils.h"
#include <vector>
#include "math.h"


#define pi 3.14159265359
#define pi2 1.5707963268

using namespace std;

#define I(x,y)   (input_image[(y)*(width)+(x)])
#define O(x,y)   (output_image[(y)*(width)+(x)])

struct point
{
	int r;
	int c;
};

// My code **********************************
int clus_threshold = 23;	// threshold used in clustering algorithm
int gmag_threshold = 300;	// threshold used to compare with gmag
// ******************************************

void drawcenters(FILE* f,vector <struct point> centers,float** mat,int width, int height);

int main(int argc, char **argv)
{
	float** image ;		/* image array */
	int** accumu ;		/* accumulator array */
	int	width, height;	/* row and column of images */
	float r;                      /* radius trials */
	int center_threshold;		/* threshold to pick highest values from accumulator */
	vector <struct point> centers;
	struct point pts; 

	if (argc < 3)
	{
		fprintf( stderr, "usage: %s [input_image] [radius] [center_threshold] > [output_image]\n", argv[0] );
		exit(0) ;
	}

	// Parsing arguments
	r = (float) atof( argv[2] );
	center_threshold = atoi( argv[3]);
	// Read image into a matrix
	image = pgm_mat( argv[1], width, height);

	// Allocate and initialize accumulators 
	accumu = imatrix(height,width);

	for ( int i = 0; i < height; i++)	{
		for ( int j = 0; j < width; j++)
			accumu[i][j] = 0;
	}

	/**************************************************
	**
	**			YOUR CODE GOES HERE
	**
	**	Find the center candidates from accumulation
	**	matrix. Cluster center candidates. You can use 
	**	accumu, centers and pts variables.
	**	
	**						
	**************************************************/
	// build Sobel operator
	int** xmask = imatrix(3, 3);
    xmask[0][0] = -1;
    xmask[0][1] = 0;
	xmask[0][2] = 1;
	xmask[1][0] = -2;
	xmask[1][1] = 0;
	xmask[1][2] = 2;
	xmask[2][0] = -1;
	xmask[2][1] = 0;
	xmask[2][2] = 1;

	int** ymask = imatrix(3, 3);
	ymask[0][0] = 1;
    ymask[0][1] = 2;
	ymask[0][2] = 1;
	ymask[1][0] = 0;
	ymask[1][1] = 0;
	ymask[1][2] = 0;
	ymask[2][0] = -1;
	ymask[2][1] = -2;
	ymask[2][2] = -1;

	// gradient X, gradient Y, and gradient magnitude
	float gx, gy;
	float gmag;
	float theta;	// gradient angle
	int r0, c0;	// center of the circle
	int a, b;	// row and column of the mask

	// traverse every pixel of the image, except the edges and corners
	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			gx = 0;
			gy = 0;

			// calculate Gx and Gy at current pixel			
			for (a = 0; a < 3; a++)	{
				for (b = 0; b < 3; b++) {
					gx += (image[i + a - 1][j + b - 1] * xmask[a][b]);
					gy += (image[i + a - 1][j + b - 1] * ymask[a][b]);
				}
			}

			// calculate G magnitude
			gmag = sqrt(gx * gx + gy * gy);

			// if G magnitude is greater than threshold,
			// calculate G angle
			if (gmag > gmag_threshold) {
				theta = atan2(gy, gx);
			
				// center of the circle
				r0 = i - r * sin(theta);
				c0 = j + r * cos(theta);

				// accumulate votes for the center of the circle
				if (r0 >= 0 && r0 < height && c0 >= 0 && c0 < width)
					accumu[r0][c0]++;
			
			}
		}
	}

	// traverse the accumulator
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// if the pixel has higher votes than center threshold
			// clear the votes of its adjacent bins in order to
			// get just one center for each circle
			if (accumu[i][j] > center_threshold) {
				for (a = i - clus_threshold; a < i + clus_threshold; a++) {
					for (b = j - clus_threshold; b < j + clus_threshold; b++) {
						if(a >= 0 && a < height && b >= 0 && b < width) {
							accumu[a][b] = 0;
						}
					}
				}
				pts.r = i;
				pts.c = j;
				centers.push_back(pts);
			}
		}
	}
	// Draw the centers of circles on top of input image
	// and output the image to stdout
	drawcenters( stdout, centers, image, width,height);

	// my code *********************
	// delete xmask and ymask
	for (int i = 0; i < 3; i++) {
		delete[] xmask[i];
		delete[] ymask[i];	
	}
	delete xmask;
	delete ymask;
}


/*****************************************************************************
*      Draw the centers of circles on top of input image
*****************************************************************************/
void drawcenters (FILE* f, vector <struct point> centers, float** mat, int width, int height)
{
	unsigned char* output;
	output = new unsigned char [3*width*height];
	for ( int i = 0; i < width; i++)
		for ( int j = 0; j < height; j++)
			for ( int k = 0; k < 3; k++)
				output [ 3 * (j*width + i) + k ] = (unsigned char) mat[j][i];

	for ( int i = 0; i < (int) centers.size(); i++)
	{
		for ( int j = 0; j < 5; j++)
		{
			output [ 3* ( (centers[i].r-2 + j) * width + centers[i].c ) + 0] = 255;
			output [ 3* ( (centers[i].r-2 + j) * width + centers[i].c ) + 1] = 0;
			output [ 3* ( (centers[i].r-2 + j) * width + centers[i].c ) + 2] = 0;
		}
		for ( int k = 0; k < 5; k++)
		{
			output [ 3* ( centers[i].r * width + centers[i].c-2 + k) + 0] = 255;
			output [ 3* ( centers[i].r * width + centers[i].c-2 + k) + 1] = 0;
			output [ 3* ( centers[i].r * width + centers[i].c-2 + k) + 2] = 0;
		}

	}
	write_ppm ( f, width, height, output);
}
