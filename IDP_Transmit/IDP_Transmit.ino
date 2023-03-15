//Serial has two main uses in this body of code.
//Almost all of the lines of code that include serial use it for user input
//The rest of the lines of code are used for testing.

//Constants
int inputPin = 7;
int outputPin = 10;
int markFrequency = 2125;
int spaceFrequency = 2295;
int serialFrequency = 1200;

char escapeChar = 0;
char nullChar = 27;

float baudRate = 45.45;
int durationOfBit = floor((1/baudRate) * 1000); //duration of each Bit in seconds in milliseconds

float markPeriod = 1/(markFrequency) * 1000000; //markPeriod in microseconds
float spacePeriod = 1/(spaceFrequency) * 1000000; //markPeriod in microseconds

void setup() {

 Serial.begin(serialFrequency);   //1200 baud is used as the output frequency for serial
 pinMode(outputPin, OUTPUT);  //Pin 10 is used as the output pin for tone commands
 pinMode(inputPin, INPUT); 
 Serial.println("Enter a string to be transmitted and then press \"Enter\": ");
 }

void loop() {
  waitingForAction();
}


void playSound(bool in) {
 if (in) {
   tone(outputPin, markFrequency, durationOfBit);  //Plays a tone of 2125hz for 22ms at Port 10 (corresponds to high or mark)
 } else {
   tone(outputPin, spaceFrequency, durationOfBit);  //Plays a tone of 2295hz for 22ms at Port 10 (corresponds to low or space)
 }
 Serial.print(in);
 delay(durationOfBit);   //Waits for the tone to be fully played before running any other code
 noTone(outputPin);  //Makes sure that no tone is played after 22ms.
}

//Receiver Code


//readInputString returns a string of the whole Received Message. If a blank string is returned, an error has occurred.
//Assumes that the first start bit (space) is already read.
String readInputString(int pin) {
  String message = "";
  bool receiving = true;
  while (receiving) {
    char inputChar = readInputChar(pin);
    if (inputChar == escapeChar) {
      return message;
    }
    else if (inputChar == nullChar) {
      return "";
    }
    else {
      message += inputChar;
    }
  }
   
}
//Reads the subsequent packet to get the equivalent character value to the packet. If an escape character is sent, the transmission has ended. If an null character
//is sent there is an error.
char readInputChar(int pin) {
  int decRepresentation = 0;
  int counter = 2;
  delay(durationOfBit/2);
  if (readInputBit(pin)) { //If the first bit is a mark instead of the start bit, the transmission has ended.
    return escapeChar; //Escape Character
  }
  while (counter < 10) {
    delay(durationOfBit);
    decRepresentation += readInputBit(pin) * pow(2, 9 - counter);
    counter++;
  }

  if (!readInputBit(pin)) { //If the last bit is not a mark, there is a problem)
    return nullChar; //Null Character
  }
  delay(durationOfBit/2);
  return decRepresentation;
}

//Returns whether the current input is a mark (1) or space (0)
bool readInputBit(int pin) {
  int inputPeriod = 0;
  inputPeriod = pulseIn(pin, HIGH, 100); //waits 0.1ms before collecting data
                                          //InputPeriod is the length of a high until it reaches a low
                                          //thus this will calculate half the period
  inputPeriod *= 2;
  int markDifference = abs(inputPeriod - markPeriod);
  int spaceDifference = abs(inputPeriod - spacePeriod);

  if(spaceDifference > markDifference) { //Whatever is greater is further away from the inputPeriod
    return 1;
  } else {
    return 0;
  }
}

void waitingForAction() {
  tone(outputPin, markFrequency);
  if(Serial.available() != 0) {
    processOutboundTransmission();
  }
  else if(!readInputBit(inputPin)) {
    processReceivedTransmission();
  }
  
}

void processOutboundTransmission() {
  bool confirmedString = false;
  String inputString;
  while (!confirmedString) {
    inputString = Serial.readString();
    Serial.println("Your string was: \"" + inputString + "\"");  //repeats the input phrase to user
    Serial.println("Press 1 to Confirm: ");

    while (Serial.available() == 0) {}  //Loop that causes the program to wait until input is given

    String confirmationString = Serial.readString();
    //If input is equal to the string "1", the loop will be exited. Otherwise, the user will be re-prompted for a phrase.
    if (confirmationString == "1") {
      confirmedString = true;
      break; //Leaves the While loop
    }
    Serial.println("Enter a string to be transmitted and then press \"Enter\": "); //Reprompt the User
  }

 int length = inputString.length();
 noTone(outputPin); //Turns off the mark tone as the program begins to transmit the phrase

  for (int i = 0; i < length; i++) {
     int inChar = inputString.charAt(i);
     playSound(0);  //start bit (space)
     for (int j = 7; j >= 0; j--) {
       playSound(bitRead(inChar, j));
     }
     playSound(1);  //end bit (mark)
  }
  Serial.println("Completed Sending the Message: \"" + inputString + "\"");

}

void processReceivedTransmission() {
  String message = readInputString(inputPin);
  Serial.println("The received message was: \"" + message + "\"");
  Serial.println("Enter a string to be transmitted and then press \"Enter\": ");
}



