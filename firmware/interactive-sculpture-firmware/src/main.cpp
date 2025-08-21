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

// Set up state machone for movement
MovementState currentState = IDLE; // Start in IDLE state

// A struct to hold the target state we are easing towards.
MovementSet targetState;

// Variables to hold the current, real-time animation parameters.
float currentAmplitude;
float currentCenterAngle;
float currentSpeed;



// Test harness configuration
unsigned long lastStateChangeTime = 0;
const long stateChangeInterval = 5000; 


//-------------[ FUNCTION PROTOTYPES ]-------------
void moveLeaf(float phase, int leafIndex);
void initializeLeafPositions();
void updateLeafMovement();
void setMovementState(MovementState state);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

//-------------[ SETUP FUNCTION ]-------------
void setup() {

  // Initialize serial communication for debugging
  Serial.begin(9600);
    
  // Initialize the PCA9685 servo driver.
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQUENCY);

  // Initialize the starting phase for each leaf
  for (int i = 0; i < NUM_LEAVES; i++) {
    currentPhases[i] = LEAF_BASELINES[i].phaseOffset;
  }

  // Move leaves to starting position
  initializeLeafPositions();
  

}

//-------------[ MAIN LOOP ]-------------
void loop() {
  
  // Simple logic to cycle through states for the test harness
  if (millis() - lastStateChangeTime > stateChangeInterval) {
        lastStateChangeTime = millis();
                
        if (currentState == IDLE) {
            setMovementState(LISTEN);
        } else {
            setMovementState(IDLE);
        }
    }
  updateLeafMovement();

}

  
//-------------[ HELPER FUNCTIONS ]-------------

/** 
 * @brief  Translates an animation phase into a physical servo position.
 *
 * @details This is a core utility function that takes a point in an animation cycle
 * (the phase) and maps it to a precise pulse width for a specific servo,
 * respecting the pre-defined safe movement range for that leaf.
 *
 * @param   phase The current phase of the sine wave for the leaf.
 * @param   leafIndex The index of the leaf to move.
 * 
 */
void moveLeaf(float phase, int leafIndex) {
  
  // Calculate the sine value for the current phase of this leaf
  float sinValue = sin(phase);

  // Get the angle the leaf should have using float precision
  float angle = mapFloat(sinValue, -1, 1, LEAF_RANGES[leafIndex].minAngle, LEAF_RANGES[leafIndex].maxAngle);

  // Convert the angle to pulse width
  int pulseWidth = mapFloat(angle, 0, SERVO_MAX_ANGLE, PULSEWIDTH_MIN, PULSEWIDTH_MAX);
  
  // Set the servo position
  pwm.writeMicroseconds(LEAF_PINS[leafIndex].servoPin, pulseWidth);

}

/**
 * @brief  Initializes the leaf positions based on their starting phases.
 * 
 */
void initializeLeafPositions() {
  
  for (int i = 0; i < NUM_LEAVES; i++) {

    // Move the leaf to its initial position based on its baseline phase offset
    moveLeaf(LEAF_BASELINES[i].phaseOffset, i);
    }

  // Give the servos a moment to reach their starting positions
  delay(1500);  
}

/**
 * @brief  Moves the leaf servos in organic paths
 *
 * @details This function uses the moveLeaf() function to move all leaves in
 * organic undulating paths and handles phase wrapping to prevent overflow.
 *
 * @todo add logic to handle amplitude and center angle dynamically
 * 
 */
void updateLeafMovement() {
  
   MovementSet activeMovement;

    // Select the correct movement parameters based on the current state
    switch (currentState) {
        case LISTEN:
            activeMovement = LISTEN_MOVEMENT;
            break;
        case IDLE:
        default:
            activeMovement = IDLE_MOVEMENT;
            break;
    }
  for (int i = 0; i < NUM_LEAVES; i++) {

    // Move the leaf to its new position based on the current phase
    moveLeaf(currentPhases[i], i);

    // Increment the phase for the current leaf
    currentPhases[i] += (LEAF_BASELINES[i].speed*activeMovement.speedFactor);

    // Reset the phase of the leaf if it exceeds 2*PI to avoid overflow
    if (currentPhases[i] >= 2 * PI) {
      currentPhases[i] -= 2 * PI;
    }

  }  
}

/**
 * @brief  Changes the current state.
 * 
 * @param   state The new state to set.
 * 
 * @todo    Implement logic to handle smooth transitions between states
 * 
 */
void setMovementState(MovementState state) {
  // Set the current state to the new state
  currentState = state;  


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