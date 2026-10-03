#include "envelope.h"
#include <algorithm>

Envelope::Envelope() { recalculateSteps(); }

void Envelope::setSampleRate(double rate) {
  sampleRate = rate;
  recalculateSteps();
}

void Envelope::setADSR(double attackSec, double decaySec, float sustainLvl,
                       double releaseSec) {
  attackTime = std::max(0.001, attackSec);
  decayTime = std::max(0.001, decaySec);
  sustainLevel = std::clamp(sustainLvl, 0.0f, 1.0f);
  releaseTime = std::max(0.001, releaseSec);
  recalculateSteps();
}

void Envelope::recalculateSteps() {
  attackStep = static_cast<float>(1.0 / (attackTime * sampleRate));
  decayStep =
      static_cast<float>((1.0 - sustainLevel) / (decayTime * sampleRate));
  releaseStep = static_cast<float>(sustainLevel / (releaseTime * sampleRate));
}

void Envelope::noteOn() { state = EnvelopeState::ATTACK; }

void Envelope::noteOff() {
  if (state != EnvelopeState::IDLE) {
    state = EnvelopeState::RELEASE;
    // Calculate dynamic release step based on current gain position
    if (currentGain > 0.0f) {
      releaseStep =
          static_cast<float>(currentGain / (releaseTime * sampleRate));
    }
  }
}

float Envelope::processNextSample() {
  switch (state) {
  case EnvelopeState::IDLE:
    currentGain = 0.0f;
    break;
  case EnvelopeState::ATTACK:
    currentGain += attackStep;
    if (currentGain >= 1.0f) {
      currentGain = 1.0f;
      state = EnvelopeState::DECAY;
    }
    break;
  case EnvelopeState::DECAY:
    currentGain -= decayStep;
    if (currentGain <= sustainLevel) {
      currentGain = sustainLevel;
      state = EnvelopeState::SUSTAIN;
    }
    break;
  case EnvelopeState::SUSTAIN:
    currentGain = sustainLevel;
    break;
  case EnvelopeState::RELEASE:
    currentGain -= releaseStep;
    if (currentGain <= 0.0f) {
      currentGain = 0.0f;
      state = EnvelopeState::IDLE;
    }
    break;
  }

  return currentGain;
}
