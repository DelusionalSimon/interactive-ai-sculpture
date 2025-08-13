/*****************************************************************************
 *
 * File:        main.c
 * Author:      Simon Håkansson
 * Date:        2025-08-13
 *
 * Description: Firmware for handling sensor inputs and controlling 
 * the mechatronic leaves of the GIBCA 2025 interactive sculpture. 
 * Interfaces with a host computer over serial to initialize 
 * interaction and receive commands based on AI analysis.
 *
 * Copyright (c) 2025 Simon Håkansson
 *  
 * This software is released under the MIT License.
 * See the LICENSE file in the project root for the full license text.
 *
 *****************************************************************************/
//-------------[ LIBRARIES ]-------------
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

//-------------[ CONFIGURATION ]-------------
// Create an instance of the Adafruit_PWMServoDriver class
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Define motor outputs on the PCA9685
#define SERVO_LEAF_1 0 // testing with one leaf first

// Define the constants for the servos used 
#define PULSEWIDTH_MIN 500 
#define PULSEWIDTH_MAX 2500 
#define SERVO_MAX_ANGLE 270
#define SERVO_FREQUENCY 50 

// Define the movement range for the leaves to not break the sculpture
#define LEAF_MIN_ANGLE 45
#define LEAF_MAX_ANGLE 135

// Animation parameters to make each leaf move independently
float speedLeaf1 = 0.015;
float phaseLeaf1 = 0.0;
// TODO: Add more leaves and their parameters

//-------------[ SETUP FUNCTION ]-------------
void setup() {
  
  // Initialize the PCA9685 servo driver.
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQUENCY);

}

//-------------[ MAIN LOOP ]-------------
void loop() {

  float sinValueLeaf1 = sin(phaseLeaf1);

  // Get the angle the leaf should have, with higher resolution
  int angleLeaf1 = map(sinValueLeaf1*1000, -1000, 1000, LEAF_MIN_ANGLE*10, LEAF_MAX_ANGLE*10);
  // TODO : write a mapping function to use the full resolution of the servo

  // Convert the angle to pulse width
  int pulseWidthLeaf1 = map(angleLeaf1, 0, SERVO_MAX_ANGLE*10, PULSEWIDTH_MIN, PULSEWIDTH_MAX);
  
  // Set the servo position
  pwm.writeMicroseconds(SERVO_LEAF_1, pulseWidthLeaf1);

  // Increment the phase for the next iteration
  phaseLeaf1 += speedLeaf1;

  // Reset the phase if it exceeds 2*PI to avoid overflow
  if (phaseLeaf1 >= 2 * PI) {
    phaseLeaf1 -= 2 * PI;
  }

}

