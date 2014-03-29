#include <math.h>
int leds[6] = {3, 5, 6, 9, 10, 11};
int rotateSpeed = 10;
int sumOld = 0;
int sumNew = 0;
int audioCutoff = 0;
int loops = 0;
boolean rotateOverride = false;
boolean music = false;
boolean playing = false;

int spectrumReset=5;
int spectrumStrobe=4;
int spectrumAnalog=0;

void setup () {
  pinMode(leds[0],OUTPUT);
  pinMode(leds[1],OUTPUT);
  pinMode(leds[2],OUTPUT);
  pinMode(leds[3],OUTPUT);
  pinMode(leds[4],OUTPUT);
  pinMode(leds[5],OUTPUT);
  
  pinMode(spectrumReset, OUTPUT);
  pinMode(spectrumStrobe, OUTPUT);
      
  digitalWrite(spectrumStrobe,LOW);
  digitalWrite(spectrumReset,HIGH);
  digitalWrite(spectrumStrobe,HIGH);
  digitalWrite(spectrumStrobe,LOW);
  digitalWrite(spectrumReset,LOW); 
  Serial.begin(9600);
}

void loop () {
    int rotateMode = -1;
    int notPlaying = 0;
    int prevMode = -1; //Tells system the previous state it was in
    int rotIntensity[6] = {0, 0, 0, 0, 0, 0}; //sets up the first of 3 things I am storing the values
    int musicIntensity[6] = {0, 0, 0, 0, 0, 0};
    while(true)
    {     
      delay(100);
      
      playing = readSpectrum(musicIntensity, audioCutoff); //runs the readSpectrum function to get the values
     
      if(!playing)
      {
        notPlaying++; 
      }
      else
      {
        notPlaying = 0; 
      }
      
      Serial.print("NP: ");
      Serial.print(notPlaying);
      Serial.print("\n");
      
      if(notPlaying < 600 || playing)
      {
        if(prevMode == -1) //-1 means it came from the starting loop.
        {
          prevMode = 0; 
        }
        else if(prevMode == 1) //1 means it came from the rotate loop
        {
          prevMode = 0; //0 means it came from the fade loop
          rotateMode = -1;
        }
        
        music = true;
        
        audioCutoff = 0;
      }
      else
      {            
        music = false;
    
        if(prevMode == -1)
        {
          prevMode = 1; 
          rotateMode = -1;
        }
        else if(prevMode == 0)
        {
          //fade(15, musicIntensity);
          prevMode = 1;
          rotateMode = -2;
        }
        
        rotateMode = rotate(rotIntensity, rotateMode);  
        
        audioCutoff = 0;
      }
      
      ledWrite(musicIntensity, rotIntensity, music);
    }
}

boolean readSpectrum(int musicIntensity[6], int audioCutoff) 
{
  // Band 0 = Lowest Frequencies.
  int tempL = 0;
  int tempR = 0;
  int readSum = 0;
  int readAvg = 0;
  for(byte Band = 0; Band < 7 ; Band++){
    int tempL = analogRead(0);
    int tempR = analogRead(1);
    double tempIntensity = 0;    
    
    Serial.print("L/R: ");
    Serial.print(tempL);
    Serial.print(", ");
    Serial.print(tempR);
    Serial.print("\n");
    
    /*This next portion takes the raw data from spectrum, and converts it to something that can be displayed. 
    Some data that should be mentioned, to get a good look, I square the input value. However the power values are inverted
    on the light strips. AKA 0 is full on, and 255 is off. To sum up, 16^2 is 256; the max value of tempL + tempR is 2048
    and 2048/126.68 is slightly greater than 16. This is okay because it is unlikely that both frequences will be capped out
    at any one time. */
    tempIntensity = (16 - ((tempL+tempR)/128));
    musicIntensity[Band] = square(tempIntensity); /*((tempL+tempR)/8);*/
    readSum += (tempL + tempR);
    
    digitalWrite(spectrumStrobe,HIGH);
    digitalWrite(spectrumStrobe,LOW);     
  }
  readAvg = readSum / 7;
  isPlaying2(readAvg);
}
 
