/**
 * @file        main.cpp
 * @author      Simon Håkansson
 * @date        2025-08-23
 * @version     0.1.0
 * @brief       Isolated sensor bridge development, to be integrated into interactive-sculpture-firmware.
 * *
 * @copyright   Copyright (c) 2025 Simon Håkansson
 *
 * This software is released under the MIT License.
 * See the LICENSE file in the project root for the full license text.
 */

 /* 
 TODO: The current MVP relies solely on the ultrasonic sensor for both
 approach and interaction detection. Post-sprint, this needs to be
 rigorously tested for reliability. Implement capacitive touch and IR 
 distance detection as a fallback if necessary.
  */

//-------------[ LIBRARIES ]-------------
#include <Arduino.h>

//-------------[ CONFIGURATION ]------------
// to be moved to config.h when integrated into interactive-sculpture-firmware

// An enum to create clear, readable names for the sensors
enum SensorType {
  APPROACH_SENSOR,
  INTERACTION_SENSOR
};

// An enum to create clear, readable names for the user position states
enum UserState {
    NO_USER,
    USER_APPROACHING,
    USER_INTERACTING
};
UserState currentState = NO_USER;


// Define Ultrasonic sensor pins
// Approach sensor
#define APPROACH_TRIG_PIN 2
#define APPROACH_ECHO_PIN 3
// Interaction sensor
#define INTERACTION_TRIG_PIN 4
#define INTERACTION_ECHO_PIN 5

// Threshold distances in cm
#define APPROACH_THRESHOLD_CM 30.0 
#define INTERACTION_THRESHOLD_CM 10.0

// Ultrasonic sensor timing and conversion
const int ULTRASONIC_CLEAR_PULSE = 2; // in microseconds
const int ULTRASONIC_TRIGGER_PULSE = 10; // in microseconds
const float SPEED_OF_SOUND = 0.0343; // cm per microsecond

// Sampling Interval for the sensors
const int SAMPLING_INTERVAL_MS = 100; // 100 ms between readings

// Concurrency variables for each separate task
unsigned long userDetectTime = 0;

//-------------[ FUNCTION PROTOTYPES ]-------------
float readUltrasonicDistance(SensorType sensor);
void userDetection();

//-------------[ SETUP FUNCTION ]-------------
void setup() {

  // Initialize serial communication
  Serial.begin(9600);

  // Set the pin modes
  pinMode(APPROACH_TRIG_PIN, OUTPUT);
  pinMode(APPROACH_ECHO_PIN, INPUT);
  pinMode(INTERACTION_TRIG_PIN, OUTPUT);
  pinMode(INTERACTION_ECHO_PIN, INPUT);
}

//-------------[ MAIN LOOP ]-------------
void loop() {
  
  // Check for user approach and interaction
  userDetection(); 

}

//-------------[ HELPER FUNCTIONS ]-------------
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
    switch (currentState) {
        case NO_USER:
            if (approachDistance <= APPROACH_THRESHOLD_CM) {
                Serial.println("event:user_approach_start");
                currentState = USER_APPROACHING;
            }
            break;

        case USER_APPROACHING:
            if (interactionDistance <= INTERACTION_THRESHOLD_CM) {
                Serial.println("event:user_interaction_start");
                currentState = USER_INTERACTING;
            } else if (approachDistance > APPROACH_THRESHOLD_CM) {
                Serial.println("event:user_approach_end");
                currentState = NO_USER;
            }
            break;

        case USER_INTERACTING:
            if (interactionDistance > INTERACTION_THRESHOLD_CM) {
                Serial.println("event:user_interaction_end");
                currentState = USER_APPROACHING;
            }
            break;
    }
}