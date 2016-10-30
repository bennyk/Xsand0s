#include <stdio.h>
#include <iostream>

#include "platform.h"

#ifdef _CONSOLE
  // Windows

void Platform::clearScreen()
{
  // Believe it or not, Microsoft actually suggests this method:
  system("cls");
}


void Platform::disableLineBuffering()
{
  // Nothing to do
}


char Platform::getCharNoBuffering()
{
  return _getch();
}


#else
  // Linux

void Platform::clearScreen()
{
  // use the ANSI command
  std::cout << "\x1b[2J\x1b[H";
}


void Platform::disableLineBuffering()
{
  struct termios t;
  tcgetattr(STDIN_FILENO, &t);
  t.c_lflag &= ~ICANON;
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}


char Platform::getCharNoBuffering()
{
  return getchar();
}

#endif
