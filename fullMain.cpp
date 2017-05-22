#include <stdio.h>
#include "E101.h"

int numPixels = 0;          //Number of white pixels in the row
int linePlace[320] = {0};   //Array storing detection of line for pixels

/**Sets the motors of the robot to turn towards where it should be
 * @param offSet how far off centre the desired position is
 */
void setMotors(int offSet){
	int left_motor = 1;
	int right_motor = 2;

	int speedFact = 40;     //How fast the robot moves by default
	double prop = 0.002;    //the proportional value for the offset
	int turnFact = offSet * prop;   //How sharply the robot turns to the left/right

    //Limit the turnFact, so it doesn't end up too large for set_motor
	if (turnFact > (254-speedFact)) turnFact = (254 - speedFact);

	if (offSet > 0) { //Line is to the right
		set_motor(left_motor, -1*(speedFact + (int)(offSet * prop)));
		set_motor(right_motor, speedFact - (int)(offSet * prop));
	}else if (offSet < 0) { //Line is to the left
		set_motor(left_motor, -1*(speedFact + (int)(offSet * prop)));
		set_motor(right_motor,(speedFact - (int)(offSet * prop)));
	}else if (offSet == 0) {
		if (numPixels == 0) { //If line isn't detected, then reverse, slightly offset
			set_motor(left_motor, speedFact);
			set_motor(right_motor, -speedFact-10);
		} else {
			set_motor(left_motor, -speedFact);
			set_motor(right_motor, speedFact);
		}
	}
}

/**Sets the values in linePlace[] for if it's white or black
 * @param row The row of the image which is read
 * (Takes the average of the 3 rows centered on passed row)
 */
void getRow(int row){
	int rowsAv[320] = {0};
	int pixelTotal = 0; 	//Total value of the pixels from all 3 rows
	int average = 0; 	    //The average (mean) value of the three pixels
	int max = 150;          //Default maximum value, acts as the minimum for the threshold

    if (row > 238) row = 238;   //Guarantee that the value passed is next too big
    if (row < 1) row = 1;       //or that the value is not too small

	for (int pixel = 0; pixel < 320; pixel++ ) { //Get's every pixel
		pixelTotal = get_pixel(row-1,pixel,3) + get_pixel(row,pixel,3) + get_pixel(row+1,pixel,3);
		average = pixelTotal/3; //Get the int average of the 3 pixels
		rowsAv[pixel] = average;
		if (average > max) max = average; //Make the threshold higher if the line brighter
	}
	
	int threshold = max / 2;

	for (int pixel = 0; pixel < 320; pixel++) {//Iterate through the row of pixels
		if (rowsAv[pixel] > threshold) {    //Test if pixel value is above threshold (white)
			linePlace[pixel] = 1;           //If so, set it to show there's a white pixel there (1)
		} else {
			linePlace[pixel] = 0;           //Otherwise, there is no white pixel there (0)
		}
	}
}

 /**Detects placement of white line in relation to centre of image
  * @return proportional value of where the line is, relative to centre
  * Value is positive if to the right, negative if to the left
  */
double detectLine(){
	take_picture(); //Take a picture with the camera
	getRow(160);    //and read the pixels in line 160 to find the line
	
	double error = 0;   //The error signal
	numPixels = 0;      //reset the number of white pixels
	for (int pixel = 0; pixel < 320; pixel++) {
		if (linePlace[pixel] == 1) {
			error += (pixel-160); //Increase error signal, according to distance from centre
			numPixels++;          //Increment the number of how many white pixels there are
		}
	}

    return (error);
}

int main(){
	init(); //Initialise the Raspberry Pi
	
	//Set the address of the server for the gate
    char serverAdd[15] = {0};
	sprintf(serverAdd, "130.195.6.196");
	int serverPort = 1024;

    //Set the message to send to the server, and initialise the reply
	char serverMessage[24] = {''};
	sprintf(serverMessage, "Please");
    char serverReply[24] = {''};

    //Connect to server, then send/receive message for opening the gate
	connect_to_server(serverAdd, serverPort);
	send_to_server(serverMessage);
	receive_from_server(serverReply);
	send_to_server(serverReply);

	setMotors(detectLine()); //Start moving the robot
	
	while (true){ //Loop endlessly
		setMotors(detectLine()); //Move robot according to where the line is
	}
	stop(0); //Stop the robot
return 0;}
