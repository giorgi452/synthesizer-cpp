#pragma once

enum class Waveform { SINE, SAWTOOTH, SQUARE, TRIANGLE };

class Oscillator {
public:
  Oscillator();

  void setWaveform(Waveform type);
  void setSampleRate(double sampleRate);

  float processNextSample(double frequency);

  void resetPhase();

private:
  Waveform waveform{Waveform::SINE};
  double phase{0.0};
  double sampleRate{44100.0};
};
