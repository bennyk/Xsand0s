XsAndOs SDK Quick Start Guide - Windows


Note:  The project was created using Microsoft Visual Studio Professional 2015.  If you are running a different version you may need to make adjustments.


Open the “host” solution and build it.

  > start host/host.sln

Launch the host in demonstration mode from a console window:

  > host/x64/Release/host boards/D_Blob.txt –players=0 –interactive

This allows you to step through the frames to see how the predicate rules affect the state of the game.


Now let’s add an automated player:

Open the “player” solution and build it:

  > start player/player.sln

Launch the host in single-player mode:

  > host/x64/Release/host boards/D_Blob.txt –players=1

The host is now waiting for a player to connect.  In a separate terminal window on the same machine:

  > player/x64/Release/player

The host will now be displaying a new frame every second, and the player is submitting moves.  Both processes will exit when the game is complete.

  > host/x64/Release/host boards/D_Blob.txt –players=1

Now let’s run the player against another one on a different machine.  Take note of the IP address of the machine:

  > ipconfig

It will display information about your network connection.  Look for a line similar to this:

   IPv4 Address. . . . . . . . . . . : 10.10.244.37

The default host mode is two player:

  > host/x64/Release/host boards/D_Blob.txt

Start the first player:

  > player/x64/Release/player

Do the steps necessary to build and run a player on a different machine.  From that machine, run the second player and supply it the IP address of the host machine:

  > player/x64/Release/player 10.10.244.37


The default port is 8080.  If you experience conflict with other contestants, supply your own port number to both the host and players:

  > host/x64/Release/host boards/D_Blob.txt –port=8081
  > player/x64/Release/player 10.10.244.37:8081


For a full list of command-line options, run the host executable without any filename on the command-line.

Now, go make your own player using the example code to get started!

