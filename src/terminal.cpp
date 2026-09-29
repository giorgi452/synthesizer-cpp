#include "terminal.h"
#include <cstdlib>
#include <iostream>

TerminalManager::TerminalManager() { enableRawMode(); }

TerminalManager::~TerminalManager() { disableRawMode(); }

void TerminalManager::enableRawMode() {
  if (rawModeEnabled)
    return;

  // Save original terminal attributes
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
    std::cerr << "Error getting terminal attributes" << std::endl;
    return;
  }

  struct termios raw = orig_termios;

  // Disable Canonical Mode and Key Echo
  raw.c_lflag &= ~(ECHO | ICANON);

  raw.c_cc[VMIN] = 0;  // Return immediately with whatever is available
  raw.c_cc[VTIME] = 0; // No timeout delay

  // Apply modified attributes
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    std::cerr << "Error setting raw terminal mode" << std::endl;
    return;
  }

  rawModeEnabled = true;
}
