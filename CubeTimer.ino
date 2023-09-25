/*
 * CubeTimer servers as a timer when solving the Rubik's cube.
 * See https://www.youtube.com/watch?v=0gvcZBQACMk for more.
 * 
 * Basd on implementation from by Bram Roelandts in October 2018.
 */

#include "LedControl.h"
#include <FastCapacitiveSensor.h>

FastCapacitiveSensor fastRightSensor;
FastCapacitiveSensor fastLeftSensor;

// MARK: - Define hardware setup
LedControl lc=LedControl(12,11,10,1);
// Physical Buttons
const int rightButtonPin = 8;
const int leftButtonPin = 2;
const int resetButtonPin = 7;

// MARK: - Variables
unsigned long startTime = 0;
int clearCounter = 0;
bool sessionInProgress = false;
bool previouslyPressed = false;


// MARK: - Setup
void setup() {
  Serial.begin(9600);
  // Initialize the 8-segment display
  lc.shutdown(0,false);
  lc.setIntensity(0,4);
  lc.clearDisplay(0);

  lc.setChar(0,2,'-',false);
  lc.setChar(0,5,'-',false);
  printLCD(0, 0, 1);
  printLCD(0, 3, 4);
  printLCD(0, 6, 7);

  // Setup the pins for each sensor
  pinMode(A0, OUTPUT);
  pinMode(A1, INPUT);

  pinMode(A2, OUTPUT);
  pinMode(A3, INPUT);
  // initialize fastRightSensor
  fastRightSensor.begin(A0, A1, 5.0, 8, 10, 0.2);
  // initialize fastRightSensor
  fastLeftSensor.begin(A2, A3, 5.0, 8, 10, 0.2);
  
  // Initialize the buttons
  // Physical buttons
  // pinMode(rightButtonPin, INPUT_PULLUP);
  // pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
}

#define CHRONO_CLEAR 0
#define CHRONO_HOLD 1
#define CHRONO_RUNNING 2
#define CHRONO_STOPPED 3


#define HIGH_LIMIT 300
#define LOW_LIMIT 500

// MARK: - Main loop
void loop() {
  static int counter = 0;
  // Fetch the latest button state
  static int rightButtonState = HIGH; // = digitalRead(rightButtonPin);
  
  //long val = rightSensor.capacitiveSensor(30);
  double rVal = fastRightSensor.touch();
  if (rVal > LOW_LIMIT && rightButtonState != LOW) {
    rightButtonState = LOW;
    //Serial.print("rightSensorLOW: ");
    //Serial.println(val);
  } else if (rVal < HIGH_LIMIT && rightButtonState != HIGH) {
    rightButtonState = HIGH;
    //Serial.print("rightSensorHIGH: ");
    //Serial.println(val);
  }

  static int leftButtonState = HIGH; //digitalRead(leftButtonPin);
  double lVal = fastLeftSensor.touch();
  if (lVal > LOW_LIMIT && leftButtonState != LOW) {
    leftButtonState = LOW;
    //Serial.print("leftSensorLOW: ");
    //Serial.println(val);
  } else if (lVal < HIGH_LIMIT && leftButtonState != HIGH) {
    leftButtonState = HIGH;
    //Serial.print("leftSensorHIGH: ");
    //Serial.println(val);
  }
  Serial.print("Left:");
  Serial.print(lVal);
  Serial.print(",");
  Serial.print("Right:");
  Serial.println(rVal);



  // Determine whether one or multiple buttons were pressed
  bool buttonsPressed =((rightButtonState == LOW) && (leftButtonState == LOW));// (rightButtonState == LOW) ? (leftButtonState == LOW) : false;
  bool oneButtonPressed = ((rightButtonState == LOW) || (leftButtonState == LOW));

  static int chronoState = CHRONO_CLEAR; // initial state

  if (0 == counter%1000) {
    //Serial.print ("IN_CHRONO_STATE: ");
    //Serial.println (chronoState);        
  }
  switch (chronoState) {
    case CHRONO_CLEAR:
    
      if (buttonsPressed) {
        
        chronoState = CHRONO_HOLD;
        //Serial.print ("TO_CHRONO_STATE: ");
        //Serial.println (chronoState);        
      }
     break;
    case CHRONO_HOLD:
      // Buttons released
      if (!oneButtonPressed) {
        //Serial.print ("Buttons: ");
        //Serial.print (leftButtonState);
        //Serial.print ("--");
        //Serial.println (rightButtonState);
        chronoState = CHRONO_RUNNING;
        startTime = micros()/1000;
        //Serial.print ("TO_CHRONO_STATE: ");
        //Serial.println (chronoState);        
      }
      break;
    case CHRONO_RUNNING:
      // Stop the chrono if any buttons; other than reset have been pressed after more than 1000 milliseconds
      if (oneButtonPressed && ((micros()/1000) - startTime > 1000) ) {
        chronoState = CHRONO_STOPPED;
        //Serial.print ("TO_CHRONO_STATE: ");
        //Serial.println (chronoState);        
      } else {
        updateTime();
      }
      break;
    case CHRONO_STOPPED:
    
      if (digitalRead(resetButtonPin) == LOW) {
        chronoState = CHRONO_CLEAR;
        // Zero the time
        startTime = micros()/1000;
        updateTime();
        //Serial.print ("TO_CHRONO_STATE: ");
        //Serial.println (chronoState);        
      }
      break;
  }
  if (0 == counter%1000) {
    //Serial.print ("OUT_CHRONO_STATE: ");
    //Serial.println (chronoState);        
  }
  ++counter;
 
}


// MARK: - Handlers

void updateTime() {

  // Determine how long it's been
  unsigned long currentTime = micros()/1000;
  unsigned long sessionTime = currentTime - startTime;

  // Split into different units of time
  int centiseconds = ((sessionTime / 10) % 100);
  int seconds = (((sessionTime / 1000) % 3600) % 60);
  int minutes = (((sessionTime / 1000) % 3600) / 60);

  // Print to the LCD
  printLCD(centiseconds, 0, 1);
  printLCD(seconds, 3, 4);
  printLCD(minutes, 6, 7);
}


// MARK: - Helpers

void printLCD(int v, int a, int b) {
    int ones;
    int tens;
    
    ones=v%10;
    v=v/10;
    tens=v%10;

    lc.setDigit(0,a,(byte)ones,false);
    lc.setDigit(0,b,(byte)tens,false);
}
