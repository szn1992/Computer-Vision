// Copyright 2014 Zhuonan Sun 11/15/2014
// HW5
// 1130849
// szn1992@cs.washington.edu

#include <math.h>
#include <FL/Fl.H>
#include <FL/Fl_Image.H>
#include "features.h"

float threshold = 0.0000028f;	// a threshold used to filter corner responses
							// in order to find interest points

// Compute features of an image.
bool computeFeatures(CFloatImage &image, FeatureSet &features, int featureType) {
	// Instead of calling dummyComputeFeatures, you can write your own
	// feature computation routines and call them here.
	switch (featureType) {
	case 1:
		dummyComputeFeatures(image, features);
		break;
	case 2:
		dummyComputeFeatures(image, features);
		break;
	default:
		return false;
	}

	// This is just to make sure the IDs are assigned in order, because
	// the ID gets used to index into the feature array.
	for (unsigned int i=0; i<features.size(); i++) {
		features[i].id = i+1;
	}

	return true;
}

// Perform a query on the database.  This simply runs matchFeatures on
// each image in the database, and returns the feature set of the best
// matching image.
bool performQuery(const FeatureSet &f, const ImageDatabase &db, int &bestIndex, vector<FeatureMatch> &bestMatches, double &bestScore, int matchType) {
	// Here's a nice low number.
	bestScore = -1e100;

	vector<FeatureMatch> tempMatches;
	double tempScore;

	for (unsigned int i=0; i<db.size(); i++) {
		if (!matchFeatures(f, db[i].features, tempMatches, tempScore, matchType)) {
			return false;
		}

		if (tempScore > bestScore) {
			bestIndex = i;
			bestScore = tempScore;
			bestMatches = tempMatches;
		}
	}

	return true;
}

// Match one feature set with another.
bool matchFeatures(const FeatureSet &f1, const FeatureSet &f2, vector<FeatureMatch> &matches, double &totalScore, int matchType) {
	// Instead of calling dummyMatchFeatures, you can write your own
	// feature matching routines and call them here.
	switch (matchType) {
	case 1:
		dummyMatchFeatures(f1, f2, matches, totalScore);
		return true;
	case 2:
		dummyMatchFeatures(f1, f2, matches, totalScore);
		return true;
	default:
		return false;
	}
}

// Evaluate a match using a ground truth homography.  This computes the
// average Euclidean distance between the matched feature points and
// the actual transformed positions.
double evaluateMatch(const FeatureSet &f1, const FeatureSet &f2, const vector<FeatureMatch> &matches, double h[9]) {
	double d = 0;
	int nMatches = 0;

	double xNew;
	double yNew;

	int nInliers = 0;
	float epsilon = 10; // allow matches within 10 pixels of the homography

	for (unsigned int i=0; i<f1.size(); i++) 
	{
		applyHomography(f1[i].x, f1[i].y, xNew, yNew, h);

		// Ignore unmatched points.  There might be a better way to
		// handle this.
		if (matches[i].id > 0) 
		{		
			d = sqrt(pow(xNew-f2[matches[i].id-1].x,2)+pow(yNew-f2[matches[i].id-1].y,2));
			if (d < epsilon)
				nInliers++;
			
			nMatches++;
		}
	}

	printf("nInliers = %d\n", nInliers);
	printf("nMatches = %d\n", nMatches);
	printf("Fraction of correct matches = %f\n", nInliers / (float)nMatches);

	return (nInliers / (float)nMatches);
}

//
// TODO: this is the skeleton routine for computing features
// you will need to modify ComputeHarris() and ExtractDescriptor()
//

void dummyComputeFeatures(CFloatImage &image, FeatureSet &features) 
{
	// Compute the interest function

	CShape shape0 = image.Shape();
	CShape shape  = shape0;
	shape.nBands  = 1;

	CFloatImage harris(shape);

	// TODO: write your interest point detector in ComputeHarris()
	ComputeHarris(image, harris);

	/*
	// Test: write harris function to disk
	 NormaliseImage(harris, harris);
	 CByteImage byteImage;
	 convertToByteImage(harris, byteImage);
	 WriteFile(byteImage, "harris.tga");
	*/

	int border = 22;
	int count = 0;	// count of points detected by Harris Detector

	for (int x = border; x < shape.width - border; x++)
	{
		for (int y = border; y < shape.height - border; y++)
		{
			
			bool isMax = true;
			float pix0 = harris.Pixel(x, y, 0);

			for (int dx = -1; dx <= 1; dx++)
			{
				for (int dy = -1; dy <= 1; dy++)
				{
					if (dx == 0 && dy == 0)
					{ 
						// do nothing
					}
					else
					{
						float pix = harris.Pixel(x + dx, y + dy, 0);
						if (pix >= pix0)
							isMax = false;
					}// end else
				}// end for dy
			}// end for dx
		
		if (isMax && (pix0 > threshold))
		{
			count++;
			// TODO: Write your feature descriptor in ExtractDescriptor()
			ExtractDescriptor(x, y, image, features);

		} // end if isMax
		
		}// end for y
	}// end for x

	fprintf(stderr, "Points detected: %d\n", count);
}

