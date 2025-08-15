/*****************************************************************************
 *
 * File:        test_main.cpp
 * Author:      Simon Håkansson
 * Date:        2025-08-15
 *
 * Description: Quick test blinking the built-in LED to verify 
 * that the code is running on the Arduino.
 *
 * Copyright (c) 2025 Simon Håkansson
 *  
 * This software is released under the MIT License.
 * See the LICENSE file in the project root for the full license text.
 *
 *****************************************************************************/
//-------------[ LIBRARIES ]-------------
#include <Arduino.h>


// -------------[ SETUP FUNCTION ]-------------
void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
 
}

//-------------[ MAIN LOOP ]-------------
void loop() {
   digitalWrite(LED_BUILTIN, HIGH); // Turn LED on
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);  // Turn LED off
  delay(500);        
}