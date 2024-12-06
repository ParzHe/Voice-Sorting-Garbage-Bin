// Include the required library files
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define the pins for ultrasonic, motor, and LED lights
#define TRIGPIN 7                         
#define ECHOPIN 6 

// Initialize the LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize variables for ultrasonic distance measurement
int duration = 0;
int distance = 0;

//Initialize the LED and speaker
const int greenLight=13, redLight=12;
const int speaker=8;   

void setup() {
// Set up serial communication
  Serial.begin(9600);
  
  pinMode(redLight,OUTPUT);
  pinMode(greenLight,OUTPUT);

// Initialize the LCD
  lcd.begin(16, 2);
  Serial.println("LCD initialized");
  lcd.init();
  lcd.backlight();

// Set pin modes
  // Initialize speaker pin and ultrasonic sensor pins
   noTone(speaker);
   pinMode(TRIGPIN, OUTPUT);         
   pinMode(ECHOPIN, INPUT);          
}

void loop() 

{
  // Trigger ultrasound
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);

// Measurement of ultrasonic echo time
  duration = pulseIn(ECHOPIN, HIGH);
// Distance calculation based on echo time
  distance = duration * 0.017175;

// Use dtostrf to convert floating point numbers to strings.
  char buffer[10]; 
	dtostrf(distance, 5, 1, buffer); // Parameters: value, total width, number of decimal places, buffer

// Control the operation of individual components based on distance
    if (distance >3) 
  {
 // Display a Good message on the LCD
    digitalWrite(redLight,0);
    digitalWrite(greenLight,1);
    lcd.clear();
    lcd.setCursor(3,0);                //  LCD Cursor is set to the third character of the first column.
   	lcd.print("It's Good!!");             //  LCD Display Distance
   	lcd.setCursor(1,1);            // LCD Cursor set to 2nd column, 1st character 
    lcd.print("Distance: ");
   	lcd.print(buffer); 
  } 
  
  else {
    // At a distance of less than 3, turn on red light(flash)
    digitalWrite(redLight, HIGH);
    delay(1000); // Delay 1 second

    digitalWrite(redLight, LOW);
    delay(1000); // Delay 1 second
    digitalWrite(greenLight,0);

    playLowFrequency();
    delay(distance * 7);
	  noTone(speaker);
	  delay(distance * 7);
  
    lcd.clear();
    lcd.setCursor(3,0);                // LCD cursor is set to the third character of the first column.
   	lcd.print("Full!!");             // LCD cursor is set to display warnings
    lcd.setCursor(1,1);
    lcd.print("Distance:");         // LCD display distance and cm
   	lcd.print(buffer);                     
   
  } 
  delay(50); // Wait a while to avoid frequent testing
}

// Play low-frequency buzzer sound
void playLowFrequency() {
  tone(speaker, 1000); // 1000Hz
}