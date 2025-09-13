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
MovementState movementState = DEATH; // Start in DEATH state
MovementState pendingState = NO_CHANGE; // Keeps track of the next state

// Holds the live, interpolated movement parameters
MovementSet currentMovement; 

// Set up state machine for user detection
UserState userState = NO_USER;

// Flag to indicate if the sculpture is under external AI control
bool isAiControlled = false;

// Concurrency variables for each separate task
unsigned long userDetectTime = 0;
unsigned long lastStateChangeTime = 0;

//-------------[ FUNCTION PROTOTYPES ]-------------
// Mechatronics & Movement functions
void  moveLeaf(float phase, int leafIndex, const MovementSet& movementSet);
void  initializeLeafPositions();
void  updateLeafMovement();
// State Machine Functions
MovementSet getMovementSetForState(MovementState state);
void  requestStateChange(MovementState newState);
void  updateStateMachine();
void  readSerialCommands();
// Sensor Handling Functions
float readUltrasonicDistance(SensorType sensor);
void  userDetection();
// Diagnostics & Calibration Functions
void runDemonstrationMode();
// Utility Functions
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
float lerp(float start, float end, float progress);

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

  // TODO: Add a physical killswitch to stall the whole firmware

  // Diagnostics: Test and tune the system.
  // When in this mode comment out the Input functions userDetection() and 
  // readSerialCommands() below.
  runDemonstrationMode();
  
  // Input: Gather information from sensors and serial
  //userDetection(); 
  //readSerialCommands();

  // Process: Make decisions based on the new information
  updateStateMachine();

  // Output: Move the leaves to reflect the current state
  updateLeafMovement();

}

  
//-------------[ MECHATRONICS & MOVEMENT FUNCTIONS ]-------------

/** 
 * @brief  Translates an animation phase and MovementSet into a physical servo position.
 *
 * @details This is a core utility function that takes a point in an animation cycle
 * (the phase) and maps it to a precise pulse width for a specific servo,
 * respecting the pre-defined safe movement range for that leaf.
 * It uses a virtual range supplied in an input MovementSet to differentiate
 * the midpoint and amplitude of each type of movement
 *
 * @param   phase The current phase of the sine wave for the leaf.
 * @param   leafIndex The index of the leaf to move.
 * @param   movementSet The MovementSet currently active 
 * 
 */
void moveLeaf(float phase, int leafIndex, const MovementSet& movementSet) {
  
  // Calculate the sine value for the current phase of this leaf
  float sinValue = sin(phase);

  // Map the virtual center and amplitude directly to the physical safety range
  float angle = mapFloat( movementSet.centerAngle + sinValue * movementSet.amplitude,
                          VIRTUAL_MIN,
                          VIRTUAL_MAX,
                          LEAF_RANGES[leafIndex].minAngle,
                          LEAF_RANGES[leafIndex].maxAngle );

  // Safety net in case of floating point errors
  angle = constrain(angle, LEAF_RANGES[leafIndex].minAngle, LEAF_RANGES[leafIndex].maxAngle);

  // Convert the angle to pulse width
  int pulseWidth = mapFloat(angle, 0, SERVO_MAX_ANGLE, PULSEWIDTH_MIN, PULSEWIDTH_MAX);
  
  // Set the servo position
  pwm.writeMicroseconds(LEAF_PINS[leafIndex].servoPin, pulseWidth);

}

/**
 * @brief  Initializes the leaf positions based on their starting phases
 * and the idle movement set.
 * 
 */
void initializeLeafPositions() {

  for (int i = 0; i < NUM_LEAVES; i++) {    
    moveLeaf(LEAF_BASELINES[i].phaseOffset, i, DEATH_MOVEMENT);
  }

  // Give the servos a moment to reach their starting positions
  delay(1500);  
}

/**
 * @brief  Moves the leaf servos in organic paths.
 *
 * @details This function uses the moveLeaf() function to move all leaves in
 * organic undulating paths, using easing to make movement changes flow,
 * and handles phase wrapping to prevent overflow.
 *
 * @todo    add randomness to the movements to make them even more organic
 * 
 */
void updateLeafMovement() {

   // 1. Determine the destination based on the current state
    MovementSet targetMovement = getMovementSetForState(movementState);

    // 2. Smoothly interpolate the current movement towards the target
    currentMovement.centerAngle = lerp(currentMovement.centerAngle, targetMovement.centerAngle, SMOOTHING_FACTOR);
    currentMovement.amplitude = lerp(currentMovement.amplitude, targetMovement.amplitude, SMOOTHING_FACTOR);
    currentMovement.speedFactor = lerp(currentMovement.speedFactor, targetMovement.speedFactor, SMOOTHING_FACTOR);
    
    // 3. Move the leaves using the newly updated "currentMovement"
  for (int i = 0; i < NUM_LEAVES; i++) {

    // Move the leaf to its new position based on the current phase
    moveLeaf(currentPhases[i], i, currentMovement);

    // Increment the phase for the current leaf
    currentPhases[i] += (LEAF_BASELINES[i].speed*currentMovement.speedFactor);

    // Reset the phase of the leaf if it exceeds 2 * PI to avoid overflow
    if (currentPhases[i] >= 2 * PI) {
      currentPhases[i] -= 2 * PI;
    }

  }  
}


//-------------[ STATE MACHINE FUNCTIONS ]-------------

/**
 * @brief  Select the correct movement parameters based on the current state
 * 
 * @param   state The state to get movements for
 * 
 */
