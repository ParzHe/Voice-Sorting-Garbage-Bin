#include <Wire.h>
#include <Servo.h>

//Interface Ultrasonic Sensor with Arduino uno
#define echoPin 11  //Connect the echo pin of the ultrasonic sensor to D12 of the Arduino
#define trigPin 8   //Connect the trigger pin of the ultrasonic sensor to D10 of the Arduino

// Define servos
int a = 0;
Servo myservo;    // Residual
Servo myservo_1;  //Recyclable
// Servo myservo_2; Hazardous
Servo myservo_3;  //Food

// Define button pins
const int greenButtonPin = 13;
const int redButtonPin = 8;
const int blueButtonPin = 7;
const int grayButtonPin = 4;

// Define RGB LED pins
const int redLightPin = 3;
const int greenLightPin = 5;
const int blueLightPin = 6;

String s = "";  // Srial and voice
// String m = "";  // NodeMCU
char toNode = ' ';

int foodStatus = LOW;
int recyclableStatus = LOW;
int residualStatus = LOW;

// for ultrasonic
// int distance;
// long duration;  // Declare variables that hold duration and distance

void setup() {
  // Set and initialize the servos
  myservo.attach(9);  // Connect the servo to the corresponding pins
  myservo.write(90);  // Initialize the servo to the neutral position
  myservo_1.attach(10);
  myservo_1.write(90);
  // myservo_2.attach(11);
  // myservo_2.write(90);
  myservo_3.attach(12);
  myservo_3.write(90);

  Serial.begin(9600);

  // Initialize button pins as inputs
  pinMode(greenButtonPin, INPUT);
  // pinMode(redButtonPin, INPUT);
  pinMode(blueButtonPin, INPUT);
  pinMode(grayButtonPin, INPUT);

  // Initialize RGB LED pin as output
  pinMode(redLightPin, OUTPUT);
  pinMode(greenLightPin, OUTPUT);
  pinMode(blueLightPin, OUTPUT);

  Wire.begin(8);

  // Ultrasonic
  pinMode(trigPin, OUTPUT);  //Set trigPin as the output pin of Arduino
  pinMode(echoPin, INPUT);   //Set echoPin as the output pin of Arduino

  Wire.onReceive(receiveEvent); // Register receive event for receive instruction from master - ESP8286
  Wire.onRequest(requestEvent); // // Register request event for send bin status to master - ESP8286

  delay(3000);
}

void loop() {
  // Use button to control the bin
  checkButtonAndControl(greenButtonPin, foodStatus, "F", "f");  // Green button controls food waste
  // checkButtonAndControl(redButtonPin, "hazardous");    // Red button controls hazardous waste
  checkButtonAndControl(blueButtonPin, recyclableStatus, "C", "c");  // Blue button controls recyclable waste
  checkButtonAndControl(grayButtonPin, residualStatus, "R", "r");    // Gray buttons control residual waste

  // Check serial port input and get instruction from voice recognition module - LU-ASR01
  s = "";
  while (Serial.available() > 0) {
    s += char(Serial.read());
    Serial.print("Receive: ");
    Serial.println(s);  // Output to serial monitor for debugging
    s = numToBin(s); // 6 for food waste, 4 for recyclable, 3 for residual waste
    controlBin(s); // control each bin
    delay(100); // delay for avoiding multiple input
  }

  // distance = getUltrasonicValue(); // Used to detect whether the garbage is full
  delay(100);
}

// Input corresponding number to simplify the debug
// 6 for food waste, 4 for recyclable, 3 for residual waste
String numToBin (String charNum) {
  if(charNum == "6") {
    return "food";
  }
  if (charNum == "5") {
    return "hazardous";
  }
  if(charNum == "4") {
    return "recyclable";
  }
  if(charNum == "3") {
    return "residual";
  }
  return charNum;
}

// Receive instruction from master - ESP8286
void receiveEvent(int howMany) {
  s = "";
  while (Wire.available() > 0) {
    delay(2);
    s += char(Wire.read());
    Serial.print("Receive: ");
    Serial.println(s);  // Output to serial monitor for debugging
    // s = numToBin(s);
    controlBin(s);
    delay(100);
  }
}

