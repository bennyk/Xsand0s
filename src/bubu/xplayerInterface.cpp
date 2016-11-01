#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <thread>
#include <cassert>

#include "network.h"
#include "xplayerInterface.h"


XPlayerInterface::XPlayerInterface() :
        playerAssignment_(Occupation_Total),
        socketHandle_(-1),
        io_(NULL),
        playerIsActive_(false),
        verbose_(false)
{}


XPlayerInterface::~XPlayerInterface()
{
    delete io_;
    if (socketHandle_ >= 0)
        Network::platformSpecificCloseSocket(socketHandle_);
}


int XPlayerInterface::playerProgramEntryPoint(int argc, char* argv[])
{
    // Socket connection to game host

    Network::platformSpecificInitSockets();

    std::string hostname = "localhost";
    std::string port = "8080";
    if (argc >= 2)
    {
        hostname = argv[1];
        size_t portDelimiter = hostname.find_first_of(':');
        if (portDelimiter != std::string::npos)
        {
            port = hostname.substr(portDelimiter + 1);
            hostname = hostname.substr(0, portDelimiter);
        }
    }

    std::cout << "Requesting connection to " << hostname.c_str() << ":" << port.c_str() << std::endl;
    hostent* hostEntity = gethostbyname(hostname.c_str());
    if (!hostEntity)
    {
        std::cerr << "DNS lookup failed" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Usage:  [server_address]" << std::endl;
        std::cerr << "        will use localhost if nothing specified" << std::endl;
        std::cerr << std::endl;
        return 2;
    }

    socketHandle_ = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (socketHandle_ < 0)
    {
        std::cerr << "Unable to create a socket" << std::endl;
        return 2;
    }

    sockaddr_in addr = {0};
    std::memcpy(&addr.sin_addr, hostEntity->h_addr, hostEntity->h_length);
    addr.sin_port = atoi(port.c_str());
    addr.sin_family = AF_INET;

    std::cout << "Attempting to connect to host..." << std::endl;
    if (connect(socketHandle_, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Unable to connect with socket" << std::endl;
        return 2;
    }

    std::cout << "Socket connection established." << std::endl;

    io_ = new SocketIO(socketHandle_);
    io_->setEchoRecvStrings(verbose_);
    io_->setEchoSendStrings(verbose_);

    // Tell the host who we are
    io_->send(Network::versionString);
    io_->send(argv[0]);

    // Get game information from the host
    std::vector<std::string> boardData;
    for (;;)
    {
        std::string line = io_->recv();
        if (line.empty())
            return 3;
        if (line.find_first_of("BOARD:") == 0)
        {
            boardData.push_back(line.substr(6));
        }
        if (line.find_first_of("PLAYER:") == 0)
        {
            // Player assignment marks the end of the board dump
            playerAssignment_ = char_to_occupation(line[7]);
            if (playerAssignment_ >= Occupation_Total)
            {
                std::cerr << "Unrecognized player assignment: " << line[7] << std::endl;
                return false;
            }
            std::cout << "Player assignment is " << occupation_to_char(playerAssignment_) << std::endl;
            break;
        }
    }

    // Load the game
    if (!game_.loadLines(boardData))
    {
        std::cerr << "Error loading board" << std::endl;
        return 1;
    }

    std::cout << std::endl;
    std::cout << "Game loaded!" << std::endl;
    std::cout << std::endl;

    // Spool up a thread to handle networking
    playerIsActive_ = true;
    std::thread listenThread(hostLiaisonThreadEntryPoint, this);

    // Play!
    playGame();

    // Clean up
    playerIsActive_ = false;
    listenThread.join();
    return 0;
}


void XPlayerInterface::makeAMove(Move const& move)
{
    // Once play begins, this is the only place where we send data to the host
//  const size_t frameNum = getGame().getBoards().size() - 1;
    const int frameNum = getGame().current_frame_index();
    std::stringstream moveStream;
    moveStream << "MOVE" << " " << frameNum <<  " " << move.x << " " << move.y;
    io_->send(moveStream.str());
}


void XPlayerInterface::hostLiaison()
{
    while (playerIsActive_ && !game_.is_over())
    {
        // Wait for data from the  host
        std::string line = io_->recv();
        if (line.empty())
        {
            // There was a receive problem
            exit(0);
        }

        // Look for move notifications; anything else is unexpected
        std::stringstream ss(line);
        std::string command;
        ss >> command;
        if (command != "MOVES")
            break;

        // Check the frame number
        const int expectedFrame = getGame().current_frame_index();
        int frameNum = -1;
        ss >> frameNum;
        if (verbose_)
            std::cout << "Got move notification for frame " << frameNum << std::endl;
        if (frameNum != expectedFrame)
        {
            std::cerr << "Frame mismatch!  Host sent moves for frame " << frameNum << ", we expected frame " << expectedFrame << std::endl;
            return;
        }

        // Parse the individual moves
        std::vector<Move> moves;
        for (;;)
        {
            int x = -1, y = -1;
            ss >> x >> y;

            // Save only the valid moves
            auto current_frame = game_.current_frame();
            if (x < 0 || x >= current_frame->xsize() || y < 0 || y >= current_frame->ysize())
                break;

            if (verbose_)
                std::cout << "  Got move (" << x << ", " << y << ") --> " << std::endl;
            moves.push_back(Move(x, y));
        }

        assert(moves.size() <= 2);

        // Apply the moves
        game_.apply_moves_and_generate_next_frame(moves);
        nextFrame();
    }
}