void ledWrite(int musicIntensity[6], int rotIntensity[6], boolean music)
{
   int sum = 0;
   int avg = 0;
   if(music)
   {
     for(int i = 0; i < 6; i++)
     {
       analogWrite(leds[i], musicIntensity[i]);
     }
   }
   else
   {
     for(int i = 0; i < 6; i++)
     {
       analogWrite(leds[i], rotIntensity[i]);
     }
   }
} 

boolean isPlaying2(int avg)
{
  boolean playing = false;
  //int tempAvg = 255 - avg;
  //Serial.print(tempAvg);
  //Serial.print("\n");
  
  if(avg > audioCutoff)
  {
     playing = true; 
  }

  if(playing)
  {
   return true; 
  }
  return false;  
}

int rotate(int rotIntensity[6], int rotateMode)
 {
   if (rotateMode == -2)
    {
      for(int i = 0; i < 6; i++)
      {
        rotIntensity[i] = 255;
      }
      
      rotateMode = -1;
    }
    else if (rotateMode == -1)
    {
      
      rotIntensity[5] = rotIntensity[2] = (rotIntensity[2] - rotateSpeed);

      
      if(rotIntensity[2] < 10)
      {
        rotateMode = 0;
        rotIntensity[5] = rotIntensity[2] = 0;
      }
    }
    else if(rotateMode == 0) //LEDS 0 & 3
    {
      rotIntensity[3] = rotIntensity[0] = rotIntensity[0] - rotateSpeed;
      
      rotIntensity[5] = rotIntensity[2] = rotIntensity[2] + rotateSpeed;

      if(rotIntensity[0] < 10)
      {
        rotateMode = 1;
        rotIntensity[3] = rotIntensity[0] = 0;
      }
    }
    else if(rotateMode == 1) //LEDS 1 & 4
    {
      
      rotIntensity[4] = rotIntensity[1] = rotIntensity[1] - rotateSpeed;
      
      rotIntensity[3] = rotIntensity[0] = rotIntensity[0] + rotateSpeed;

      if(rotIntensity[1] < 10)
      {
        rotateMode = 2;
        rotIntensity[4] = rotIntensity[1] = 0;
      }
    }
    else if(rotateMode == 2) //LEDS 2 & 5
    {
      
      rotIntensity[5] = rotIntensity[2] = rotIntensity[2] - rotateSpeed;
      
      rotIntensity[4] = rotIntensity[1] = rotIntensity[1] + rotateSpeed;

      if(rotIntensity[2] < 10)
      {
        rotateMode = 0;
        rotIntensity[5] = rotIntensity[2] = 0;
      }
    }
    return rotateMode;
 }
 
 void serialPrint(int Intensity[6])
 {
  Serial.print("Intensity: ");
  Serial.print(Intensity[0]);
  Serial.print(", ");
  Serial.print(Intensity[1]);
  Serial.print(", ");
  Serial.print(Intensity[2]);
  Serial.print(", ");
  Serial.print(Intensity[3]);
  Serial.print(", ");
  Serial.print(Intensity[4]);
  Serial.print(", ");
  Serial.print(Intensity[5]);
  Serial.println(""); 
  Serial.println(""); 
 }
 
 void fade(int fadeSpeed, int musicIntensity[6])
{
  audioCutoff = 1024;
  int fadeDelay = 20;
  int ledSum = 0;
  for(int i = 0; i < 6; i++)
  {
   ledSum = ledSum + musicIntensity[i]; 
  }
  
 if(ledSum > 245)
  {
   for(int i = 0; i < 6; i++)
   {
    musicIntensity[i] = 255; 
   }
  }
  else
  {
    while(ledSum < 245)
    {
      ledSum = 0;
      for(int i = 0; i < 6; i++)
      {
        if(musicIntensity[i] < 255)
        {
           musicIntensity[i] = musicIntensity[i] + fadeSpeed;
           if(musicIntensity[i] > 255)
           { 
              musicIntensity[i] = 255;
           }
           ledSum = ledSum + musicIntensity[i];
        }
        
      } 
    }
    ledWrite(musicIntensity, musicIntensity, true);
  }
}
