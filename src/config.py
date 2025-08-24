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
# -------------[ VOICE TRANSCRIPTION ]-------------

# Audio recording settings
SAMPLE_RATE = 16000     # Whisper requires 16kHz sample rate
DURATION = 5            # Recording duration in seconds
OUTPUT_FOLDER = "output"
RECORDING_FILENAME = "temp_recording.wav"

# Whisper model settings
WHISPER_MODEL = "base.en" # Let's use a smaller model for faster processing during testing

# -------------[ SENTIMENT ANALYSIS ]-------------

# -------------[ LANGUAGE SYNTHESIS ]-------------

# - ------------[ VOICE SYNTHESIS ]-------------

