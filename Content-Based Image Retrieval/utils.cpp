#include "utils.h"

#define I(x,y,c)   (input_image[3*((y)*(width)+(x))+(c)])
#define O(x,y)     (output_image[(y)*(width)+(x)])
#define I_gray(x,y)   (input_image[(y)*(width)+(x)])

/*****************************************************************************
*      Convert a pixel from HSV colorspace to RGB colorspace
*****************************************************************************/
void hsv_to_rgb( double h, double s, double v,
				unsigned char* r, unsigned char* g, unsigned char* b )
{
	int i;
	double f;
	unsigned char p, q, t, z;

	if ( s == 0 )
	{
		*r = *g = *b = (unsigned char) v*255;
		return;
	}

	h /= (M_PI/3);

	i = (int) h;
	f = h - (double) i;

	p = 255 * (unsigned char) v * ( 1 - (unsigned char) s);
	q = 255 * (unsigned char) v * ( 1 - (unsigned char) s* (unsigned char)f );
	t = 255 * (unsigned char) v * ( 1 - (unsigned char) s * (1- (unsigned char) f) );
	z = 255 * (unsigned char) v;

	*r = (i==0||i==5) ? z : ( (i==2||i==3) ? p : ( i==1 ? q : t ) );
	*g = (i==1||i==2) ? z : ( (i==4||i==5) ? p : ( i==3 ? q : t ) );
	*b = (i==3||i==4) ? z : ( (i==0||i==1) ? p : ( i==5 ? q : t ) );
}


/*****************************************************************************
*      Read PGM image into a unsigned char array
*****************************************************************************/
unsigned char* read_pgm( char* filename, int* width, int* height )
{
	char buffer[80];
	int phase = 0;
	int type, maxval;
	int c;
	unsigned char* data;
	FILE* f;

	if ( strcmp( filename, "-" ) == 0 )
		f = stdin;
	else
	{
		f = fopen( filename, "rb" );
		if ( f == NULL )
			return NULL;
	}

	while ( phase < 4 )
	{
		fgets( buffer, 80, f );
		if ( buffer[0] == '#' ) 
			continue;
		switch( phase )
		{
		case 0: c = sscanf( buffer, "P%d %d %d %d\n", &type, width, height, &maxval ); break;
		case 1: c = sscanf( buffer, "%d %d %d\n", width, height, &maxval ); break;
		case 2: c = sscanf( buffer, "%d %d\n", height, &maxval ); break;
		case 3: c = sscanf( buffer, "%d\n", &maxval ); break;
		case 4: c = 0; break;
		}
		phase += c;
	}

	if ( type != 5 )
	{
		fprintf( stderr, "bad input image format.\n" );
		return NULL;
	}

	data = ( unsigned char* )malloc ( *width * *height );
	fread( data, *width * *height, 1, f );

	fclose( f );

	return data;
}


/*****************************************************************************
*      Write unsigned char array data to PPM image
*****************************************************************************/
void write_ppm( FILE* f, int width, int height, unsigned char* data )
{
	fprintf( f, "P6\n%d %d\n%d\n", width, height, 255 );
	fwrite( data, width*height*3, 1, f );
	fclose( f );
}

/*****************************************************************************
*      Write unsigned char array data to PGM image
*****************************************************************************/
void write_pgm( FILE* f, int width, int height, unsigned char* data )
{
	fprintf( f, "P5\n%d %d\n%d\n", width, height, 255 );
	fwrite( data, width*height, 1, f );
	fclose( f );
}

/*****************************************************************************
*      Write integer matrix to PGM image
*****************************************************************************/
void mat_pgm( FILE* f,int** mat,int width, int height )
{
	unsigned char* output;
	output = new unsigned char [ width*height ];
	for ( int i = 0; i < width; i++ )
		for ( int j = 0; j < height; j++ )
			output[ j*width +i ] = ( unsigned char ) mat[j][i];
	write_pgm( f, width, height, output );
}


/*****************************************************************************
*      Read PGM image into integer matrix
*****************************************************************************/
int** pgm_mat( char* filename, int& width, int& height )
{
	unsigned char* input_image;
	int** input_image_int;

	input_image = read_pgm( filename, &width, &height );
	input_image_int = new int* [ height ];
	for ( int i = 0; i < height; i++ )
	{
		input_image_int[i] = new int[ width ];
		for ( int j = 0; j < width; j++ )
			input_image_int[i][j] = I_gray( j,i );
	}
	return input_image_int;
}

/*****************************************************************************
*      Allocate memory for integer matrix
*****************************************************************************/
int** imatrix( int row, int col )
{
	int **m;

	m = new int* [ row ];
	for ( int i = 0; i < row; i++ )
		m[i] = new int[ col ];
	return m;
}


/*****************************************************************************
 *      Read pgm image into float matrix.
 *****************************************************************************/
float*** ppm_mat( char* filename, int& width, int& height )
{
	 int i,j,k;
	 FILE* f;
	 unsigned char* input_image;
	 float*** input_image_float;

	 input_image=read_ppm(filename,&width,&height);
	 input_image_float=new float** [height];
	 for (i=0;i<height;i++)
	 {
		 input_image_float[i]=new float* [width];
		 for (j=0;j<width;j++)
		 {
		    input_image_float[i][j]=new float[3];
			input_image_float[i][j][0]=(float)I(j,i,0);
			input_image_float[i][j][1]=(float)I(j,i,1);
			input_image_float[i][j][2]=(float)I(j,i,2);
		 }
	 }
	 return input_image_float;
}


unsigned char* read_ppm( char* filename, int* width, int* height )
{
     char buffer[80];
     int phase = 0;
     int type, maxval;
     int j;
     unsigned char* data;
     FILE* f;

     if ( strcmp( filename, "-" ) == 0 )
     {
	  f = stdin;
     }
     else
     {
	  f = fopen( filename, "rb" );
	  if ( f == NULL )
	       return NULL;
     }
     
     while ( phase < 4 )
     {
	  fgets( buffer, 80, f );
	  if ( buffer[0] == '#' ) continue;
	  switch( phase )
	  {
	     case 0: j = sscanf( buffer, "P%d %d %d %d\n", &type, width, height, &maxval ); break;
	     case 1: j = sscanf( buffer, "%d %d %d\n", width, height, &maxval ); break;
	     case 2: j = sscanf( buffer, "%d %d\n", height, &maxval ); break;
	     case 3: j = sscanf( buffer, "%d\n", &maxval ); break;
	     case 4: j = 0; break;
	  }
	  phase += j;
     }

     if ( type != 6 )
     {
	  fprintf( stderr, "bad input image format.\n" );
	  return NULL;
     }

     data = (unsigned char*)malloc ( *width * *height * 3 );
     fread( data, *width * *height * 3, 1, f );

     fclose( f );
     
     return data;
}