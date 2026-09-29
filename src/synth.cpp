#include "synth.h"
#include <iostream>
#include <portaudio.h>

Synth::Synth() {}
Synth::~Synth() { Pa_Terminate(); }

bool Synth::initialize() {
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    return false;
  }
  return true;
}
