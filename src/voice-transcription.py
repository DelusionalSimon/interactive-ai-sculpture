"""
@file       voice-transcription.py
@author     Simon Håkansson
@date       2025-08-24
@brief      Standalone Python script for the voice transcription functions for the black flower.

@details    This script handles audio recording and transcription via Whisper
            for the GIBCA 2025 interactive sculpture project.

@copyright  Copyright (c) 2025 Simon Håkansson

This software is released under the MIT License.
See the LICENSE file in the project root for the full license text.
"""
# -------------[ LIBRARIES ]-------------
import sounddevice as sd
from scipy.io.wavfile import write
from pathlib import Path
import os

# TODO: add the whisper model once audio recording is working

# -------------[ CONFIGURATION ]-------------
SAMPLE_RATE = 16000     # Whisper requires 16kHz sample rate
DURATION = 5            # Recording duration in seconds
OUTPUT_FOLDER = "output"
RECORDING_FILENAME = "temp_recording.wav"

# -------------[ FUNCTIONS ]-------------
def record_audio() -> str:
    """
    @brief  Records audio from the microphone for a fixed duration.

    @details This function captures audio input and saves it to a temporary file
             in the configured output directory. It handles the creation of the
             directory if it does not exist.

    @todo Implement dynamic duration based on external input.

    @return The full path to the saved audio file.
    """
    # Get the project's root directory, one level up from the script's location.
    script_dir = Path(__file__).parent.parent
    
    # Define the output path relative to the script's location
    output_dir =  script_dir / OUTPUT_FOLDER
    
    # Create the output folder if it doesn't exist.
    output_dir.mkdir(parents=True, exist_ok=True)
    
    output_path = output_dir / RECORDING_FILENAME    

    # Use the `sd.rec()` function to record.
    print("Recording...")
    recording = sd.rec(frames=int(DURATION * SAMPLE_RATE), samplerate=SAMPLE_RATE, channels=1)
    sd.wait()  # Wait for the recording to finish.
    print("Recording complete.")
    
    # Save the NumPy array to a WAV file.
    write(output_path, SAMPLE_RATE, recording)

    return output_path

if __name__ == "__main__":
    audio_file_path = record_audio()
    print(f"Audio saved to: {audio_file_path}")