// Perform simple feature matching.  This just uses the Euclidean
// distance between two feature vectors, and matches a feature in the
// first image with the closest feature in the second image.  It can
// match multiple features in the first image to the same feature in
// the second image.
void dummyMatchFeatures(const FeatureSet &f1, const FeatureSet &f2, vector<FeatureMatch> &matches, double &totalScore) {
	int m = f1.size();
	int n = f2.size();

	matches.resize(m);
	totalScore = 0;

	double d;
	double dBest;
	int idBest;

	for (int i=0; i<m; i++) {
		dBest = 1e100;
		idBest = 0;

		for (int j=0; j<n; j++) {
			d = distanceEuclidean(f1[i].data, f2[j].data);

			if (d < dBest) {
				dBest = d;
				idBest = f2[j].id;
			}
		}

		matches[i].id = idBest;
		matches[i].score = exp(-dBest);
		totalScore += matches[i].score;
	}
}

// Convert Fl_Image to CFloatImage.
bool convertImage(const Fl_Image *image, CFloatImage &convertedImage) {
	if (image == NULL) {
		return false;
	}

	// Let's not handle indexed color images.
	if (image->count() != 1) {
		return false;
	}

	int w = image->w();
	int h = image->h();
	int d = image->d();

	// Get the image data.
	const char *const *data = image->data();

	int index = 0;

	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			if (d < 3) {
				// If there are fewer than 3 channels, just use the
				// first one for all colors.
				convertedImage.Pixel(x,y,0) = ((uchar) data[0][index]) / 255.0f;
				convertedImage.Pixel(x,y,1) = ((uchar) data[0][index]) / 255.0f;
				convertedImage.Pixel(x,y,2) = ((uchar) data[0][index]) / 255.0f;
			}
			else {
				// Otherwise, use the first 3.
				convertedImage.Pixel(x,y,0) = ((uchar) data[0][index]) / 255.0f;
				convertedImage.Pixel(x,y,1) = ((uchar) data[0][index+1]) / 255.0f;
				convertedImage.Pixel(x,y,2) = ((uchar) data[0][index+2]) / 255.0f;
			}

			index += d;
		}
	}

	return true;
}

// Convert CFloatImage to CByteImage.
void convertToByteImage(CFloatImage &floatImage, CByteImage &byteImage) {

	CShape sh = floatImage.Shape();
	byteImage.ReAllocate(sh);

	for (int y=0; y<sh.height; y++) {
		for (int x=0; x<sh.width; x++) {
			for (int c=0; c<sh.nBands; c++) {
				float value = floor(255*floatImage.Pixel(x,y,c) + 0.5f);

				if (value < byteImage.MinVal()) {
					value = byteImage.MinVal();
				}
				else if (value > byteImage.MaxVal()) {
					value = byteImage.MaxVal();
				}

				// We have to flip the image and reverse the color
				// channels to get it to come out right.  How silly!
				byteImage.Pixel(x,sh.height-y-1,sh.nBands - 1 - c) = (uchar) value;
			}
		}
	}
}

// Compute Euclidean distance between two vectors.
double distanceEuclidean(const vector<double> &v1, const vector<double> &v2) {
	int m = v1.size();
	int n = v2.size();

	if (m != n) {
		// Here's a big number.
		return 1e100;
	}

	double dist = 0;

	for (int i=0; i<m; i++) {
		dist += pow(v1[i]-v2[i], 2);
	}

	// I don't feel like taking the square root.
	return dist;
}

// Transform point by homography.
void applyHomography(double x, double y, double &xNew, double &yNew, double h[9]) {
	double d = h[6]*x + h[7]*y + h[8];

	xNew = (h[0]*x + h[1]*y + h[2]) / d;
	yNew = (h[3]*x + h[4]*y + h[5]) / d;
}


