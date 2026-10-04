#include "synth.h"
#include <iostream>
#include <portaudio.h>

Synth::Synth() {
  recordedBuffer.reserve(44100 *
                         60); // Pre-allocate 1 minute of mono audio memory
}
Synth::~Synth() {
  stop();
  if (stream) {
    Pa_CloseStream(stream);
  }
  Pa_Terminate();
}

bool Synth::initialize(double rate, unsigned long bufferSize) {
  sampleRate = rate;
  framesPerBuffer = bufferSize;

  oscillator.setSampleRate(sampleRate);
  envelope.setSampleRate(sampleRate);

  PaError err = Pa_Initialize();
  if (err != paNoError) {
    std::cerr << "PortAudio initialization error: " << Pa_GetErrorText(err)
              << std::endl;
    return false;
  }

  err = Pa_OpenDefaultStream(
      &stream,
      0,         // No input channels
      1,         // 1 output channel (Mono)
      paFloat32, // 32-bit floating point output [-1.0, 1.0]
      sampleRate, framesPerBuffer, Synth::paCallback,
      this // Pass 'this' as userData pointer
  );

  if (err != paNoError) {
    std::cerr << "PortAudio OpenStream error: " << Pa_GetErrorText(err)
              << std::endl;
    return false;
  }

  return true;
}

bool Synth::start() {
  if (!stream)
    return false;
  PaError err = Pa_StartStream(stream);
  if (err != paNoError) {
    std::cerr << "PortAudio StartStream error: " << Pa_GetErrorText(err)
              << std::endl;
    return false;
  }
  return true;
}

bool Synth::stop() {
  if (!stream)
    return false;
  if (Pa_IsStreamActive(stream) == 1) {
    Pa_StopStream(stream);
  }
  return true;
}

void Synth::triggerNote(double frequency) {
  targetFrequency.store(frequency, std::memory_order_relaxed);
  noteActive.store(true, std::memory_order_relaxed);
}

void Synth::releaseNote() {
  noteActive.store(false, std::memory_order_relaxed);
}

void Synth::setWaveform(Waveform type) {
  currentWaveform.store(type, std::memory_order_relaxed);
}

int Synth::paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags, void *userData) {
  Synth *synth = static_cast<Synth *>(userData);
  return synth->audioCallback(static_cast<float *>(outputBuffer),
                              framesPerBuffer);
}

int Synth::audioCallback(float *outputBuffer, unsigned long frames) {
  // Read atomic flags once per audio block to minimize synchronization overhead
  double freq = targetFrequency.load(std::memory_order_relaxed);
  bool active = noteActive.load(std::memory_order_relaxed);
  Waveform wave = currentWaveform.load(std::memory_order_relaxed);

  oscillator.setWaveform(wave);

  if (active) {
    if (envelope.getState() == EnvelopeState::IDLE ||
        envelope.getState() == EnvelopeState::RELEASE) {
      envelope.noteOn();
    }
  } else {
    if (envelope.getState() != EnvelopeState::IDLE &&
        envelope.getState() != EnvelopeState::RELEASE) {
      envelope.noteOff();
    }
  }

  for (unsigned long i = 0; i < frames; ++i) {
    float rawSample = oscillator.processNextSample(freq);
    float gain = envelope.processNextSample();

    // Scale by 0.5f master gain headroom to avoid clipping
    float outputSample = rawSample * gain * 0.5f;

    outputBuffer[i] = outputSample;
    recordedBuffer.push_back(outputSample);
  }

  return paContinue;
}
