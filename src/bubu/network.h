#pragma once

#include <queue>
#include <string>

// Generic networking stuff to abstract out some of the platform-specific requirements
namespace Network {

  const short int defaultPort = 8080;
  const std::string versionString = "CONTEST API VERSION 1";

  void platformSpecificInitSockets();
  void platformSpecificCloseSocket(int handle);
  bool platformSpecificIsDataAvailableOnSocket(int handle);
}

// Platform header files
#ifdef _CONSOLE

  // Windows
  #include <winsock.h>

#else

  // Linux
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netdb.h>
  #include <unistd.h>

#endif


// Handle reading and writing string-based data through a socket
class SocketIO
{
  int socketHandle_;

  std::vector<char> pendingData_;
  std::queue<std::string> linesReceived_;

  bool echoSend_, echoRecv_;


public:
  SocketIO(int socketHandle);

  void setEchoSendStrings(bool val) { echoSend_ = val; }
  void setEchoRecvStrings(bool val) { echoRecv_ = val; }

  void setSocketHandle(int handle)  { socketHandle_ = handle; }

  void send(std::string const& line);

  bool dataAvailable() const;
  std::string recv();
};
