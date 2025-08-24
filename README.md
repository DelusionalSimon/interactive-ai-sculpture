# The Black Flower: Interactive AI Installation for GIBCA 2025

**Status:** [In Development]

A real-time interactive installation using Natural Language Processing (NLP), a local Large Language Model (LLM), and custom hardware, set to be exhibited at the Gothenburg International Biennial of Contemporary Art (GIBCA) in Autumn 2025.

---

## Prerequisites

### FFmpeg 

FFmpeg is required and must be in the system's PATH variable for the Whisper module that handles transcription to work.

### eSpeak NG

eSpeak NG is required and must be in the system's PATH variable for the Piper TTS to handle voice generation.

### API Keys

This project relies on the Groq API for its language model. To run the project, you must:

1. Create a Groq account and obtain an API key.
2. Create a file named `.env` in the root of the project.
3. Add your API key to the file in the following format:

`GROQ_API_KEY="your_api_key_here"`

## Core Objective & Technical Challenge

The primary technical objective of this project is to design, build, and deploy a robust, standalone system capable of real-time interaction with the public. The system must analyze spoken user input and translate semantic and emotional data into dynamic physical outputs through mechatronics and generated speech.

Key technical challenges include:
* **Real-Time Performance:** Ensuring low latency from audio capture to physical reaction in a live environment.
* **Local AI Deployment:** Running all ML models (transcription, sentiment analysis, LLM) locally on dedicated hardware to ensure user privacy and offline functionality.
* **User Interaction** generating contextually relevant, real-time verbal replies
* **System Hardening:** Implementing safeguards against prompt injection and managing unpredictable user interactions.
* **Hardware/Software Integration:** Creating a stable interface between the AI/software layer and the physical layer of microcontrollers, sensors, motors, and LEDs.

## Concept of Operations & User Interaction Flow

The installation creates a real-time exchange between a visitor and an AI entity embodied in a physical sculpture. The interaction follows five distinct phases:

1.  **Idle Phase:** When no one is nearby, the sculpture's mechanically controlled leaves are in a gentle, undulating "breathing" motion.

2.  **Approach Phase:** An ultrasonic sensor detects an approaching visitor. The leaves curl back to reveal the central flower, which whispers a subtle audio cue to invite interaction.

3.  **Interaction Phase:** An IR distance sensor detects when a visitor leans in. An audio prompt primes them to speak, and a high-fidelity microphone is activated to capture their input.

4.  **Reaction Phase:** When the visitor leans back, the microphone deactivates. The system then executes the following AI pipeline:
    * The audio is transcribed via a local Speech-to-Text model, and the original recording is immediately deleted for privacy.
    * Sentiment analysis is performed on the text to determine the emotional tone.
    * The transcribed text is sent to the local LLM to synthesize a concise, verbal reply.
    * After a brief, deliberate pause, a moment of artificial 'thought',the system delivers a synchronized audio-visual response:
      * A generative sequence of physical movement, influenced by the sentiment analysis, is initiated in the leaves.
      * Simultaneously, the synthesized verbal reply is voiced through a hidden speaker.
    * Finally, the transcription is appended to a master list for the final synthesis.

1.  **Final words:** At the conclusion of the exhibition, the LLM processes the entire master list of user interactions to synthesize a single, final speech. This is delivered via a main speaker, accentuated by a final sequence of movement and light before the installation becomes dormant.

## Technical Stack (initial plan)

* **AI / Machine Learning:**
    * Speech-to-Text: Whisper
    * Sentiment Analysis: VADER
    * Language Synthesis: Local LLM (Llama 3 or DeepSeek)
    * Voice Synthesis: ElevenLabs
    * Primary Languages: Python & C++
    * Core Libraries: Pytorch, Numpy
* **Hardware & Mechatronics:**
    * Microcontrollers: Arduino Uno r3, PCA9685 Servo Driver
    * Sensors : Condenser Microphone, Capacitive Touch Sensor, IR distance sensor (close by), Ultrasonic Distance sensors (long distance)
    * Actuators: 3 × DM 8125MG 270° 25kg*cm High-Torque Digital Servo Motors
    * Output: Small hidden speaker for whispers, Main speaker for final synthesis, Warm White LEDs 


## Role & Responsibilities

As **Project Lead**, my responsibilities include overall project strategy, timeline management, and final delivery. I am also serving as the sole **Lead R&D Engineer**, personally managing the full technical stack, from AI model deployment to microcontroller programming and hardware integration into the sculpture.


## About the Event

The Gothenburg International Biennial of Contemporary Art (GIBCA) is one of the leading art biennials in the Nordic region. Our installation will be featured across three events, culminating in a final showing at GIBCA.


## License

This project is licensed under the MIT License. See the LICENSE file for details.


