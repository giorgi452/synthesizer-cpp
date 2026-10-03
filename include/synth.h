#pragma once

#include "envelope.h"
#include "oscillator.h"
#include <atomic>
#include <cstddef>
#include <portaudio.h>
#include <vector>

class Synth {
public:
  Synth();
  ~Synth();

  bool initialize(double sampleRate = 44100.0,
                  unsigned long framesPerBuffer = 256);
  bool start();
  bool stop();

  // Lock-free commands called from the terminal input thread
  void triggerNote(double frequency);
  void releaseNote();
  void setWaveform(Waveform type);

  const std::vector<float> &getRecordedBuffer() const { return recordedBuffer; }

  // Internal callback routine called by PortAudio's high-priority audio thread
  int audioCallback(float *outputBuffer, unsigned long framesPerBuffer);

private:
  PaStream *stream{nullptr};
  double sampleRate{44100.0};
  unsigned long framesPerBuffer{256};

  Oscillator oscillator;
  Envelope envelope;

  // Lock-free atomic state variables for cross-thread synchronization
  std::atomic<double> targetFrequency{0.0};
  std::atomic<bool> noteActive{false};
  std::atomic<Waveform> currentWaveform{Waveform::SINE};

  // Buffer to record rendered output frames
  std::vector<float> recordedBuffer;

  // Static PortAudio C-style callback wrapper
  static int paCallback(const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo *timeInfo,
                        PaStreamCallbackFlags statusFlags, void *userData);
};
