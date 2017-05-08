void setMotors(int offSet){
	int left_motor = 1;
	int right_motor = 2;
	int speedFact = 127;
	//double scaleFact = 1.0;
	double prop = ((double)offSet/136);
	
	if (offSet > 0) {
		set_motor(left_motor, speedFact + (int)(speedFact * prop));
		set_motor(right_motor, speedFact - (int)(speedFact * prop));
	}
	if (offSet < 0) {
		set_motor(left_motor, speedFact - (int)(speedFact * prop));
		set_motor(right_motor, speedFact + (int)(speedFact * prop));
	}
	if (offSet == 0) {
		set_motor(left_motor, speedFact);
		set_motor(right_motor, speedFact);
	}
}

int main(){
	init();
	int numPixels = 0;
	
	while (true){
		setMotors(detectLine());
		if (numPixels > 24){
			break;
		}
	}
	stop(0);
return 0;}
