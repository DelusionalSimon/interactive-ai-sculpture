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

#ifndef CONFIG_H
#define CONFIG_H

//-------------[ HARDWARE PINS & ADDRESSES ]-------------
#define NUM_LEAVES 1 // Number of leaves in the sculpture

// define the pin positions for the leaves on the PCA9685 servo driver
struct Leaf {
    int servoPin; // Pin connected to the servo motor
};
const Leaf LEAF_PINS[NUM_LEAVES] = {
    {0}, // Leaf 1 servo pin
};

// TODO: Add other servo and sensor pins here

//-------------[ SERVO CALIBRATION ]-------------
#define PULSEWIDTH_MIN 500
#define PULSEWIDTH_MAX 2500
#define SERVO_MAX_ANGLE 270
#define SERVO_FREQUENCY 50

//-------------[ PHYSICAL CONSTRAINTS ]-------------
// Define the safe movement range for each of the leaves
 struct AngleRange {
    int minAngle; // Minimum angle in degrees
    int maxAngle; // Maximum angle in degrees
};
const AngleRange LEAF_RANGES[NUM_LEAVES] = {
    {45, 135}, // Leaf 1 range
};
//TODO: Add more leaves with their ranges 

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
    {0.01, 0.0}, // Leaf 1 baseline movement (speed in radians per loop, phase offset in radians)
    // TODO: Add more leaves
};


//-------------[ STATE MACHINE DEFINITION ]-------------

// An enum to give the states clear, readable names.
enum MovementState {
    IDLE,       // Default state when the sculpture is not interacting
    LISTEN,     // State when the sculpture is listening for input 
};

// Define the movement sets for different states
struct MovementSet {
    float amplitude;    // How wide the movement is
    float centerAngle;  // The midpoint of the movement
    float speedFactor;  // The speed of the sine wave (times baseline speed)
};
const MovementSet IDLE_MOVEMENT = {25.0, 90.0, 1};
const MovementSet LISTEN_MOVEMENT = {3.0, 20.0, 0.5};
// TODO: Add more movement sets

#endif // CONFIG_H