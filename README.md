# The Black Flower: AI-Driven Mechatronic System

**Status:** [Completed / Shipped - Autumn 2025]

A real-time, sensor-driven interactive installation that integrates embedded hardware, generative AI, and physical actuation. Exhibited at the Gothenburg International Biennial of Contemporary Art (GIBCA) 2025.

> **Note:** This repository contains the source code for the AI pipeline, embedded firmware, and system integration logic.


## System Architecture

The system is designed as a **closed-loop physical AI agent**. It continuously monitors its environment via sensors, processes that data through a multi-stage AI pipeline, and manifests its internal state through physical movement and synthesized speech.

### The "Sensor -\> AI -\> Actuator" Loop

1.  **Sensing:** An array of ultrasonic and IR sensors monitors the physical proximity and engagement of visitors.
2.  **Perception (AI):** Spoken input is captured via a high-fidelity microphone, transcribed locally (Whisper), and analyzed for sentiment (VADER).
3.  **Cognition (LLM):** The transcribed text and sensor context are fed into a Large Language Model (via Groq API) to generate a context-aware verbal response and a target emotional state.
4.  **Actuation (Embedded):** The system translates the target emotional state into stochastic movement patterns, driving high-torque servos via a custom C++ firmware on an Arduino controller.

## Technical Stack

### Software & AI Pipeline

  * **Orchestration:** Python (AsyncIO)
  * **Speech-to-Text:** OpenAI Whisper (Local deployment)
  * **LLM Inference:** Llama 3 (via Groq API for low-latency \<100ms response)
  * **Voice Synthesis:** Piper TTS (Local, low-latency)
  * **Sentiment Analysis:** VADER (NLTK)

### Hardware & Embedded

  * **Controller:** Arduino Uno R3 with custom firmware
  * **Driver:** PCA9685 16-Channel PWM Driver
  * **Actuators:** 3× DM 8125MG High-Torque Digital Servos (25kg/cm)
  * **Sensors:** Ultrasonic Rangefinders (HC-SR04), IR Obstacle Sensors, Capacitive Touch
  * **Audio:** Custom pre-amp circuit + Class-D Amplifier


## Installation & Prerequisites

To reproduce or run the software stack locally:

### 1\. System Dependencies

  * **FFmpeg:** Required for audio processing (Whisper). Must be in `$PATH`.
  * **eSpeak NG:** Required for phoneme generation (Piper TTS). Must be in `$PATH`.

### 2\. Python Environment

```bash
conda create -n blackflower python=3.10
conda activate blackflower
pip install -r requirements.txt
```

### 3\. API Configuration

Create a `.env` file in the root directory:

```bash
GROQ_API_KEY="your_api_key_here"
```

## Core Challenges Solved

### 1\. Latency Minimization

Achieving conversational latency in a physical object required moving from cloud-based TTS (ElevenLabs) to a local neural TTS (Piper) and utilizing the Groq API for inference. Total round-trip time (Audio In -\> Audio Out) was optimized to sub-second levels.

### 2\. Stochastic State Machines

To prevent the sculpture from looking robotic, the firmware implements a stochastic state machine. Instead of looping animations, the system probabilistically transitions between states ("Breathing", "Curious", "Agitated") based on the current sentiment value derived from the user interaction.

### 3\. Robustness

The system was hardened for public deployment. This included:

  * **Watchdog Timers:** To reset the microcontroller in case of I2C hang-ups.
  * **Input Sanitization:** Safeguards against prompt injection attacks via spoken audio.
  * **Privacy-First Design:** Audio buffers are processed in RAM and immediately overwritten; no voice data is stored.


## License

Distributed under the MIT License. See `LICENSE` for more information.
