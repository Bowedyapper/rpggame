# rpggame
A basic multiplayer RPG game I am attempting to make in C / C++

Built with SDL2 and Socket.io

---

## Compiling on unix systems
Clone this repo and then run
> ``` git submodule update --init --recursive```

This will download the submodules needed to build the program.

Install required dependencies (Linux)
> ```sudo apt install g++ make cmake libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev```

Install required dependencies (Mac)
> ```brew install cmake make SDL2 SDL2_image SDL2_ttf```

Navigate to the include/socket.io-client folder and run
> ```cmake .```

Once finished run
> ```sudo make install```

This will install the header files and libs to /usr/local/

Now you should be able to build the program by running
> ```cmake -B build .```

then cd into the build directory and run
> ``` make```

and finally run the program
> ``` ./RPGGame```

from the root of the directory

---

## Compiling on windows

I'll deal with this later :S