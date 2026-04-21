/*
 * Scenarios.cpp
 * -------------
 * Built-in 8-station example and interactive input mode.
 *
 * Network layout:
 *
 *   CS0 --2-- CS1 --3-- CS2 --2-- CS3
 *    |          |          |          |
 *    4          3          3          4
 *    |          |          |          |
 *   CS4 --2-- CS5 --3-- CS6 --2-- CS7
 *
 * Diagonals: CS4--5--CS1, CS5--4--CS2
 *
 * CS2 and CS6 lie on most shortest paths → natural bottlenecks.
 */

#include "Scenarios.h"
#include "MultiDroneSystem.h"

// ── Example Scenario ─────────────────────────────────────────────────────────

void runExampleScenario(int totalArrivals) {
    const int    N  = 8;
    const double mu = 5.0;

    MultiDroneSystem sys(N);

    sys.addStation(0, "CS0", mu);
    sys.addStation(1, "CS1", mu);
    sys.addStation(2, "CS2", mu);
    sys.addStation(3, "CS3", mu);
    sys.addStation(4, "CS4", mu);
    sys.addStation(5, "CS5", mu);
    sys.addStation(6, "CS6", mu);
    sys.addStation(7, "CS7", mu);

    // Top row
    sys.addEdge(0, 1, 2.0);
    sys.addEdge(1, 2, 3.0);
    sys.addEdge(2, 3, 2.0);
    // Bottom row
    sys.addEdge(4, 5, 2.0);
    sys.addEdge(5, 6, 3.0);
    sys.addEdge(6, 7, 2.0);
    // Verticals
    sys.addEdge(0, 4, 4.0);
    sys.addEdge(1, 5, 3.0);
    sys.addEdge(2, 6, 3.0);
    sys.addEdge(3, 7, 4.0);
    // Diagonals
    sys.addEdge(4, 1, 5.0);
    sys.addEdge(5, 2, 4.0);

    sys.addDemand(0, 7, 1.0);
    sys.addDemand(4, 3, 1.0);
    sys.addDemand(0, 6, 0.8);
    sys.addDemand(4, 7, 0.8);
    sys.addDemand(1, 7, 0.6);
    sys.addDemand(0, 3, 0.6);

    sys.run(totalArrivals);
}

// ── Interactive Mode ──────────────────────────────────────────────────────────

void runInteractiveMode() {
    int N;
    std::cout << "\nEnter number of charging stations: ";
    std::cin >> N;

    double mu;
    std::cout << "Enter service rate (mu) for all stations: ";
    std::cin >> mu;

    MultiDroneSystem sys(N);
    for (int i = 0; i < N; ++i)
        sys.addStation(i, "CS" + std::to_string(i), mu);

    int E;
    std::cout << "Enter number of edges: ";
    std::cin >> E;
    std::cout << "Enter each edge:  u  v  distance  (0-indexed)\n";
    for (int i = 0; i < E; ++i) {
        int u, v; double d;
        std::cin >> u >> v >> d;
        sys.addEdge(u, v, d);
    }

    int D;
    std::cout << "Enter number of demand pairs: ";
    std::cin >> D;
    std::cout << "Enter each demand:  src  dst  lambda\n";
    for (int i = 0; i < D; ++i) {
        int s, t; double lam;
        std::cin >> s >> t >> lam;
        sys.addDemand(s, t, lam);
    }

    int totalArrivals;
    std::cout << "Enter total arrivals to simulate (e.g. 50000): ";
    std::cin >> totalArrivals;

    sys.run(totalArrivals);
}
