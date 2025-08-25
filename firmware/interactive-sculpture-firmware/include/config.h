/**
 * @file        config.h
 * @author      Simon Håkansson
 * @date        2025-08-19
 * @brief       Configuration file for the GIBCA 2025 interactive sculpture.
 *
 * @details     This file contains all the hardware pin definitions, servo
 * calibration constants, and animation parameters for the
 * flower-controller firmware.
 *
 * @copyright   Copyright (c) 2025 Simon Håkansson
*
 * This software is released under the MIT License.
 * See the LICENSE file in the project root for the full license text.
 */

// TODO: replace #define with const across the board 
// TODO: add type hinting

#ifndef CONFIG_H
#define CONFIG_H

//-------------[ HARDWARE PINS & ADDRESSES ]-------------
// Serial communication baud rate
#define BAUD_RATE 9600 

// Number of leaves in the sculpture
#define NUM_LEAVES 2 

// Define the pin positions for the leaves on the PCA9685 servo driver
struct Leaf {
    int servoPin; // Pin connected to the servo motor
};
const Leaf LEAF_PINS[NUM_LEAVES] = {
    {0}, // Leaf 1 servo pin
    {1},
};

// Define Ultrasonic sensor pins
// Approach sensor
#define APPROACH_TRIG_PIN 2
#define APPROACH_ECHO_PIN 3
// Interaction sensor
#define INTERACTION_TRIG_PIN 4
#define INTERACTION_ECHO_PIN 5

//-------------[ SERVO CALIBRATION ]-------------
#define PULSEWIDTH_MIN 500
#define PULSEWIDTH_MAX 2500
#define SERVO_MAX_ANGLE 270
#define SERVO_FREQUENCY 50

// -------------[ ULTRASONIC SENSOR CALIBRATION ]-------------
// An enum to create clear, readable names for the sensors
enum SensorType {
  APPROACH_SENSOR,
  INTERACTION_SENSOR
};

// Ultrasonic sensor timing and conversion
const int ULTRASONIC_CLEAR_PULSE = 2; // in microseconds
const int ULTRASONIC_TRIGGER_PULSE = 10; // in microseconds
const float SPEED_OF_SOUND = 0.0343; // cm per microsecond

// Sampling Interval for the sensors
const int SAMPLING_INTERVAL_MS = 100; // 100 ms between readings

//-------------[ PHYSICAL CONSTRAINTS ]-------------
// Define the safe movement range for each of the leaves
 struct AngleRange {
    int minAngle; // Minimum angle in degrees
    int maxAngle; // Maximum angle in degrees
};
const AngleRange LEAF_RANGES[NUM_LEAVES] = {
    {45, 135}, // Leaf 1 range
    {45, 135},
};
//TODO: Add more leaves with their ranges 

// Sensor threshold distances in cm
#define APPROACH_THRESHOLD_CM 30.0 
#define INTERACTION_THRESHOLD_CM 10.0

//-------------[ MOVEMENT SET CONFIGURATIONS ]-------------
// Declare the array of current phases for each leaf.
extern float currentPhases[];

// Declaration of the leaf baseline movement
struct BaselineMovement {
    float speed; // Speed of the movement
    float phaseOffset; // Phase offset for sine wave motion
};
// Define the baseline movement for each leaf
const BaselineMovement LEAF_BASELINES[NUM_LEAVES] = {
    {0.001, 0.0}, // Leaf 1 baseline movement (speed in radians per loop, phase offset in radians)
    {0.0015, 0.3},
    
};


//-------------[ STATE MACHINE DEFINITION ]-------------
// An enum to create clear, readable names for the user position states
enum UserState {
    NO_USER,
    USER_APPROACHING,
    USER_INTERACTING
};

// An enum to give the movement states clear, readable names.
enum MovementState {
    IDLE,               // Default state when the sculpture is not interacting
    LISTEN,             // State when the sculpture is listening for input 
    REACTING_POSITIVE,  
    REACTING_NEGATIVE,
    REACTING_NEUTRAL
};

// Define the movement sets for different states
struct MovementSet {
    float amplitude;    // How wide the movement is
    float centerAngle;  // The midpoint of the movement
    float speedFactor;  // The speed of the sine wave (times baseline speed)
};
const MovementSet IDLE_MOVEMENT = {25.0, 90.0, 1};
const MovementSet LISTEN_MOVEMENT = {3.0, 20.0, 0.5};
const MovementSet POSITIVE_MOVEMENT = {25.0, 90, 2};
const MovementSet NEGATIVE_MOVEMENT = {5, 135, 3};
const MovementSet NEUTRAL_MOVEMENT = {20, 90, 1.5};
// TODO: Add more movement sets

#endif // CONFIG_H