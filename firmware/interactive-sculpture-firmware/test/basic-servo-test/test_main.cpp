/*****************************************************************************
 *
 * File:        test_main.cpp
 * Author:      Simon Håkansson
 * Date:        2025-08-15
 *
 * Description: Quick test for servo functionality using the Servo library.
 *
 * Copyright (c) 2025 Simon Håkansson
 *  
 * This software is released under the MIT License.
 * See the LICENSE file in the project root for the full license text.
 *
 *****************************************************************************/
//-------------[ LIBRARIES ]-------------
#include <Arduino.h>
#include <Servo.h>

//-------------[ CONFIGURATION ]-------------
// Create an instance of the Servo class
Servo testServo;

// Define your servo's pulse width range
#define PULSEWIDTH_MIN  500
#define PULSEWIDTH_MAX  2500

// -------------[ SETUP FUNCTION ]-------------
void setup() {
  Serial.begin(9600);
  Serial.println("Direct Servo Test...");
  
  // Attach the servo on Pin 9 and specify the pulse width range
  testServo.attach(9, PULSEWIDTH_MIN, PULSEWIDTH_MAX);
  Serial.println("Servo attached.");
}

//-------------[ MAIN LOOP ]-------------
void loop() {
  Serial.println("Moving to 0 degrees...");
  testServo.write(0);   
  delay(2000);          

  Serial.println("Moving to 180 degrees...");
  testServo.write(180); 
  delay(2000);          
  
  Serial.println("Moving to 270 degrees...");
  testServo.write(270); 
  delay(2000);         
}