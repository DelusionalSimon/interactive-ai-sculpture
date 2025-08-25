"""
@file       main.py
@author     Simon Håkansson
@date       2025-08-24
@brief      The main orchestrator for the GIBCA 2025 interactive sculpture.

@details    This script manages the core AI conversation loop, integrating modules
            for voice transcription, sentiment analysis, language synthesis (LLM),
            and voice synthesis (TTS). It will eventually be triggered by
            serial events from the firmware.

@copyright  Copyright (c) 2025 Simon Håkansson

This software is released under the MIT License.
See the LICENSE file in the project root for the full license text.
"""
# TODO: For the MVP, the AI models are initialized directly in this script.
#       For a future, more robust version, this initialization logic should be
#       encapsulated within the respective handler modules to improve modularity
#       and create a cleaner separation of concerns.

# TODO: Add function to handle final synthesis.

# TODO: Add threading to make the ai loop run in parallel to enable the next todo
# TODO: Make the recording run until the user leaves the interaction zone

# -------------[ LIBRARIES ]-------------
import os
from pathlib import Path
import whisper
from piper import PiperVoice
import serial
import time

# Internal module imports
from voice_transcription import record_audio, transcribe_audio
from sentiment_analysis import analyze_sentiment
from language_synthesis import get_llm_response
from voice_synthesis import synthesize_speech

# import configuration settings
from config import ( WHISPER_MODEL, OUTPUT_WAV_PATH, MODEL_ONNX_PATH, MODEL_JSON_PATH, 
                    SERIAL_PORT, BAUD_RATE, SENTIMENT_TO_MOVEMENT_MAP, STANDARD_STATE,
                    SENTIMENT_GOOD_THRESHOLD, SENTIMENT_BAD_THRESHOLD, REACTION_TIMING)

# -------------[ INITIALIZATION ]-------------
# Initialize the Whisper model
print("Loading Whisper model...")
model = whisper.load_model(WHISPER_MODEL)
print("Model loaded.")

# form the proper paths for piper model and config
root_dir = Path(__file__).parent.parent
piper_model_full_path = root_dir / MODEL_ONNX_PATH
piper_config_full_path = root_dir / MODEL_JSON_PATH

# form the proper path for output directory
output_wav_full_path = root_dir / OUTPUT_WAV_PATH
# ensure output directory exists
os.makedirs(output_wav_full_path.parent, exist_ok=True)

# Initialize the Piper voice model
print("Loading Piper model...")
piper_voice = PiperVoice.load(  model_path=piper_model_full_path, 
                                config_path=piper_config_full_path)
print("Piper model loaded.")



# -------------[ FUNCTIONS ]-------------
def ai_pipeline(ser):
    """
    @brief  Executes the full AI conversation pipeline.
    
    @details This function orchestrates the entire process from audio recording
             to providing a final AI-generated text reply.
    """
    # Step 1: Record audio and transcribe
    audio_path = record_audio()
    user_input = transcribe_audio(str(audio_path), model)
    print(f"User said: {user_input}")

    # Step 2: Analyze sentiment
    sentiment_score = analyze_sentiment(user_input)
    print(f"Sentiment score: {sentiment_score}")

    # Send the command to the Arduino over the existing serial connection
    command = sentiment_to_movement(sentiment_score)
    ser.write(command.encode('utf-8'))
    print(f"Sent to Arduino: {command}")

    # Step 3: Get LLM reply
    ai_reply = get_llm_response(user_input)
    print(f"AI reply: {ai_reply}")

    # Step 4: Synthesize and play voice reply
    synthesize_speech(ai_reply, str(output_wav_full_path), piper_voice)
    print("Playing synthesized speech...")
    # Play the synthesized audio using ffplay (part of FFmpeg)
    os.system(f"ffplay -nodisp -autoexit -hide_banner -loglevel quiet {output_wav_full_path}")
 
    #TODO: Include prompt injection protection
    #TODO: save all user inputs to list for final synthesis

    print("Interaction complete.")
    # Send command for sculpture to go back to the base state after it has reacted
    time.sleep(REACTION_TIMING)
    command = STANDARD_STATE
    ser.write(command.encode('utf-8'))
    print(f"Sent to Arduino: {command}")

def main_loop():
    """
    @brief The main loop to listen for serial events and trigger the AI pipeline.
    
    @details This function will contain the main loop logic to listen for
             serial events from the Arduino and trigger the AI conversation
             pipeline accordingly.
    """
    try:
        with serial.Serial(SERIAL_PORT , BAUD_RATE, timeout=1) as ser:
                print(f"Connected to Arduino on {ser.portstr}")
                while True:
                    if ser.in_waiting > 0:
                        line = ser.readline().decode('utf-8').strip()
                        print(f"Received from Arduino: {line}")
                        if line == "event:user_interaction_start":
                            print("User interaction event received. Starting AI pipeline.")
                            ai_pipeline(ser)

    except serial.SerialException as e:
        print(f"Serial Error: {e}")
    except KeyboardInterrupt:
        print("Exiting...")

def sentiment_to_movement(sentiment_score: float) -> str:
    """
    @brief Takes a sentiment score as input and sends a movement command to the firmware
    
    @param float The sentiment score
    @return The movement map command to send over serial as a string
    """
    if sentiment_score > SENTIMENT_GOOD_THRESHOLD:
        state_name = "positive"
    elif sentiment_score < SENTIMENT_BAD_THRESHOLD:
        state_name = "negative"
    else:
        state_name = "neutral"
    return SENTIMENT_TO_MOVEMENT_MAP[state_name]


# -------------[ MAIN EXECUTION ]-------------
if __name__ == "__main__":
            
    print("Starting Main Loop...")
    main_loop()