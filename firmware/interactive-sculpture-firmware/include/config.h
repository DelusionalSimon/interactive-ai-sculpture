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
#define SERVO_LEAF_1 0 // testing with one leaf first
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
const AngleRange LEAF_1_RANGE = {45, 135};
//TODO: Add more leaves with their ranges 


// Animation parameters declarations
extern float speedLeaf1;
extern float phaseLeaf1;
// TODO: Add more leaves

//TODO: Add more movement sets for listening, reacting, and other animations

#endif // CONFIG_H