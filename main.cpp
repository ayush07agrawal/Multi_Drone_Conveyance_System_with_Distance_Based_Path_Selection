/*
 * main.cpp
 * --------
 * Entry point for the Multi-Drone Conveyance System.
 *
 * Compile (C++17):
 *   g++ -std=c++17 -O2 -o drone_sim \
 *       main.cpp ChargingStation.cpp Drone.cpp Graph.cpp \
 *       Simulator.cpp MultiDroneSystem.cpp Comparator.cpp Scenarios.cpp
 *
 * Windows (MinGW):
 *   mingw32-make
 */

#include "Scenarios.h"
#include <iostream>
#include <stdexcept>

int main() {
    std::cout << "================================================================\n"
              << "  Multi-Drone Conveyance System — Theory vs Simulation\n"
              << "  M/M/1 Closed-Form  |  Discrete-Event Simulation\n"
              << "================================================================\n\n"
              << "  [1] Built-in example  (8 stations, 6 flows, 50000 arrivals)\n"
              << "  [2] Built-in example  (high accuracy — 200000 arrivals)\n"
              << "  [3] Interactive — enter custom topology\n\n"
              << "Choice: ";

    int choice;
    std::cin >> choice;

    try {
        switch (choice) {
            case 1: runExampleScenario(50000);  break;
            case 2: runExampleScenario(200000); break;
            case 3: runInteractiveMode();        break;
            default:
                std::cout << "Invalid choice.\n";
                return 1;
        }
    } catch (const std::exception& ex) {
        std::cerr << "\n[ERROR] " << ex.what() << "\n";
        return 1;
    }

    std::cout << "\n[Done]\n";
    return 0;
}
