XsAndOs SDK Quick Start Guide -- EC Linux environment


** Important: Make sure you are running a SLES 11 terminal.  Older versions of the OS are not supported. **


From the root directory of the SDK, run “make”.  It will recursively build the “host” and “player” executables.

  % make


Launch the host in demonstration mode:

  % host/build/host boards/D_Blob.txt -players=0 -interactive

This allows you to step through the frames to see how the predicate rules affect the state of the game.

(If the board does not fit in your xterm window, you can access the font menu with CTRL+right_mouse_click.)


Now let’s add an automated player:

  % host/build/host boards/D_Blob.txt -players=1

The host is now waiting for a player to connect.  In a separate terminal window on the same machine:

  % player/build/player

The host is displaying a new frame every second, and the player is submitting moves.  Both processes will exit when the game is complete.


Now let’s run the player against another one on a different machine.  The default host mode is two player:

  % host/build/host boards/D_Blob.txt

Start the first player:

  % player/build/player

Open up another terminal on a different machine.  Take note of the machine the host was launched.  For instance, dlxc1597.  Supply that name to the player:

  % player/build/player dlxc1597

For machines in different locations, you may need the fully-qualified name:

  % player/build/player dlxc1597.pdx.intel.com


The default port is 8080.  If you experience conflict with other contestants, supply your own port number to both the host and players:

  % host/build/host boards/D_Blob.txt -port=8081
  % player/build/player dlxc1597.pdx.intel.com:8081


For a full list of command-line options, run the host executable without any filename on the command-line.

Now, go make your own player using the example code to get started!

