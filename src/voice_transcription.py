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
import whisper
import warnings
import subprocess

# Check that ffmpeg is installed and in PATH
try:
        subprocess.run(["ffmpeg", "-version"], check=True, capture_output=True)
except (subprocess.CalledProcessError, FileNotFoundError):
    raise EnvironmentError("ffmpeg is not installed or not found in PATH. Please install ffmpeg to use Whisper.")

# Suppress the "FP16 is not supported on CPU; using FP32 instead" warning from Whisper
warnings.filterwarnings("ignore", category=UserWarning)


# -------------[ CONFIGURATION ]-------------
# Audio recording settings
SAMPLE_RATE = 16000     # Whisper requires 16kHz sample rate
DURATION = 5            # Recording duration in seconds
OUTPUT_FOLDER = "output"
RECORDING_FILENAME = "temp_recording.wav"

# Whisper model settings
WHISPER_MODEL = "base.en" # Let's use a smaller model for faster processing during testing

# -------------[ INITIALIZATION ]-------------
print("Loading Whisper model...")
model = whisper.load_model(WHISPER_MODEL)
print("Model loaded.")


# -------------[ FUNCTIONS ]-------------
def record_audio() -> Path:
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
    output_dir = script_dir / OUTPUT_FOLDER
    
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

def transcribe_audio(path: str, model) -> str:
    """
    @brief  Transcribes audio from an input audio file using a Whisper model.

    @details This function takes the location of an audio file as input
             and .uses a local Whisper model to transcribe the audio content
             into text. The transcribed text is returned as a string.

    @todo Save the text to a file or database.
    @todo Make the model work on GPU for faster processing (fp16).

    @param path The file path of the audio recording to transcribe.
    @param model The Whisper model object to use for transcription.
    @return The transcribed text as a string.
    """
    print("Transcribing audio...")
    result = model.transcribe(path)
    transcription = result["text"]
    print("Transcription complete.")

    os.remove(path)  # Remove the temporary audio file for privacy.
    print("Temporary audio file deleted.")
    return transcription


# -------------[ MAIN EXECUTION FOR TESTING ]-------------
if __name__ == "__main__":
    audio_file_path = record_audio()
    print(f"Audio saved to: {audio_file_path}")
    transcription = transcribe_audio(str(audio_file_path), model)
    print(f"Transcription: {transcription}")