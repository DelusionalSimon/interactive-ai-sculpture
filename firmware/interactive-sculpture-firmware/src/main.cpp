/**
 * @file        main.cpp
 * @author      Simon Håkansson
 * @date        2025-08-13
 * @version     0.2.2
 * @brief       Firmware for the GIBCA 2025 interactive sculpture.
 *
 * @details     This firmware handles sensor inputs and controls the mechatronic
 * leaves of the sculpture. It interfaces with a host computer
 * over serial to initialize interaction and receive commands
 * based on AI analysis.
 *
 * @copyright   Copyright (c) 2025 Simon Håkansson
 *
 * This software is released under the MIT License.
 * See the LICENSE file in the project root for the full license text.
 */
//-------------[ LIBRARIES ]-------------
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <config.h>

//-------------[ INITIALIZATION ]-------------
// Create an instance of the Adafruit_PWMServoDriver class
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Initialize an array to hold the current phase for each leaf
float currentPhases[NUM_LEAVES];

//-------------[ FUNCTION PROTOTYPES ]-------------
void updateLeafMovement();
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

//-------------[ SETUP FUNCTION ]-------------
void setup() {
  
  // Initialize the PCA9685 servo driver.
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQUENCY);

  // Initialize the starting phase for each leaf
  for (int i = 0; i < NUM_LEAVES; i++) {
    currentPhases[i] = LEAF_BASELINES[i].phaseOffset;
  }

  // TODO: Move leaves to starting position
  

}

//-------------[ MAIN LOOP ]-------------
void loop() {
  
  updateLeafMovement();

}
 

//-------------[ HELPER FUNCTIONS ]-------------
/**
 * @brief  Calculates and sets the new position for all leaf servos based on a sine wave.
 *
 * @details This function reads the current phase and speed for the leaves, calculates a
 * new angle using a sine function to create a smooth, organic motion,
 * and then sends the corresponding pulse width to the servo driver. It also
 * handles phase wrapping to prevent overflow.
 *
 * @param   None
 *
 * @return  None
 */
void updateLeafMovement() {
  
  for (int i = 0; i < NUM_LEAVES; i++) {

    // Calculate the sine value for the current phase of this leaf
    float sinValue = sin(currentPhases[i]);

    // Get the angle the leaf should have, with higher resolution
    float angle = mapFloat(sinValue, -1, 1, LEAF_RANGES[i].minAngle, LEAF_RANGES[i].maxAngle);

    // Convert the angle to pulse width
    int pulseWidth = mapFloat(angle, 0, SERVO_MAX_ANGLE, PULSEWIDTH_MIN, PULSEWIDTH_MAX);
    
    // Set the servo position
    pwm.writeMicroseconds(LEAF_PINS[i].servoPin, pulseWidth);

    // Increment the phase for the current leaf
    currentPhases[i] += LEAF_BASELINES[i].speed;

    // Reset the phase of the leaf if it exceeds 2*PI to avoid overflow
    if (currentPhases[i] >= 2 * PI) {
      currentPhases[i] -= 2 * PI;
    }

  }  
}

 /**
 * @brief Re-maps a number from one range to another using floating-point math.
 * @param x The number to map.
 * @param in_min The lower bound of the value's current range.
 * @param in_max The upper bound of the value's current range.
 * @param out_min The lower bound of the value's target range.
 * @param out_max The upper bound of the value's target range.
 * @return The mapped value as a float.
 */
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}