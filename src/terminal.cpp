#include "terminal.h"
#include <cstdlib>
#include <iostream>
#include <sys/select.h>

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

void TerminalManager::disableRawMode() {
  if (!rawModeEnabled)
    return;

  // Restore original terminal attributes
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
  rawModeEnabled = false;
}

bool TerminalManager::kbhit() {
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  // Non-blocking select check on stdin with zero timeout
  int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
  return (result > 0 && FD_ISSET(STDIN_FILENO, &readfds));
}

char TerminalManager::getChar() {
  char ch = 0;
  if (read(STDIN_FILENO, &ch, 1) < 0) {
    return 0;
  }
  return ch;
}