// if the corresponding button is pressed, the bin will open or close
void checkButtonAndControl(int buttonPin, int& binStatus, String openMsg, String closeMsg) {
  if (digitalRead(buttonPin) == HIGH) {
    if (binStatus == LOW) {
      controlBin(openMsg);
    }
    else {
      controlBin(closeMsg);
    }
  }
}

// control bin when receive specific instruciton from button and Master (ESP8286 OR LU-ASR01)
void controlBin(String binType) {

  controOneBin(binType, "R", "r", "residual", myservo, residualStatus, 'R', 'r', 255, 0, 0);
  controOneBin(binType, "C", "c", "recyclable", myservo_1, recyclableStatus, 'C', 'c', 0, 255, 255);
  controOneBin(binType, "F", "f", "food", myservo_3, foodStatus, 'F', 'f', 255, 0, 255);
  
  delay(100);
}

// Implementation of control each button
void controOneBin (String msg, String openMsg, String closeMsg, String voiceMsg,  
                    Servo& servo, int& binStatus, 
                    char openSignal, char closeSignal, 
                    int red, int green, int blue ) {
  /* 
    args:
    msg: Message frrom parent;
    openMsg: Message to control the bin open;
    closeMsg: Message to control the bin close;
    voiceMsg: Message from voice control module to control the bin
    Servo: the servo to control the open and close of the bin
    binStatus: call variable of the binstatus
    openSignal: the message from arduino to NodeMCU, to update open status of the bin to the webpage
    closeSignal: the message from arduino to NodeMCU, to update close status of the bin to the webpage
    red, green, blue: the light status of the rgb led to stand for the open of the specific bin
  */

  if (msg == openMsg) {
    if (binStatus == LOW) {
      activateBin(servo, binStatus, openSignal, red, green, blue);
    }
  }
  else if (msg == closeMsg) { 
    if (binStatus == HIGH)  {
      closeBin(servo, binStatus, closeSignal);
    }
  }
  else if (msg == voiceMsg) {
    voiceControl(servo, binStatus, openSignal, closeSignal, red, green, blue);
  } 
}

// Open the bin
void activateBin(Servo& servo, int& binStatus, char openSignal, int red, int green, int blue) {
  servo.write(0);  // open bin
  toNode = openSignal;
  RGB_color(red, green, blue);  // Light up LED
  Serial.println("The lip have openend.");
  binStatus = HIGH;
  delay(50);
}

// close the bin
void closeBin(Servo& servo, int& binStatus, char closeSignal) {
  servo.write(90);  // close bin
  toNode = closeSignal;
  RGB_color(0, 0, 0);  // Turn off the LED
  Serial.println("The lip have closed.");
  binStatus = LOW;
  delay(50);
  // Serial.println();
}

// When use the voice to control the bin, the bin will automatically open and then close after 3.5 seconds
void voiceControl(Servo& servo, int& binStatus, char openSignal, char closeSignal, int red, int green, int blue) {
  activateBin(servo, binStatus, openSignal, red, green, blue);
  delay(3500);
  closeBin(servo, binStatus, closeSignal);
}

// Send the state of bin after open/close to Master - ESP8286
void requestEvent() {
  Wire.write(toNode); /*send string on request */
}

// control rgb led
void RGB_color(int redValue, int greenValue, int blueValue) {
  analogWrite(redLightPin, redValue);
  analogWrite(greenLightPin, greenValue);
  analogWrite(blueLightPin, blueValue);
}

// Ultrasonic sensor
int getUltrasonicValue() {
  long duration;  // Declare variables that hold duration and distance
  int sensorValue = 0;

  digitalWrite(trigPin, LOW);  //Generate square wave on trigger pin
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);  //Calculate distance to obstacles
  sensorValue = (duration * 0.034 / 2);

  Serial.print("Distance : ");
  Serial.print(sensorValue);
  Serial.println(" cm ");
  delay(250);
  return sensorValue;
}