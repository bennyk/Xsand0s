
I have been testing this source on CMake3.6 on my home PC. Hopefully it will work the same on Intel SLES11.

cmake_minimum_required(VERSION 3.6)

1. unpack the tar ball to bubu/
2. chdir to bubu/src/bubu 
3. Type "cmake ."
4. make
5. Run the client executable "bubu-player"

The client sometimes hung-up on exit. If so Ctrl+C on exit. The thread join() API does not work properly.

