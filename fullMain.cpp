#include <stdio.h>
#include "E101.h"
#include <stdlib.h>

int numPixels = 0;
int linePlace[320] = {0};

void setMotors(int offSet){
	int left_motor = 1;
	int right_motor = 2;
	int speedFact = 40;
	//double scaleFact = 1.0;
	//offSet = abs(offSet);
	double prop = 0.002;//((double)offSet/136);
	int turnFact = offSet * prop;
	if (turnFact > (255-speedFact)) {
		turnFact = (255 - speedFact);
	}

	printf("%d\n", offSet);
	if (offSet > 0) { //Line is to the right
		set_motor(left_motor, -1*(speedFact + (int)(offSet * prop)));
		set_motor(right_motor, speedFact - (int)(offSet * prop));
	}else if (offSet < 0) { //Line is to the left
		set_motor(left_motor, -1*(speedFact + (int)(offSet * prop)));
		set_motor(right_motor,(speedFact - (int)(offSet * prop)));
	}else if (offSet == 0) {
		if (numPixels == 0) {
			set_motor(left_motor, speedFact);
			set_motor(right_motor, -speedFact-10);
		} else {
			set_motor(left_motor, -speedFact);
			set_motor(right_motor, speedFact);
		}
	}
}

/*Returns the average values of the pixels of 3 rows
 *(one row above, and one row below the passed row int)
 *int row must be between 1 and 238
 */
void getRow(int row){
	int rowsAv[320] = {0};
	int pixelTotal = 0; 	//Total value of the pixels from all 3 rows
	int average = 0; 	    	// will hold the average value of the three pixels
	int max = 150;
	
	int i = 0;
	for (int pixel = 0; pixel < 320; pixel++ ) { //Get's every 10th pixel
		//printf("%d\n", pixel);
		pixelTotal = get_pixel(row-1,pixel,3) + get_pixel(row,pixel,3) + get_pixel(row+1,pixel,3);
		average = pixelTotal/3;
		//printf("%d\n", average);
		rowsAv[i++] = average;
		//printf("%d\n", rowsAv[i++]);
		if (average > max) max = average;
	}
	//printf("End\n");
	
	int threshold = max / 2;
	
	//printf("[ ");
	for (int pixel = 0; pixel < 320; pixel++) {
		//printf("%d\n", pixel);
		if (rowsAv[pixel] > threshold) {
			linePlace[pixel] = 1;
		} else {
			linePlace[pixel] = 0;
		}
	//	printf("%d ", linePlace[pixel]);
	}
	//printf("]\n");
}

/*Detects where the white line is in relation to the centre of the robot.
 *returns positive integer if line is to the right
 *returns negative integer if line is to the left
 */
double detectLine(){
	//take a picture
	take_picture();
	//display_picture(1,0);
	
	getRow(160);
	
	double error = 0;
	numPixels = 0;
	for (int pixel = 0; pixel < 320; pixel++) {
		if (linePlace[pixel] == 1) {
			error += (pixel-160);
			numPixels++;
		}
	}
	
	return (error);
	}

int main(){
	init();
	
	char serverAdd[15] = {0};
	sprintf(serverAdd, "130.195.6.196");
	int serverPort = 1024;
	char serverReply[24] = {0};
	char serverMessage[24] = {0};
	sprintf(serverMessage, "Please");
	
	connect_to_server(serverAdd, serverPort);
	send_to_server(serverMessage);
	receive_from_server(serverReply);
	send_to_server(serverReply);
	setMotors(detectLine());
	
	while (true){
		setMotors(detectLine());
		//if (numPixels > 24){
		//	break;
		//}
	}
	stop(0);
return 0;}
