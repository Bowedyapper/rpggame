#include <future>
#include <iostream>
int ptime = clock();    // Time since last frame
int offset = 0;       // Time since last physics tick

int tickRate = 1000 / 60;

// Calculate delta time
int deltaTime(int previous, int offset) {
    int Δt = (clock() - previous);  // Δt = Current time - time of last frame
    return Δt;
}

void game_physics_loop() {
    while (true) {
        _sleep(tickRate);
        // Calculate delta time
        int delta = deltaTime(ptime, offset);
        ptime = clock();
        offset = delta % (1000 / tickRate);
        ptime = clock(); // Current time becomes time of last frame
        std::cout << text << delta << std::endl;
    }
}

int main() {
    auto physLoop = std::async(std::launch::async, game_physics_loop);
    while (true) {
        int k = 1;
        int l = 2;
        _sleep(100);
        int m = k + l;
    }
}