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
# -------------[ LIBRARIES ]-------------
import whisper

# Internal module imports
from voice_transcription import record_audio, transcribe_audio
from sentiment_analysis import analyze_sentiment

# import configuration settings
from config import WHISPER_MODEL

# -------------[ INITIALIZATION ]-------------
print("Loading Whisper model...")
model = whisper.load_model(WHISPER_MODEL)
print("Model loaded.")



# -------------[ FUNCTIONS ]-------------
def ai_pipeline():
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
    
    # Step 3: Get LLM reply

    # Step 4: Synthesize and play voice reply

    # TODO: Add logic to connect back to the Arduino

    print("Interaction complete.")


# -------------[ MAIN EXECUTION ]-------------
if __name__ == "__main__":
    # TODO: Add logic here to listen for a serial event from the Arduino
    
    
    print("Starting AI Conversation Loop...")
    ai_pipeline()