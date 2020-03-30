/*
 * CubeTimer servers as a timer when solving the Rubik's cube.
 * See https://www.youtube.com/watch?v=0gvcZBQACMk for more.
 * 
 * Written by Bram Roelandts in October 2018.
 */

#include "LedControl.h"

// MARK: - Define hardware setup
LedControl lc=LedControl(12,11,10,1);
// Buttons
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

  // Initialize the 8-segment display
  lc.shutdown(0,false);
  lc.setIntensity(0,4);
  lc.clearDisplay(0);

  lc.setChar(0,2,'-',false);
  lc.setChar(0,5,'-',false);
  printLCD(0, 0, 1);
  printLCD(0, 3, 4);
  printLCD(0, 6, 7);
  
  // Initialize the buttons
  pinMode(rightButtonPin, INPUT);
  pinMode(leftButtonPin, INPUT);
  pinMode(resetButtonPin, INPUT);
}


// MARK: - Main loop
void loop() {

  // Fetch the latest button state
  int rightButtonState = digitalRead(rightButtonPin);
  int leftButtonState = digitalRead(leftButtonPin);
  // Determine whether one or multiple buttons were pressed
  bool buttonsPressed = (rightButtonState == LOW) ? (leftButtonState == LOW) : false;
  bool oneButtonPressed = ((rightButtonState == LOW) || (leftButtonState == LOW));

  // Handle the button press based on the game state
  if (sessionInProgress) {
  
    // Check whether the game ended
    if (oneButtonPressed) {
      sessionInProgress = false;
      clearCounter = 0;  
    } else {
      updateTime();
    }
    
  } else {
    
    // Get ready to start a new game
    if (buttonsPressed == false && previouslyPressed == true) {

      // Start a new game
      sessionInProgress = true;
      previouslyPressed = false;
      startTime = micros()/1000;
      delay(100);
          
    } else if (buttonsPressed) {
      previouslyPressed = true;
    } else if (digitalRead(resetButtonPin) == HIGH) {
      // Start the game
      startTime = micros()/1000;
      updateTime();
    }
  }
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
