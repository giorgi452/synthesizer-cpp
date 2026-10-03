#pragma once

enum class EnvelopeState { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };

class Envelope {
public:
  Envelope();

  void setSampleRate(double sampleRate);
  void setADSR(double attackSec, double decaySec, float sustainLevel,
               double releaseSec);

  void noteOn();
  void noteOff();

  float processNextSample();

  EnvelopeState getState() const { return state; }

private:
  EnvelopeState state{EnvelopeState::IDLE};
  double sampleRate{44100.0};

  // ADSR parameters
  double attackTime{0.01};  // 10ms default
  double decayTime{0.05};   // 50ms default
  float sustainLevel{0.7f}; // 70% level
  double releaseTime{0.1};  // 100ms default

  float currentGain{0.0f};

  // Calculated gain increments per sample frame
  float attackStep{0.0f};
  float decayStep{0.0f};
  float releaseStep{0.0f};

  void recalculateSteps();
};
