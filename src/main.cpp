#include "synth.h"
#include <iostream>

int main(int argc, char *argv[]) {
  Synth synth;
  if (synth.initialize()) {
    std::cout << "PortAudio initialized successfully!" << std::endl;
  }
  return 0;
}
