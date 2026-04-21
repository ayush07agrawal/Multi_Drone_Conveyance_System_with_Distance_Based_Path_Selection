#pragma once

/*
 * MultiDroneSystem.h
 * ------------------
 * Top-level orchestrator.  Owns:
 *   - The network Graph
 *   - All ChargingStations
 *   - All DroneFlow demand pairs
 *
 * Responsibilities:
 *   1. Route each DroneFlow via Dijkstra (shortest-distance path)
 *   2. Compute theoretical M/M/1 metrics (effective arrival rates → formulas)
 *   3. Drive the Simulator to collect empirical metrics
 *   4. Delegate comparison display to Comparator
 */

#include "common.h"
#include "ChargingStation.h"
#include "Drone.h"
#include "Graph.h"
#include "Simulator.h"
#include "Comparator.h"

class MultiDroneSystem {
private:
    std::vector<ChargingStation> stations;
    Graph                        graph;
    std::vector<DroneFlow>       flows;

    // ── Routing & theory ─────────────────────────────────────────────────
    void computeShortestPaths();
    void computeArrivalRates();

    // ── Display helpers ──────────────────────────────────────────────────
    void printHeader(const std::string& title) const;
    void printGraph()        const;
    void printPaths()        const;
    void printTheoryTable()  const;

public:
    explicit MultiDroneSystem(int N);

    // ── Network builders ─────────────────────────────────────────────────
    void addStation(int id, const std::string& name, double mu);
    void addEdge(int u, int v, double distance);
    void addDemand(int src, int dst, double lambda);

    /*
     * run()
     *   totalArrivals — number of drone arrivals to simulate (post warm-up)
     *                   Higher → slower but more accurate simulation results.
     *                   Default: 50 000
     */
    void run(int totalArrivals = 50000);
};
