/*****************************************************************************
 *
 * File:        main.cpp
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
float speedLeaf1 = 0.001;
float phaseLeaf1 = 0.0;
// TODO: Add more leaves and their parameters

//-------------[ FUNCTION PROTOTYPES ]-------------
float map_float(float x, float in_min, float in_max, float out_min, float out_max);

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
  float angleLeaf1 = map_float(sinValueLeaf1, -1, 1, LEAF_MIN_ANGLE, LEAF_MAX_ANGLE);

  // Convert the angle to pulse width
  int pulseWidthLeaf1 = map_float(angleLeaf1, 0, SERVO_MAX_ANGLE, PULSEWIDTH_MIN, PULSEWIDTH_MAX);
  
  // Set the servo position
  pwm.writeMicroseconds(SERVO_LEAF_1, pulseWidthLeaf1);

  // Increment the phase for the next iteration
  phaseLeaf1 += speedLeaf1;

  // Reset the phase if it exceeds 2*PI to avoid overflow
  if (phaseLeaf1 >= 2 * PI) {
    phaseLeaf1 -= 2 * PI;
  }

}
 

//-------------[ HELPER FUNCTIONS ]-------------

 /**
 * @brief Re-maps a number from one range to another using floating-point math.
 * @param x The number to map.
 * @param in_min The lower bound of the value's current range.
 * @param in_max The upper bound of the value's current range.
 * @param out_min The lower bound of the value's target range.
 * @param out_max The upper bound of the value's target range.
 * @return The mapped value as a float.
 */
float map_float(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}