MovementSet getMovementSetForState(MovementState state) {
  switch (state) {
    case APPROACH: return APPROACH_MOVEMENT;
    case LISTEN: return LISTEN_MOVEMENT;
    case REACTING_POSITIVE: return POSITIVE_MOVEMENT;
    case REACTING_NEGATIVE: return NEGATIVE_MOVEMENT;
    case REACTING_NEUTRAL: return NEUTRAL_MOVEMENT;
    case FINAL_SPEECH: return FINAL_SPEECH_MOVEMENT;
    case DEATH: return DEATH_MOVEMENT;
    case IDLE:
    default: return IDLE_MOVEMENT;
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
void requestStateChange(MovementState newState) {
    // Log the request if it's different from the current state AND any pending state
    if (newState != movementState && newState != pendingState) {
        pendingState = newState;
    }
}

/**
 * @brief  Checks for and processes pending state changes after a cooldown
 * 
 */
void updateStateMachine() {
    // Check if a state change is pending AND the cooldown has expired
    if (pendingState != NO_CHANGE && millis() - lastStateChangeTime > STATE_CHANGE_COOLDOWN_MS) {
        
        // Apply the pending state change
        movementState = pendingState;
            
        // Clear the pending state
        pendingState = NO_CHANGE;

        // Update the timestamp to start the new cooldown period
        lastStateChangeTime = millis();
    }
}

/** 
 * @brief  Reads incoming serial commands and sets the state accordingly
 * 
 * @details This function takes in serial commands sent from the python AI pipeline and 
 * utilizes the requestStateChange() function to set the state accordingly. It also sets
 * the isAiControlled variable to make sure that incoming sensor readings don't interrupts
 * the series of movement states sent from the AI pipeline
 * 
 */
void readSerialCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        
        if (command == "set_state:REACTING_POSITIVE") {
            isAiControlled = true; // AI takes control
            requestStateChange(REACTING_POSITIVE); 
        } else if (command == "set_state:REACTING_NEGATIVE") {
            isAiControlled = true; // AI takes control
            requestStateChange(REACTING_NEGATIVE); 
        } else if (command == "set_state:REACTING_NEUTRAL") {
            isAiControlled = true; // AI takes control
            requestStateChange(REACTING_NEUTRAL); 
        } else if (command == "set_state:FINAL_WORDS") {
            isAiControlled = true; // AI takes control
            requestStateChange(FINAL_SPEECH); 
        } else if (command == "set_state:DEATH") {
            isAiControlled = true; // AI takes control
            requestStateChange(DEATH); 
        } else if (command == "set_state:IDLE") {
            isAiControlled = false; // AI takes control
            requestStateChange(IDLE); 
        }
    }
}


//-------------[ SENSOR HANDLING FUNCTIONS ]-------------

/** 
 * @brief  Reads distance from the ultrasonic sensor.
 * 
 * @details This function triggers the selected ultrasonic sensor and reads 
 * the echo time to calculate the distance to the nearest object in centimeters.
 * 
 * @param   sensor The sensor type to read from.
 * 
 * @return  Distance in centimeters as a float. 
 * 
 * @todo change pulseIn() for a non-blocking read function like those found in NewPing
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
  long duration = pulseIn(echoPin, HIGH, ULTRASONIC_TIMEOUT);
  float distance = (duration * SPEED_OF_SOUND) / 2; // Convert to cm

  // If the distance is 0 (a timeout), return our out-of-range value instead.
    if (distance == 0) {
        distance = SENSOR_OUT_OF_RANGE;
    }

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
    if(isAiControlled) {
        return; // Don't run user detection loop when the firmware is under the control of the AI
    }
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
                requestStateChange(APPROACH);
            }
            break;

        case USER_APPROACHING:
            if (interactionDistance <= INTERACTION_THRESHOLD_CM) {
                Serial.println("event:user_interaction_start");
                userState = USER_INTERACTING;
                requestStateChange(LISTEN);
            } else if (approachDistance > APPROACH_THRESHOLD_CM) {
                Serial.println("event:user_approach_end");
                userState = NO_USER;
                requestStateChange(IDLE);
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



//-------------[ DIAGNOSTIC & CALIBRATION FUNCTIONS ]-------------

/**
 * @brief  Cycles through all defined movement states for demonstration.
 * 
 * @details This is a nonblocking function intended for testing and showcasing
 * the animation engine. It should not be used in the final interactive loop.
 * 
 */
void runDemonstrationMode() { 
    static int currentDemoIndex = 0;

    if (millis() - lastStateChangeTime > DEMONSTRATION_MODE_TIMING) {
       
      lastStateChangeTime = millis();
        
      // Find the next valid state to demonstrate, skipping unwanted ones
      do {
          // Iterate demo index and round over when the end is reached
          currentDemoIndex = (currentDemoIndex + 1) % NUM_MOVEMENT_STATES;
          MovementState nextState = static_cast<MovementState>(currentDemoIndex);
          
          // If the next state is unwanted, skip the rest of the code and run the loop again 
          if (nextState == NUM_MOVEMENT_STATES || nextState == NO_CHANGE) {
              continue; // Continue to the next iteration
          }
          // if the state is wanted, update the state
          requestStateChange(nextState);

          // print the state to terminal for debugging
          Serial.println(nextState);

          break; // Exit the do-while loop
      } while (true); // This loop will skip through unwanted states until a valid state is found
    }    
}



//-------------[ UTILITY FUNCTIONS ]-------------

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
 * @brief  Calculates a point between two values using linear interpolation (lerp).
 * 
 * @param  start The starting value (returned when progress is 0.0).
 * @param  end The ending value (returned when progress is 1.0).
 * @param  progress The interpolation factor, typically a value from 0.0 to 1.0.
 * 
 * @return The interpolated value between the start and end points
 */
float lerp(float start, float end, float progress) {
  return start + (end - start) * progress;
}

