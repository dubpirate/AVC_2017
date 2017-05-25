#include <stdio.h>
#include "E101.h"

int numPixels = 0;          //Number of white pixels in the row
int linePlace[320] = {0};   //Array storing detection of line for pixels
int vertLinePlace[240] = {0};//Array storing detection of vertical line (side paths)
int vertNumPixels = 0;      //Number of white pixels in the column
int minThreshold = 240;     //Default maximum value, acts as the minimum for the threshold

bool redLine = false;   //Whether or not there is a red line ahead
int speedFact = 45;     //How fast the robot travels by default

/**Sets the motors of the robot to turn towards where it should be
 * @param offSet how far off centre the desired position is
 */
void setMotors(int offSet){
	int left_motor = 1;
	int right_motor = 2;

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
	max = minThreshold;     //reset the Maximum to default

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

/**Sets the values in vertLinePlace[] for if it's white or black
 * @param col The column of the image which is read
 * (Takes the average of the 3 columns centered on passed row)
 */
void getCol(int col){
	int rowsAv[240] = {0};
	int pixelTotal = 0; 	//Total value of the pixels from all 3 columns
	int average = 0; 	    //The average (mean) value of the three pixels
	max = minThreshold;     //Reset the maximum to default

	if (col > 358) col = 258;   //Guarantee that the value passed cannot be too big
	if (col < 1) col = 1;       //or that the value is can't be too small

	for (int pixel = 0; pixel < 240; pixel++ ) { //Get's every pixel
		pixelTotal = get_pixel(pixel,col-1,3) + get_pixel(pixel,col,3) + get_pixel(pixel,col+1,3);
		average = pixelTotal/3; //Get the int average of the 3 pixels
		rowsAv[pixel] = average;
		if (average > max) max = average; //Make the threshold higher if the line brighter
	}

	int threshold = max / 2;

	for (int pixel = 0; pixel < 240; pixel++) {//Iterate through the row of pixels
		if (rowsAv[pixel] > threshold) {    //Test if pixel value is above threshold (white)
			vertLinePlace[pixel] = 1;       //If so, set it to show there's a white pixel there (1)
		} else {
			vertLinePlace[pixel] = 0;       //Otherwise, there is no white pixel there (0)
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

/**
 * Detects the line to the side
 * @return error signal
 */
double detectSideLine(int col){
	take_picture(); //Take a picture with the camera
	getCol(col);    //and read the pixels in line 160 to find the line

	double error = 0;   //The error signal
	vertNumPixels = 0;      //reset the number of white pixels
	for (int pixel = 20; pixel < 220; pixel++) {
		if (vertLinePlace[pixel] == 1) {
			error += (pixel-120); //Increase error signal, according to distance from centre
			vertNumPixels++;          //Increment the number of how many white pixels there are
		}
	}

	return (error);
}

/**
 * Detects whether or not there is a red line ahead of the robot
 */
void detectRed() {
	take_picture(); //Take picture for reference

    //Initialise values for pixels
	int pixelRed, pixelGreen, pixelBlue;
	int totalRed = 0; //total amount of red in the pixels

    //Get the RGB values for 40 pixels in centre of image
	for (int pixel = 0; pixel < 40; pixel++) {
		pixelRed = get_pixel(pixel,120,0);
		pixelGreen = get_pixel(pixel,120,1);
		pixelBlue = get_pixel(pixel,120,2);
		totalRed += pixelRed;                   //Increase total red
        totalRed -= (pixelGreen + pixelBlue);   //Minus the amount of non-red
	}
    //If the area is mainly red, there's a red line there
	if (totalRed/40 > 100) redLine = true;
}

/**
 * Turns the robot left until line is past halfway
 */
void turnLeft(){
    double forwardError = 0;
	do {
        //Set the motors for turning left on the spot
        set_motor(1, speedFact);
        set_motor(2, speedFact);
        forwardError = detectLine();
	//Keep going whilst the line is not roughly centered
    } while(forwardError >= 0);
}

/**
 * Turns the robot right until line is past halfway
 */
void turnRight(){
    double forwardError = 0;
    do {
        //Set the motors for turning right on the spot
        set_motor(1, -speedFact);
        set_motor(2, -speedFact);
        forwardError = detectLine();
    //Keep going whilst the line is not roughly centered
    } while(forwardError <= 0);
}

/**
 * Method for navigating through Quadrant-C, should be renamed
 */
void detectSquare(){
	detectRed(); //Detect if there is a red line ahead of it

	//Detect if there is a line ahead of the robot
	int currentError = detectLine();
    //If there's a line, go forward
	if (numPixels > 0){setMotors(currentError); return;}

    //Otherwise the robot will have to turn

	vertNumPixels = 0;  //Reset number of pixels to the side
	detectSideLine(80); //Detect whether or not there's a line just to the left
    //If there is a line to the left, then turn left
	if (vertNumPixels > 0){turnLeft(); return;}

    //Otherwise see if there is a line to the right
    detectSideLine(160);
    if (vertNumPixels > 0) {turnRight(); return;} //And turn right

    setMotors(0); //Otherwise there's no line, and nother either side, so reverse
}

int main(){
	init(); //Initialise the Raspberry Pi
	
	//Set the address of the server for the gate
	char serverAdd[15] = {0};
	sprintf(serverAdd, "130.195.6.196");
	int serverPort = 1024;

	//Set the message to send to the server, and initialise the reply
	char serverMessage[24] = {0};
	sprintf(serverMessage, "Please work now");
	char serverReply[24] = {0};

	//Connect to server, then send/receive message for opening the gate
	connect_to_server(serverAdd, serverPort);
	send_to_server(serverMessage);
	receive_from_server(serverReply);
	send_to_server(serverReply);

	setMotors(detectLine()); //Start moving the robot

	while (true){ //Loop endlessly until it detects the end of Quadrant-B
	setMotors(detectLine()); //Move robot according to where the line is
		if (numPixels > 240) break;
	}
    
	printf("Starting Quadrant-C\n");
	while (! redLine){ //Keep going whilst there isn't any red
		detectSquare();
	}

	stop(0); //Stop the robot
return 0;}
