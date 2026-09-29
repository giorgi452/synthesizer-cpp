#pragma once

#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

class TerminalManager {
public:
  TerminalManager();
  ~TerminalManager();

  void enableRawMode();
  void disableRawMode();

  void kbhit();
  void getChar();

private:
  struct termios orig_termios;
  bool rawModeEnabled{false};
};
