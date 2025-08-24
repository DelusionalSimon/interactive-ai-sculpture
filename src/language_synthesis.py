"""
@file       language_synthesis.py
@author     Simon Håkansson
@date       2025-08-24
@brief      Standalone Python script for handling LLM interactions using Groq.

@details    This script provides a function to .talk to a large language model (LLM)
            via the Groq API. It can be run independently for testing.

@copyright  Copyright (c) 2025 Simon Håkansson

This software is released under the MIT License.
See the LICENSE file in the project root for the full license text.
"""
#TODO:  Implement a local model for privacy and to make the system self-contained.
#       Options include running a smaller LLaMA model with GGML or using other 
#       open-source models that can run on local hardware.
#       This will require significant changes to the codebase and infrastructure.

#TODO:  Add a counter to track token usage and a fallback mechanism if limits are reached.



# -------------[ LIBRARIES ]-------------
import os
import sys
from groq import Groq
from dotenv import load_dotenv

# load environment variables from .env file
load_dotenv()

# Load the Groq API key from the environment variables and check for its existence.
api_key = os.getenv("GROQ_API_KEY")
if not api_key:
    print("Error: GROQ_API_KEY environment variable not set. Please check your .env file.")
    sys.exit(1) # Exit the script with an error code

# Initialize the Groq client. This may fail if the API key is invalid.
try:
    client = Groq(api_key=api_key)
except Exception as e:
    # This will catch specific Groq API errors, authentication failures, etc.
    print(f"Error: Failed to initialize Groq client. Details: {e}")
    sys.exit(1)
print("Groq client initialized.")

# -------------[ FUNCTIONS ]-------------
def get_llm_response(prompt: str) -> str:
    """
    @brief Gets a conversational response from the Groq API.

    @details This function sends a prompt to the Groq API and retrieves a response
             from the specified LLM model. It handles the API interaction and
             returns the generated text.

    @todo Implement error handling for API failures and rate limiting.
    @todo add core prompt to guide the model's responses.
                      
    @param prompt The user's input string.
    @return The generated response text as a string.
    """
    chat_completion = client.chat.completions.create(
        messages=[
            {
                "role": "user",
                "content": prompt,
            }
        ],
        model="llama-3.1-8b-instant", # Or another model like "llama-3.3-70b-versatile"
    )
    return chat_completion.choices[0].message.content

# -------------[ MAIN EXECUTION FOR TESTING ]-------------
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: llm_handler.py \"Your prompt here\"")
        sys.exit(1)
    
    prompt = sys.argv[1]
    print("Thinking...")
    response = get_llm_response(prompt)
    print(f"Response: {response}")
