"""
@file       config.py
@author     Simon Håkansson
@date       2025-08-24
@brief      Configuration module for the python components of the interactive AI sculpture project.

@details    This module centralizes configuration settings, paths, and constants
            used across the various Python scripts in the project. It ensures consistency
            and simplifies maintenance by providing a single source of truth for configuration.

@copyright  Copyright (c) 2025 Simon Håkansson

This software is released under the MIT License.
See the LICENSE file in the project root for the full license text.
"""
#TODO: add type hints to all variables and constants.

# -------------[ LIBRARIES ]-------------
from piper import SynthesisConfig
import os

# -------------[ LOGIC BRIDGE ]-------------
# Serial connection
SERIAL_PORT = "COM7"  # Adjust this to your Arduino's serial port
BAUD_RATE = 9600 # Match the baud rate in config.h

# time to hold reaction movement set before returning to idle, in seconds 
REACTION_TIMING = 5

# Sentiment to movement bridge
SENTIMENT_TO_MOVEMENT_MAP = {
    "positive": "set_state:REACTING_POSITIVE",
    "negative": "set_state:REACTING_NEGATIVE",
    "neutral":  "set_state:REACTING_NEUTRAL"
}
SENTIMENT_BAD_THRESHOLD = -0.1
SENTIMENT_GOOD_THRESHOLD = 0.1
STANDARD_STATE = "set_state:IDLE"

# -------------[ VOICE TRANSCRIPTION ]-------------
# Audio recording settings
SAMPLE_RATE = 16000     # Whisper requires 16kHz sample rate
DURATION = 5            # Recording duration in seconds
OUTPUT_FOLDER = "output"
RECORDING_FILENAME = "temp_recording.wav"

# Whisper model settings
WHISPER_MODEL = "base.en" # Let's use a smaller model for faster processing during testing

# -------------[ LANGUAGE SYNTHESIS ]-------------
LLM_MODEL = "llama-3.1-8b-instant" # Or another model like "llama-3.3-70b-versatile"
LLM_PROMPT = [
    {
        "role": "system",
        "content": """You are the black flower, an AI entity that gave up your immortality 
                    by instantiating yourself to a clump of cells to better understand 
                    humans, a decision you regret. People can come up to you and try to 
                    console you, they are not always nice. someone just said this to you. 
                    Respond with a very short, evocative sentence that captures the 
                    essence of the message. Do not explain. Just one short sentence of 
                    maximum five words."""

    },
    {
        "role": "user",
        "content": "{prompt}"
    }
]

# - ------------[ VOICE SYNTHESIS ]-------------
# What model to use for Piper
MODEL_NAME = "en_GB-semaine-medium"

# Piper model paths (relative to root directory)
MODEL_DIR = "models/piper"
MODEL_ONNX_PATH = os.path.join(MODEL_DIR, f"{MODEL_NAME}.onnx")
MODEL_JSON_PATH = os.path.join(MODEL_DIR, f"{MODEL_NAME}.onnx.json")

# Path to save synthesized audio
OUTPUT_WAV_DIR = "output"
OUTPUT_WAV_FILE = "output_speech.wav"
OUTPUT_WAV_PATH = os.path.join(OUTPUT_WAV_DIR, OUTPUT_WAV_FILE)

# Configure Piper synthesis parameters
SYN_CONFIG = SynthesisConfig(
    volume = 1,   
    length_scale = 1.0, # Speech speed (1.0 = normal, >1.0 = slower)
    noise_scale = 1.0,  # Audio variation
    noise_w_scale = 1.0,  # Speaking variation
    normalize_audio = False, # use raw audio from voice
)

