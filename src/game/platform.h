#pragma once

#include <queue>
#include <string>

// Abstraction layer for platform-specific functionality
namespace Platform {

  void clearScreen();
  void disableLineBuffering();
  char getCharNoBuffering();

  bool doTextColorCodesWork();
}

// Platform header files
#ifdef _CONSOLE

  // Windows
  #include <conio.h>

  inline bool Platform::doTextColorCodesWork() { return false; }

#else

  // Linux
  #include <unistd.h>
  #include <termios.h>

  inline bool Platform::doTextColorCodesWork() { return true; }

#endif
