#include "key_map.h"
#include "synth.h"
#include "terminal.h"
#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  Synth synth;
  if (!synth.initialize(44100.0, 256)) {
    std::cerr << "Failed to initialize synthesizer." << std::endl;
    return 1;
  }

  if (!synth.start()) {
    std::cerr << "Failed to start audio stream." << std::endl;
    return 1;
  }

  TerminalManager terminal;
  KeyboardMap keyMap;

  std::cout << "--- Real-Time CLI Synthesizer Running ---" << std::endl;
  std::cout << "Play keys: A W S E D F T G Y H U J K" << std::endl;
  std::cout << "Waveforms: 1=Sine, 2=Saw, 3=Square, 4=Triangle" << std::endl;
  std::cout << "Press 'q' to quit." << std::endl;

  bool running = true;
  char lastPressedKey = 0;
  auto lastKeyPressTime = std::chrono::steady_clock::now();

  while (running) {
    if (terminal.kbhit()) {
      char key = terminal.getChar();

      if (key == 'q' || key == 'Q') {
        running = false;
      } else if (key == '1') {
        synth.setWaveform(Waveform::SINE);
      } else if (key == '2') {
        synth.setWaveform(Waveform::SAWTOOTH);
      } else if (key == '3') {
        synth.setWaveform(Waveform::SQUARE);
      } else if (key == '4') {
        synth.setWaveform(Waveform::TRIANGLE);
      } else if (keyMap.isNoteKey(key)) {
        KeyInfo info = keyMap.getKeyInfo(key);
        synth.triggerNote(info.frequency);
        lastPressedKey = key;
        lastKeyPressTime = std::chrono::steady_clock::now();
      }
    } else {
      // Silence note if key has not been re-triggered within 120ms (simulated
      // key-release)
      auto now = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                         now - lastKeyPressTime)
                         .count();
      if (elapsed > 120 && lastPressedKey != 0) {
        synth.releaseNote();
        lastPressedKey = 0;
      }
    }

    // Sleep briefly to avoid high CPU usage in the polling thread
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  synth.stop();
  return 0;
}
