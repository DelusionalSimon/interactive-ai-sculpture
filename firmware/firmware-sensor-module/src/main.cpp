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

// Define sensor pins
#define ULTRASONIC_TRIG_PIN 2
#define ULTRASONIC_ECHO_PIN 3


// Ultrasonic sensor timing and conversion
const int ULTRASONIC_CLEAR_PULSE = 2; // in microseconds
const int ULTRASONIC_TRIGGER_PULSE = 10; // in microseconds
const float SPEED_OF_SOUND = 0.0343; // cm per microsecond

// Main loop delay for the test harness
const int LOOP_DELAY_MS = 500;

// Timer variable
unsigned long lastReadingTime = 0;

//-------------[ FUNCTION PROTOTYPES ]-------------
float readUltrasonicDistance();

//-------------[ SETUP FUNCTION ]-------------
void setup() {

  // Initialize serial communication
  Serial.begin(9600);

  // Set the pin modes
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
}

//-------------[ MAIN LOOP ]-------------
void loop() {
  // Check if it's time to take a new sensor reading
  if (millis() - lastReadingTime >= LOOP_DELAY_MS) {
    lastReadingTime = millis(); // Update the timer

    float distance = readUltrasonicDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  // TODO: Add capacitive touch and IR sensor reading logic here
}

//-------------[ HELPER FUNCTIONS ]-------------
/** 
 * @brief  Reads distance from the ultrasonic sensor.
 * 
 * @details This function triggers the ultrasonic sensor and reads the echo time
 * to calculate the distance to the nearest object in centimeters.
 * 
 * @return  Distance in centimeters as a float. 
 */
float readUltrasonicDistance() {
  // Clear the trigger pin
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(ULTRASONIC_CLEAR_PULSE);

  // Set the trigger pin high for a specified pulse duration
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(ULTRASONIC_TRIGGER_PULSE);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  // Read the echo pin and calculate the distance
  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  float distance = (duration * SPEED_OF_SOUND) / 2; // Convert to cm

  return distance;
}