//Serial has two main uses in this body of code.
//Almost all of the lines of code that include serial use it for user input
//The rest of the lines of code are used for testing.
int inputPin = 7;
int outputPin = 10;
int markFrequency = 2125;
int spaceFrequency = 2295;

float baudRate = 45.45;
float durationOfBit = (1/45.45) * 1000000; //duration of each Bit in seconds in microseconds

float markPeriod = 1/(markFrequency * 1000) ; //markPeriod in microseconds
float spacePeriod = 1/(spaceFrequency * 1000); //markPerod in microseconds

void setup() {
 // put your setup code here, to run once:


 Serial.begin(1200);   //1200 baud is used as the output frequency for serial
 pinMode(outputPin, OUTPUT);  //Pin 10 is used as the output pin for tone commands


 }


void loop() {
 // put your main code here, to run repeatedly:
 bool stringAchieved = false;  //As long as this value is false, a user input has not been chosen
 bool repeatString = false;    //If true, the user input will be repeated until the program is reset.
                               //Otherwise, the user will get prompted for user input once the phrase has been transmitted
 String inputString;           //Input String
 while (!stringAchieved) {
   tone(10, markFrequency); //A mark tone is played while nothing is transmitted (idle)
   Serial.println("Enter a string to be transmitted and then press \"Enter\": ");


   while (Serial.available() == 0) {}  //Loop that causes the program to wait until input is give


   inputString = Serial.readString();
   Serial.println("Your string was: \"" + inputString + "\"");  //repeats the input phrase to user
   Serial.println("Press 1 to Confirm: ");


   while (Serial.available() == 0) {}  //Loop that causes the program to wait until input is given


   String confirmationString = Serial.readString();


   //If input is equal to the string "1", the loop will be exited. Otherwise, the user will be re-prompted for a phrase.
   if (confirmationString == "1") {
     stringAchieved = true;
   } 
 }
  Serial.println("Press 1 to Repeat this Message, else press any other character: ");


 while (Serial.available() == 0) {}  //Loop that causes the program to wait until input is given


 String confirmationString = Serial.readString();
 //If input is not equal to 1, the input phrase will not be repeated.
 if (confirmationString == "1") {
   repeatString = true;
   Serial.println("Your string will now repeat. Send any key to Serial Monitor to stop.");
 }

 noTone(10); //Turns off the mark tone as the program begins to transmit the phrase
 int length = inputString.length();
 bool stringPrint = true;
 while (stringPrint) {  // this loop will continue forever if repeatString is true


   for (int i = 0; i < length; i++) {
     int inChar = inputString.charAt(i);
     Serial.println("");
     playSound(0);  //start bit (space)
     for (int j = 7; j >= 0; j--) {
       playSound(bitRead(inChar, j));
     }
     playSound(1);  //end bit (mark)
   }
   Serial.println("");


   //If any input is detected in the serial buffer, the program will stop repeating and prompt the user for a new phrase
   if (Serial.available() != 0) {
     Serial.readString(); //clears the input buffer to not interfere with future code
     repeatString = false;
   }
  
   if (!repeatString) {
     stringPrint = false;
   }
 }
}


void playSound(bool in) {
 if (in) {
   tone(10, markFrequency, 22);  //Plays a tone of 2125hz for 22ms at Port 10 (corresponds to high or mark)
 } else {
   tone(10, spaceFrequency, 22);  //Plays a tone of 2295hz for 22ms at Port 10 (corresponds to low or space)
 }
 Serial.print(in);
 delay(22);   //Waits for the tone to be fully played before running any other code
 noTone(10);  //Makes sure that no tone is played after 22ms.
}

//Receiver Code


//Returns whether the current input is a mark (1) or space (0)
int readInput(int pin) {
  int inputPeriod = 0;
  inputPeriod = pulseIn(pin, HIGH, 2000); //waits 2ms before collecting data
                                          //InputPeriod is the length of pulse from a high to a low.
                                          //thus this will calculate half the period
  inputPeriod *= 2;
  int markDifference = abs(inputPeriod - markPeriod);
  int spaceDifference = abs(inputPeriod - spacePeriod);

  if(spaceDifference > markDifference) {
    return 1;
  } else {
    return 0;
  }
}
