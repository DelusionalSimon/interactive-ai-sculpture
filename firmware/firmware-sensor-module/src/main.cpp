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

// An enum to create clear, readable names for our sensors
enum SensorType {
  APPROACH_SENSOR,
  INTERACTION_SENSOR
};

// Define Ultrasonic sensor pins
// Approach sensor
#define APPROACH_TRIG_PIN 2
#define APPROACH_ECHO_PIN 3
// Interaction sensor
#define INTERACTION_TRIG_PIN 4
#define INTERACTION_ECHO_PIN 5


// Ultrasonic sensor timing and conversion
const int ULTRASONIC_CLEAR_PULSE = 2; // in microseconds
const int ULTRASONIC_TRIGGER_PULSE = 10; // in microseconds
const float SPEED_OF_SOUND = 0.0343; // cm per microsecond

// Main loop delays for the test harness
const int APPROACH_INTERVAL_MS = 500;   // Read the approach sensor every 500ms
const int INTERACTION_INTERVAL_MS = 1200; // Read the interaction sensor every 1.2s

// Timer variables for each task
unsigned long lastApproachReadingTime = 0;
unsigned long lastInteractionReadingTime = 0;

//-------------[ FUNCTION PROTOTYPES ]-------------
float readUltrasonicDistance(SensorType sensor);

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
  // Check if it's time to take a new sensor reading
  if (millis() - lastApproachReadingTime >= APPROACH_INTERVAL_MS) {
    lastApproachReadingTime = millis(); // Update the timer

    float distance = readUltrasonicDistance(APPROACH_SENSOR);
    Serial.print("Approach Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  // An second independent task to test that functionality works in parallel
  if (millis() - lastInteractionReadingTime >= INTERACTION_INTERVAL_MS) {
    lastInteractionReadingTime = millis(); // Update the timer

    float distance = readUltrasonicDistance(INTERACTION_SENSOR);
    Serial.print("Interaction Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

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