// Convert CFloatImage to grey
void ConvertToGrey(CFloatImage &img, CFloatImage &imgGrey) {

	CShape sh   = img.Shape();
	int nBands0 = sh.nBands;
	sh.nBands   = 1;

	imgGrey.ReAllocate(sh);
		
	for (int y=0; y<sh.height; y++) {
		for (int x=0; x<sh.width; x++) {
			float g = 0;
			for (int c=0; c<nBands0; c++) {
				g += img.Pixel(x, y, c);
			}
			imgGrey.Pixel(x, y, 0) = g / float(nBands0);
		}
	}
}


// Convolve an image with a Gaussian kernel
void ConvolveGaussian(CFloatImage &src, CFloatImage &dst, float sigma)
{
	CFloatImage kernel = GaussKernel(sigma);
	ConvolveSeparable(src, dst, kernel, kernel, 1);
}

// Return Gaussian kernel with standard deviation sigma samples
// the kernel has width 2 * (2 * sigma) + 1
CFloatImage GaussKernel(float sigma)
{
	float PI = 3.1415926538f;

	int len = int(ceil(2 * sigma));

	CFloatImage kernel((2 * len) + 1, 1, 1);

	float sumKernel = 0.0f;

	for (int i = 0; i <= len; i++)
	{
		float ki = float((1 / (sigma * sqrt(2 * PI))) * exp(-0.5 * pow((i / sigma), 2.0f)));
		kernel.Pixel(len + i, 0, 0) = ki;
		kernel.Pixel(len - i, 0, 0) = ki;
		if (i == 0)
			sumKernel += ki;
		else
			sumKernel += 2 * ki;
	}

	// Normalise
	for (int i = 0; i < (2 * len) + 1; i++)
	{
		kernel.Pixel(i, 0, 0) = kernel.Pixel(i, 0, 0) / sumKernel;
	}

	kernel.origin[0] = len;

	return kernel;
}

// 1D derivative kernels
CFloatImage dxKernel(void)
{
	CFloatImage kernel(3, 1, 1);

	kernel.Pixel(0, 0, 0) = -1.0f;
	kernel.Pixel(1, 0, 0) = 0.0f;
	kernel.Pixel(2, 0, 0) =  1.0f;

	kernel.origin[0] = 1;

	return kernel;
}

CFloatImage dyKernel(void)
{
	CFloatImage kernel(1, 3, 1);

	kernel.Pixel(0, 0, 0) = -1.0f;
	kernel.Pixel(0, 1, 0) = 0.0f;
	kernel.Pixel(0, 2, 0) =  1.0f;

	kernel.origin[1] = 1;

	return kernel;
}


// Scale image pixel values to the range 0 -> 1
void NormaliseImage(CFloatImage src, CFloatImage &dst)
{
	dst.ReAllocate(src.Shape());

	float maxVal = dst.MinVal();
	float minVal = dst.MaxVal();

	// compute max and min
	for (int i = 0; i < src.Shape().width; i++)
	{	
		for (int j = 0; j < src.Shape().height; j++)
		{
			for (int b = 0; b < src.Shape().nBands; b++)
			{
				float pix = src.Pixel(i, j, b);
				maxVal = fmax(maxVal, pix);
				minVal = fmin(minVal, pix);
			}
		}
	}

	// normalise image
	for (int i = 0; i < src.Shape().width; i++)
	{	
		for (int j = 0; j < src.Shape().height; j++)
		{
			for (int b = 0; b < src.Shape().nBands; b++)
			{
				dst.Pixel(i, j, b) = (src.Pixel(i, j, b) - minVal) / (maxVal - minVal);
			}
		}
	}

}

//
// TODO: you will need to modify the following two functions
//

