"""
@file        voice_synthesis.py
@author      Simon Håkansson
@date        2025-08-24
@brief       Standalone Python script for handling voice synthesis using Piper.

@details     This script provides a function to synthesize voice from text input
             using the Piper TTS model. It can be run independently for testing.

@copyright   Copyright (c) 2025 Simon Håkansson

This software is released under the MIT License.
See the LICENSE file in the project root for the full license text.
"""
#TODO:      Post-sprint, experiment with Coqui TTS for more tunability and the ability
#           to use the results of the sentiment analysis to influence the voice synthesis.
#           This will require significant changes to the codebase and infrastructure.

#TODO:      Set up CUDA and GPU support for Piper to speed up synthesis.
#           Install onnxruntime-gpu and use voice = PiperVoice.load(..., use_cuda=True)


# -------------[ LIBRARIES ]-------------
import os
import sys
import subprocess
import wave
from pathlib import Path
from piper import PiperVoice

# import configuration settings
from config import MODEL_ONNX_PATH, MODEL_JSON_PATH, OUTPUT_WAV_PATH, SYN_CONFIG 


# Check that eSpeak NG is installed and in PATH
try:
    subprocess.run(["espeak-ng", "--help"], check=True, capture_output=True, timeout=5)
except (subprocess.CalledProcessError, FileNotFoundError, subprocess.TimeoutExpired):
    raise EnvironmentError("espeak-ng is not installed or not found in PATH. Please install espeak-ng properly to use Piper.")

# -------------[ INITIALIZATION ]-------------
# form the proper paths for piper model and config
root_dir = Path(__file__).parent.parent
piper_model_full_path = root_dir / MODEL_ONNX_PATH
piper_config_full_path = root_dir / MODEL_JSON_PATH

# form the proper path for output directory
output_wav_full_path = root_dir / OUTPUT_WAV_PATH
# ensure output directory exists
os.makedirs(output_wav_full_path.parent, exist_ok=True)

print("Loading Piper model...")
piper_voice = PiperVoice.load(  model_path=piper_model_full_path, 
                                config_path=piper_config_full_path)
print("Piper model loaded.")


# -------------[ FUNCTIONS ]-------------
def synthesize_speech(text: str, output_path: str):
    """
    @brief Synthesizes speech from text and saves it to a WAV file.
    
    @details This function uses the pre-loaded Piper voice model to convert
             the given text into audio. The resulting audio data is then
             saved to a WAV file at the specified output path.
    
    @param text The string of text to synthesize.
    @param output_path The file path to save the resulting WAV file to.
    """
    with wave.open(output_path, "wb") as wav_file:
        piper_voice.synthesize_wav(text, wav_file, SYN_CONFIG)
    print(f"Speech synthesized and saved to {output_path}")

# -------------[ MAIN EXECUTION FOR TESTING ]-------------
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python tts_handler.py \"Your sentence here\"")
        sys.exit(1)
    
    text_to_synthesize = sys.argv[1]
    
    print("Synthesizing speech...")
    synthesize_speech(text_to_synthesize, str(output_wav_full_path))
    