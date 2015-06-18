/* erode [d|b] [size] [input_image]
* Erodes the binary image using a disc or block structuring element with given radius.
* Result is produced in stdout. To save in an image file use " erode [d|b] [size] [input_image] > [output_image] "
*/

#include "utils.h"
#include "math.h"

int main(int argc, char **argv)
{
	int width, height;
	int** image_input;
	int** image_output;
	int** structure_element;
	int sel_size,sel_type;


	if ( argc != 4 )
	{
	        fprintf( stderr, "usage: %s [d|b] [size] [input_image]\n", argv[0] );
		exit( 1 );
	}

	// Parsing arguments
	switch ( argv[1][0] )
	{
	case 'b':
	case 'B': sel_type = 1; break;
	case 'd':
	case 'D': sel_type = 2; break;
	default: fprintf( stderr, "b|d argument must be b or d.\n" ); exit( 1 );
	}
	sel_size = atoi( argv[2] );

	// Read image into a matrix 
	image_input = pgm_mat( argv[3], width, height); 

	// Create empty output image
	image_output = imatrix( height, width);

	// Create structuring element here 
	
	/**************************************************
	**
	**			YOUR CODE GOES HERE
	**
	**	You can use the variable structure_element 
	**	or declare another variable of your choice.
	**	Use sel_type variable for checking block (1) 
	**	vs disc (2). You are only responsible disc 
	**	structuring element.
	**						
	**************************************************/

        structure_element = imatrix(sel_size * 2 + 1, sel_size * 2 + 1);

        if (sel_type == 2) {
	  for (int i = 0; i <= sel_size * 2; i++) {
            for (int j = 0; j <= sel_size * 2; j++) {
              // fprintf(stderr, "i: %d j: %d \n", i, j);
              if ((pow (i - sel_size, 2) + pow (j - sel_size, 2)) > 
                                                     pow (sel_size, 2)) {
                structure_element[i][j] = 0;
              } else {
                structure_element[i][j] = 1;
              }
            }
          }
        } else {
          fprintf( stderr, "Block is not implemented yet.\n" );
        }

	// Erode

	/**************************************************
	**
	**			YOUR CODE GOES HERE
	**
	**	Use image_input and structure_element 
	**	variables to fill image_output variable.
	**	You can fill the edges and corners of the
	**	output image (where structuring_element won't 
	**	fit) with zeros.
	**						
	**************************************************/

        int x, y;  // coordinate of output image
        int a, b;  // coordinate of structure element
        bool fail;  // fail to erode one pixel
        // traverse all pixels in input image
        for (int i = 0; i < height; i++) {
          for (int j = 0; j < width; j++) {
	    if (i >= sel_size && i < height - sel_size && j >= sel_size && j < width - sel_size) {
	      // possibly qualified pixel found
	      // start from the upper left of structure element
	      a = 0;
	      b = 0;
	      fail = false;
	      // check if the surroundings of the pixel are qualified of
	      // erosion
	      for (y = i - sel_size; y <= i + sel_size; y++) {
		for (x = j - sel_size; x <= j + sel_size; x++) {
		  if (structure_element[a][b] == 0) {
		    b++;
		    continue;
		  }
		  if (image_input[y][x] != 255) {
		    // surrounding pixels are unqualified
		    fail = true;
		    break;
		  }
		  b++;
		}
		if (fail)
		  break;
		a++;
	      }
	      if (!fail) {
		// erosion successful
		image_output[i][j] = 255;
	      } else {
		image_output[i][j] = 0;
	      }
	    } else {
	      image_output[i][j] = 0;
	    }
	  }
	}

	// Write matrix to stdout
	mat_pgm( stdout, image_output, width, height);
	return 0; // Success
}
