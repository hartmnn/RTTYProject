int inputPin = 7;
int outputPin = 10;
int markFrequency = 2125;
int spaceFrequency = 2295;
int serialFrequency = 1200;

float markPeriod = (1 / (float)markFrequency)*1000000;    //markPeriod in microseconds
float spacePeriod = (1 / (float)spaceFrequency)*1000000; 

void setup() {
  Serial.begin(9600);
  pinMode(inputPin,INPUT);

}

void loop() {
  if(!readInputBit(inputPin)){
    Serial.print(readInputChar(inputPin));
    delay(2);
  }
}
  

bool readInputBit(int pin) {
  long int t1 = micros();
  
  int inputPeriod = 0;
  int inputPeriodHigh = 0;
  int inputPeriodLow = 0;
  Serial.flush(); 

  noInterrupts();
  inputPeriodHigh = pulseIn(inputPin, HIGH);
  inputPeriodLow = pulseIn(inputPin, LOW);
  interrupts();
  inputPeriod = inputPeriodHigh + inputPeriodLow;  

  int markDifference = abs(inputPeriod - markPeriod);
  int spaceDifference = abs(inputPeriod - spacePeriod);

  long int t2 = micros();
  long int t = t2 - t1;
  if (t < 2000){
    delayMicroseconds(1990 - t);
  }
  if (spaceDifference > markDifference) {  //Whatever is greater is further away from the inputPeriod
    return 1;
  } else {
    return 0;
  }
}

char readInputChar(int pin) {
  int decRepresentation = 0;
  int inBit = 0;
  for (int i = 7; i >= 0; i--){
    delay(22);
    inBit = readInputBit(inputPin);
    decRepresentation += round((inBit * pow(2, i)));
  }
  delay(22);
  return decRepresentation;
}


