"""
@file       epilogue_generator.py
@author     Simon Håkansson
@date       2025-08-28
@brief      Standalone script to trigger the final synthesis event.

@details    This module handles the end-of-life sequence for the GIBCA 2025
            interactive sculpture. It reads all stored user interactions,
            generates a final statement with a powerful LLM, and orchestrates
            the final audio playback and hardware shutdown sequence.

@copyright  Copyright (c) 2025 Simon Håkansson

This software is released under the MIT License.
See the LICENSE file in the project root for the full license text.
"""

# -------------[ LIBRARIES ]-------------
import sys
import os
from pathlib import Path

# Internal module imports
from language_synthesis import get_llm_response
from voice_synthesis import synthesize_speech

# Import configuration settings
from config import TRANSCRIPTIONS_LOG_PATH

# -------------[ INITIALIZATION ]-------------

# Form the proper path for the transcriptions log
root_dir = Path(__file__).parent.parent
transcriptions_full_path = root_dir / TRANSCRIPTIONS_LOG_PATH 

# Initialize the Piper voice model for TTS

# -------------[ FINAL SYNTHESIS FUNCTION ]-------------
def trigger_final_sequence():
    """
    @brief  Orchestrates the entire end-of-life sequence for the sculpture.
    
    @details This function is the master controller for the final event. It reads
             the log, generates the epilogue, synthesizes the audio, and sends
             the appropriate commands to the firmware to trigger the final
             movement and shutdown.
    """
    # 1. Read all the data
    all_text = read_transcriptions(str(transcriptions_full_path))
    
    # 2. Generate the epilogue
        
    # 3. Get user confirmation before proceeding
    input("Press Enter to begin the final sequence...")
    
    # 4. Send "final speech" command to firmware to set movement set for the speech
    
    # 5. Synthesize and play the final words
    
    # 6. Send "death" command to firmware
    
    print("Good bye.")


# -------------[ HELPER FUNCTIONS ]-------------
def read_transcriptions(file_path: str) -> str:
    """
    @brief Reads all transcribed text from the master log file
    
    @param file_path The file path to the transcriptions log
    @return contents of the log file as a string.
    """
    print("Loading transcriptions log...")
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            return f.read()
    except FileNotFoundError:
        print(f"Error: Log file not found at {file_path}")
        return "" # Return empty string if no log exists yet
    except IOError as e:
        print(f"Error reading log file: {e}")
        return ""
# -------------[ MAIN EXECUTION ]-------------
if __name__ == "__main__":
            
    print("Running epilogue generator...")
    trigger_final_sequence()