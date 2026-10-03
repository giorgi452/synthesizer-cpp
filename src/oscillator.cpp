#include "oscillator.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Oscillator::Oscillator() {}

void Oscillator::setWaveform(Waveform type) { waveform = type; }

void Oscillator::setSampleRate(double rate) { sampleRate = rate; }

void Oscillator::resetPhase() { phase = 0.0; }

float Oscillator::processNextSample(double frequency) {
  if (frequency <= 0.0)
    return 0.0f;

  double phaseIncrement = (2.0 * M_PI * frequency) / sampleRate;
  phase += phaseIncrement;

  if (phase >= 2.0 * M_PI) {
    phase -= 2.0 * M_PI;
  }

  double sampleValue = 0.0;

  switch (waveform) {
  case Waveform::SINE:
    sampleValue = std::sin(phase);
    break;
  case Waveform::SAWTOOTH:
    sampleValue = 1.0 - (2.0 * (phase / (2.0 * M_PI)));
    break;
  case Waveform::SQUARE:
    sampleValue = (phase < M_PI) ? 1.0 : -1.0;
    break;
  case Waveform::TRIANGLE:
    if (phase < M_PI) {
      sampleValue = -1.0 + (2.0 * (phase / M_PI));
    } else {
      sampleValue = 3.0 - (2.0 * (phase / M_PI));
    }
    break;
  }

  return static_cast<float>(sampleValue);
}
