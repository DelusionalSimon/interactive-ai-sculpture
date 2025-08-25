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
MovementState movementState = IDLE; // Start in IDLE state

// Set up state machine for user detection
UserState userState = NO_USER;

// Concurrency variables for each separate task
unsigned long userDetectTime = 0;

//-------------[ FUNCTION PROTOTYPES ]-------------
void moveLeaf(float phase, int leafIndex);
void initializeLeafPositions();
void updateLeafMovement();
void setMovementState(MovementState state);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
float readUltrasonicDistance(SensorType sensor);
void userDetection();
void readSerialCommands();

//-------------[ SETUP FUNCTION ]-------------
void setup() {

  // Initialize serial communication for debugging
  Serial.begin(BAUD_RATE);

  // Set the pin modes for the ultrasonic sensors
  pinMode(APPROACH_TRIG_PIN, OUTPUT);
  pinMode(APPROACH_ECHO_PIN, INPUT);
  pinMode(INTERACTION_TRIG_PIN, OUTPUT);
  pinMode(INTERACTION_ECHO_PIN, INPUT);
    
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

    // Continuously update leaf movements
    updateLeafMovement();

    // Check for user approach and interaction
    userDetection(); 

    // Listen for commands from the host computer
    readSerialCommands();

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
 * @todo    Add logic to handle different movement sets (e.g., dance, listen).
 * 
 */
void updateLeafMovement() {

  MovementSet activeMovement;

  // Select the correct movement parameters based on the current state
  switch (movementState) {
      case LISTEN:
          activeMovement = LISTEN_MOVEMENT;
          break;
      case REACTING_POSITIVE:
          activeMovement = POSITIVE_MOVEMENT;
          break;
      case REACTING_NEGATIVE:
          activeMovement = NEGATIVE_MOVEMENT;
          break;
      case REACTING_NEUTRAL:
          activeMovement = NEUTRAL_MOVEMENT;
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

    // Reset the phase of the leaf if it exceeds 2 * PI to avoid overflow
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
  movementState = state;  
}

 /**
 * @brief Re-maps a number from one range to another using floating-point math.
 * 
 * @param x The number to map.
 * @param in_min The lower bound of the value's current range.
 * @param in_max The upper bound of the value's current range.
 * @param out_min The lower bound of the value's target range.
 * @param out_max The upper bound of the value's target range.
 * 
 * @return The mapped value as a float.
 */
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/** 
 * @brief  Reads distance from the ultrasonic sensor.
 * 
 * @details This function triggers the selected ultrasonic sensor and reads 
 * the echo time to calculate the distance to the nearest object in centimeters.
 * 
 * @param   sensor The sensor type to read from.
 * 
 * @return  Distance in centimeters as a float. 
 */
float readUltrasonicDistance(SensorType sensor) {
  int triggerPin;
  int echoPin;

  // Select the correct pins based on the sensor type
  switch (sensor) {
    case APPROACH_SENSOR:
      triggerPin = APPROACH_TRIG_PIN;
      echoPin = APPROACH_ECHO_PIN;
      break; // The 'break' is important!

    case INTERACTION_SENSOR:
      triggerPin = INTERACTION_TRIG_PIN;
      echoPin = INTERACTION_ECHO_PIN;
      break;
  }
  // Clear the trigger pin
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(ULTRASONIC_CLEAR_PULSE);

  // Set the trigger pin high for a specified pulse duration
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(ULTRASONIC_TRIGGER_PULSE);
  digitalWrite(triggerPin, LOW);

  // Read the echo pin and calculate the distance
  long duration = pulseIn(echoPin, HIGH);
  float distance = (duration * SPEED_OF_SOUND) / 2; // Convert to cm

  return distance;
}

/** 
 * @brief  Determines if user is approaching or within interaction range.
 * 
 * @details This function uses the readUltrasonicDistance() to determine if the 
 * user is approaching and then if they lean within interaction range.
 * It updates the userState accordingly and triggers state changesin the 
 * movement state machine and sends serial events that are used by the host 
 * computer to initiate AI interaction
 * . 
 */
void userDetection() {
    if (millis() - userDetectTime < SAMPLING_INTERVAL_MS) {
        return; // Not time to sample yet
    }
    userDetectTime = millis(); // Update the timer

    // Read the sensors once at the beginning of the check
    float approachDistance = readUltrasonicDistance(APPROACH_SENSOR);
    float interactionDistance = readUltrasonicDistance(INTERACTION_SENSOR);

    // User detection state machine
    switch (userState) {
        case NO_USER:
            if (approachDistance <= APPROACH_THRESHOLD_CM) {
                Serial.println("event:user_approach_start");
                userState = USER_APPROACHING;
                setMovementState(LISTEN);
            }
            break;

        case USER_APPROACHING:
            if (interactionDistance <= INTERACTION_THRESHOLD_CM) {
                Serial.println("event:user_interaction_start");
                userState = USER_INTERACTING;
            } else if (approachDistance > APPROACH_THRESHOLD_CM) {
                Serial.println("event:user_approach_end");
                userState = NO_USER;
                setMovementState(IDLE);
            }
            break;

        case USER_INTERACTING:
            if (interactionDistance > INTERACTION_THRESHOLD_CM) {
                Serial.println("event:user_interaction_end");
                userState = USER_APPROACHING;
            }
            break;
    }
}
void readSerialCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        
        if (command == "set_state:REACTING_POSITIVE") {
            setMovementState(REACTING_POSITIVE); 
        } else if (command == "set_state:REACTING_NEGATIVE") {
            setMovementState(REACTING_NEGATIVE); 
        } else if (command == "set_state:REACTING_NEUTRAL") {
            setMovementState(REACTING_NEUTRAL); 
        } else if (command == "set_state:IDLE") {
            setMovementState(IDLE); 
        }
    }
}