// Extract a feature descriptor from img at x, y
void ExtractDescriptor(int x, int y, CFloatImage &img, FeatureSet &features)
{
	//
	// TODO: write your feature descriptor code here
	//
	Feature f;
	f.type = 1;
	f.id += 1; 
	f.x = x;
	f.y = y;

	// set the size of the feature vector
	f.data.resize(243);
	
	CShape shape = img.Shape();
	int height = shape.height;
	int width = shape.width;
	
	// construct 5 * 5 mask
	float mask[5][5] = {{0.0369f, 0.0392f, 0.04f, 0.0392f, 0.0369f},
						{0.0392f, 0.0416f, 0.0424f, 0.0416f, 0.0392f},
						{0.04f, 0.0424f, 0.0433f, 0.0424f, 0.04f},
						{0.0392f, 0.0416f, 0.0424f, 0.0416f, 0.0392f},
						{0.0369f, 0.0392f, 0.04f, 0.0392f, 0.0369f}};
	
	int m_r = 0, m_c = 0;		// column and row of the mask

	float vectors[9][9][3];		// stores 81 3d vectors

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			for (int k = 0; k < 3; k++) {
				vectors[i][j][k] = 0;
			}
		}
	}

	int v_r = 0, v_c = 0;		// column and row of vectors
	
	// calculate 81 3d vectors from individual 5 * 5 squares in
	// the whole 45 * 45 window
	for (int i = y - 22; i <= y + 17; i += 5) {
		v_c = 0;
		for (int j = x - 22; j <= x + 17; j += 5) {
			m_r = 0;	// clear the row number of mask
			for (int a = i; a < i + 5; a++) {
				m_c = 0;	// clear the column number of mask
				for (int b = j; b < j + 5; b++) {
					// concatenate a 3d vector
					vectors[v_r][v_c][0] += (img.Pixel(b,a,0) * mask[m_r][m_c]);
					vectors[v_r][v_c][1] += (img.Pixel(b,a,1) * mask[m_r][m_c]);
					vectors[v_r][v_c][2] += (img.Pixel(b,a,2) * mask[m_r][m_c]);
					m_c++;
				}
				m_r++;
			}
			v_c++;
		}
		v_r++;
	}
	
	int pos = 0;		// position of 243d vector
	float L2_norm_square = 0.0f;	// square of L2 norm
	// assign all vectors into 243d vector
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			for (int k = 0; k < 3; k++) {
				f.data[pos] = vectors[i][j][k];
				L2_norm_square += vectors[i][j][k] * vectors[i][j][k];
				pos++;
			}
		}
	}

	float L2_norm = sqrt(L2_norm_square);	// compute L2_norm
	
	// convert 243d vector to 243d normalized feature vector
	for (int i = 0; i < 243; i++) {
		f.data[i] = f.data[i] / L2_norm;
	}
	
	features.push_back(f);
}


void ComputeHarris(CFloatImage &image, CFloatImage &harris)
{
	//
	// TODO: write your harris detector code here
	//

	int height, width;	// height and width of the image
	CFloatImage dxKer = dxKernel();
	CFloatImage dyKer = dyKernel();
	
	// Convert to grey
	CFloatImage greyImage;	
	ConvertToGrey(image, greyImage);
	CShape shape = greyImage.Shape();

	// get height and width
	height = shape.height;
	width = shape.width;
	
	// Compute Harris function 
	harris.ReAllocate(shape);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			harris.Pixel(j, i, 0) = 0;
		}
	}

	// Apply 9*9 Gaussian filter to the whole image
	CFloatImage img_blurred;
	ConvolveGaussian(greyImage, img_blurred, 2.0f);
	
	// value of the center pixel, and values of 4 other pixel surrounding it
	float center, left, right, top, bottom;
	float Ix, Iy;	// Ix and Iy of one point
	float sum_Ix_square, sum_Iy_square, sum_IxIy;
	float det, tr, k = 0.05f;	// variables used to compute corner response
	float R;	// corner response

	// For each point in the image, compute the Harris Matrix M for that point
	for (int y = 2; y < height - 2; y++) {
		for (int x = 2; x < width - 2; x++) {
			// clears all the sums
			sum_Ix_square = 0;
			sum_Iy_square = 0;
			sum_IxIy = 0;

			// compute Harris Matrix M in a 5*5 window W around point(x, y)
			for (int i = y - 2; i < y + 2; i++) {
				for (int j = x - 2; j < x + 2; j++) {
					// get center, and its surroundings
					center = img_blurred.Pixel(j, i, 0);
					left = (j - 1 == -1) ? 0 : img_blurred.Pixel(j - 1, i, 0);
					right = (j + 1 == width) ? 0 : img_blurred.Pixel(j + 1, i, 0);
					top = (i - 1 == -1) ? 0 : img_blurred.Pixel(j, i - 1, 0);
					bottom = (i + 1 == height) ? 0 : img_blurred.Pixel(j, i + 1, 0);
					
					// get Ix and Iy
					Ix = left * dxKer.Pixel(0, 0, 0) + center * 
						dxKer.Pixel(1, 0, 0) + right * dxKer.Pixel(2, 0, 0);
					Iy = top * dyKer.Pixel(0, 0, 0) + center *
						dyKer.Pixel(0, 1, 0) + bottom * dyKer.Pixel(0, 2, 0);

					sum_Ix_square += (Ix * Ix);
					sum_Iy_square += (Iy * Iy);
					sum_IxIy += (Ix * Iy);
				}
			}

			// get det and tr
			det = sum_Ix_square * sum_Iy_square - sum_IxIy * sum_IxIy;
			tr = sum_Ix_square + sum_Iy_square;
			
			// finally compute R
			R = det - k * tr * tr;
			harris.Pixel(x, y, 0) = R;
		}
	}
}