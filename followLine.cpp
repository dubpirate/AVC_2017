#include <stdio.h>
#include "E101.h"

/*Returns the average values of the pixels of 3 rows
 *(one row above, and one row below the passed row int)
 *int row must be between 1 and 238
 */
int getRow(int row){
	int rowsAv[];
	int pixelTotal = 0; 	//Total value of the pixels from all 3 rows
	int average; 	    	// will hold the average value of the three pixels
	int linePlace[];	// int array of ---
	int max = 1;		// maximum value of ---
	
	for (int pixel = 0; pixel < 320; pixel += 10) { //Get's every 4th pixel
		pixelTotal = get_pixel(pixel,row-1,3) + get_pixel(pixel,row,3) + get_pixel(pixel,row+1,3);
		average = pixelTotal/3;
		rowsAv[pixelRow] = average;
		if (average > max) max = average;
	}
	
	int threshold = max / 2; //threshold of --- 
	
	for (int pixel = 0; pixel < 32; pixel++) {
		if (rowsAv[pixel] < threshold) linePlace[pixel] = 1;
	}
	
	return linePlace;
}

/*Detects where the white line is in relation to the centre of the robot.
 *returns positive integer if line is to the right
 *returns negative integer if line is to the left
 */
double detectLine(){
	//take a picture
	take_picture();
	display_picture();
	
	int[] linePlace = getRow(200);
	
	double error = 0;
	int numPixels = 0;
	for (int pixel = 0; pixel < 32; pixel++) {
		if (linePlace[pixel] == 1){
			error += (pixel-16);
			numPixels++;
		}
	}
	if (numPixels > 0) {
		return (error/numPixels);
	} else {
		return 0;
	}
}
