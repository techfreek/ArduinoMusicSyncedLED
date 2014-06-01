#include <math.h>
int leds[6] = {3, 5, 6, 9, 10, 11};
int rotateSpeed = 10;
boolean playing = false;
int fadeMode = -1;

int spectrumReset = 5;
int spectrumStrobe = 4;
int spectrumAnalog = 0;

unsigned long lastPlay;

void setup() {
	pinMode(leds[0], OUTPUT);
	pinMode(leds[1], OUTPUT);
	pinMode(leds[2], OUTPUT);
	pinMode(leds[3], OUTPUT);
	pinMode(leds[4], OUTPUT);
	pinMode(leds[5], OUTPUT);

	pinMode(spectrumReset, OUTPUT);
	pinMode(spectrumStrobe, OUTPUT);

	digitalWrite(spectrumStrobe, LOW);
	digitalWrite(spectrumReset, HIGH);
	digitalWrite(spectrumStrobe, HIGH);
	digitalWrite(spectrumStrobe, LOW);
	digitalWrite(spectrumReset, LOW);
	Serial.begin(9600);
}

void loop() {
	int spectrum[6] = {0, 0, 0, 0, 0, 0};
	int fade[6] = {255, 255, 255, 255, 255, 255};
	int volume = 0;
	while(true) {
		volume = listen(spectrum, 6);
        	Serial.print("Volume: ");
                Serial.print(volume);
                Serial.println("");
		if(volume >= 313) { //0 is full blast, this is 1/10 the max volume
                        Serial.println("Loud");
			lastPlay = millis();
			//calcColors(spectrum, 6); //Taken care of in millis
		} else {
			if((millis() - lastPlay) > 10000) { //if a play has not registered in 10 seconds
                                Serial.println("Quiet && waited");
				calcFade(spectrum, fade, 6);
			}
		}
		render(spectrum, 6);
                //delay(150);
	}

}

int listen(int spectrum[], int freqs) {
	// Band 0 = Lowest Frequencies.
	int tempL = 0;
	int tempR = 0;
	int readSum = 0;
	int readAvg = 0;
        digitalWrite(spectrumReset, HIGH);
        digitalWrite(spectrumReset, LOW);        
	for(byte Band = 0; Band < (freqs + 1) ; Band++){
                delay(20);
  		digitalWrite(spectrumStrobe,LOW);
		tempL = analogRead(0);
		tempR = analogRead(1);
		if(Band != (freqs + 1)) {
			
			double tempIntensity = 0;    
			
			/*Serial.print("L/R: ");
			Serial.print(tempL);
			Serial.print(", ");
			Serial.print(tempR);
			Serial.print("\n");*/
			
			/*This next portion takes the raw data from spectrum, and converts it to something that can be displayed. 
			Some data that should be mentioned, to get a good look, I square the input value. However the power values are inverted
			on the light strips. AKA 0 is full on, and 255 is off. To sum up, 16^2 is 256; the max value of tempL + tempR is 2048
			and 2048/126.68 is slightly greater than 16. This is okay because it is unlikely that both frequences will be capped out
			at any one time. */

			//tempIntensity = (16 - ((tempL+tempR)/128));
			//spectrum[Band] = square(tempIntensity); /*((tempL+tempR)/8);*/
                        //Serial.print("Left: ");
                        //Serial.println(255 - tempR);
                        //Serial.print("Right: ");
                        //Serial.println(255 - tempR);     
//                        spectrum[Band] = (tempL > tempR) ? tempL : tempR;                   
                        spectrum[Band] = (tempL + tempR) >>1; 
                        

			
		}

		readSum += (tempL + tempR);
		digitalWrite(spectrumStrobe,HIGH);

	}
        //Serial.println("Post scan");
        //serialPrint(spectrum, 6);
	return readSum;
}

void calcFade(int spectrum[], int fade[], int freqs) {
	int rotateSpeed = 10;
	if (fadeMode == -1)
	{
	  	fade[5] = fade[2] = (fade[2] - rotateSpeed);

		if(fade[2] < 10)
		{
			fadeMode = 0;
			fade[5] = fade[2] = 0;
		}
	}
	else if(fadeMode == 0) //LEDS 0 & 3
	{
		fade[3] = fade[0] = fade[0] - rotateSpeed;
		fade[5] = fade[2] = fade[2] + rotateSpeed;

		if(fade[0] < 10)
		{
			fadeMode = 1;
			fade[3] = fade[0] = 0;
		}
	}
	else if(fadeMode == 1) //LEDS 1 & 4
	{
		fade[4] = fade[1] = fade[1] - rotateSpeed;
		fade[3] = fade[0] = fade[0] + rotateSpeed;

		if(fade[1] < 10)
		{
			fadeMode = 2;
			fade[4] = fade[1] = 0;
		}
	}
	else if(fadeMode == 2) //LEDS 2 & 5
	{
		fade[5] = fade[2] = fade[2] - rotateSpeed;
		fade[4] = fade[1] = fade[1] + rotateSpeed;

		if(fade[2] < 10)
		{
			fadeMode = 0;
			fade[5] = fade[2] = 0;
		}
	}
	for(int i = 0; i < freqs; i++) {
		spectrum[i] = fade[i];
	}

}

void render(int spectrum[], int freqs) {
        //Serial.println("Pre Render");
	//serialPrint(spectrum, freqs);
	for(int i = 0; i < freqs; i++)
	{
		analogWrite(leds[i], spectrum[i]);
	}
}

void serialPrint(int Intensity[6], int freqs)
{
	Serial.print("Intensity: ");
	for(int i = 0; i < freqs; i++){
		Serial.print(Intensity[i]);
		Serial.print(", ");	
	}
	Serial.println(""); 
}
