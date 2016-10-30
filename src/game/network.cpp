#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>

#include "network.h"

// Platform-specific handling
#ifdef _CONSOLE

  // Windows

  // Pull in the library
  #pragma comment(lib, "ws2_32.lib")

  void Network::platformSpecificInitSockets()
  {
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(1, 1), &wsaData);
    if (err)
    {
      std::cerr << "Unable to initialize socket library" << std::endl;
      exit(err);
    }
  }

  void Network::platformSpecificCloseSocket(int handle)
  {
    closesocket(handle);
  }

  bool Network::platformSpecificIsDataAvailableOnSocket(int handle)
  {
    unsigned long bytes_available = 0;
    ioctlsocket(handle, FIONREAD, &bytes_available);
    return bytes_available != 0;
  }

#else

  // Linux

  void Network::platformSpecificInitSockets() {}
  void Network::platformSpecificCloseSocket(int handle)  { close(handle); }

  bool Network::platformSpecificIsDataAvailableOnSocket(int handle)
  {
    char tinyBuffer;
    return (::recv(handle, &tinyBuffer, sizeof(tinyBuffer), MSG_DONTWAIT | MSG_PEEK) > 0);
  }

#endif



SocketIO::SocketIO(int socketHandle) :
  socketHandle_(socketHandle),
  echoSend_(false),
  echoRecv_(false)
{}


void SocketIO::send(std::string const& line)
{
  if (echoSend_)
    std::cout << "Sending \"" << line << "\"" << std::endl;
  ::send(socketHandle_, line.c_str(), (int)line.size() + 1, 0);
}


bool SocketIO::dataAvailable() const
{
  // Anything in the cache?
  if (!linesReceived_.empty())
    return true;

  // Anything on the line?
  return Network::platformSpecificIsDataAvailableOnSocket(socketHandle_);
}


std::string SocketIO::recv()
{
  for (;;)
  {
    // Check our line cache
    if (!linesReceived_.empty())
    {
      std::string line = linesReceived_.front();
      linesReceived_.pop();
      if (echoRecv_)
        std::cout << "Consuming \"" << line << "\"" << std::endl;
      return line;
    }

    // Read from the stream
    size_t bufferSize = 4096;
    size_t endOfBuffer = pendingData_.size();
    pendingData_.resize(endOfBuffer + bufferSize);
    int result = ::recv(socketHandle_, &pendingData_[endOfBuffer], (int)bufferSize, 0);
    if (result <= 0)
    {
      // The connection was closed or there was an error
      std::cerr << "recv failed" << std::endl;
      return "";
    }
    if (echoRecv_)
      std::cout << "recv'd " << result << " bytes" << std::endl;

    // Remove the excess we didn't use
    pendingData_.resize(endOfBuffer + (size_t)result);

    // Split the data into lines (NULL delimiters)
    for (;;)
    {
      std::vector<char>::iterator terminatorIt = std::find(pendingData_.begin(), pendingData_.end(), '\0');
      if (terminatorIt == pendingData_.end())
        break;

      // Extract the string
      linesReceived_.push(std::string(&pendingData_[0]));
      ++terminatorIt;
      pendingData_.erase(pendingData_.begin(), terminatorIt);
      if (echoRecv_)
        std::cout << "Received string \"" << linesReceived_.back() << "\"" << std::endl;
    }
  }
}
