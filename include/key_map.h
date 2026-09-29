#pragma once

#include <cmath>
#include <unordered_map>

struct KeyInfo {
  const char *noteName;
  double frequency;
};

class KeyboardMap {
public:
  KeyboardMap() {
    // Equal Temperament Frequencies for Octave 4
    // Base note: A4 = 440 Hz
    map['a'] = {"C4", 261.63};
    map['w'] = {"C#4", 277.18};
    map['s'] = {"D4", 293.66};
    map['e'] = {"D#4", 311.13};
    map['d'] = {"E4", 329.63};
    map['f'] = {"F4", 349.23};
    map['t'] = {"F#4", 369.99};
    map['g'] = {"G4", 392.00};
    map['y'] = {"G#4", 415.30};
    map['h'] = {"A4", 440.00};
    map['u'] = {"A#4", 466.16};
    map['j'] = {"B4", 493.88};
    map['k'] = {"C5", 523.25};
  }

  bool isNoteKey(char key) const { return map.find(key) != map.end(); }

  KeyInfo getKeyInfo(char key) const {
    auto it = map.find(key);
    if (it != map.end()) {
      return it->second;
    }
    return {"Unknown", 0.0};
  }

  // Helper formula to compute any MIDI note frequency dynamically
  // f = 440 * 2^((midiNote - 69) / 12)
  static double midiNoteToFrequency(int midiNote) {
    return 440.0 * std::pow(2.0, (midiNote - 69) / 12.0);
  }

private:
  std::unordered_map<char, KeyInfo> map;
};
