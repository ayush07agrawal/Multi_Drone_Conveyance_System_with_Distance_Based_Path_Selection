#pragma once

/*
 * Drone.h
 * -------
 * Two related concepts live here:
 *
 * 1. DroneFlow  — a traffic demand (src, dst, lambda).
 *                 Used to compute shortest paths and accumulate
 *                 arrival rates at charging stations (theoretical side).
 *
 * 2. DroneInstance — a single physical drone in the discrete-event
 *                    simulation.  Carries timing fields that are filled
 *                    in as the drone progresses through the network:
 *                      arrivalTime    — when it arrives at a CS
 *                      serviceStart   — when charging begins
 *                      departureTime  — when charging ends
 *                      waitingTime    — serviceStart - arrivalTime  (Wq)
 *                      sojournTime    — departureTime - arrivalTime (W)
 *                    and a reference to its remaining path (CS ids still
 *                    to visit).
 */

#include "common.h"
#include "ChargingStation.h"

// ── DroneFlow ─────────────────────────────────────────────────────────────────
// Represents a stream of drones from src to dst at rate lambda.
// One DroneFlow spawns many DroneInstances during simulation.
class DroneFlow {
public:
    int              id;
    int              source;
    int              destination;
    double           arrivalRate;    // external Poisson lambda for this flow
    std::vector<int> path;           // shortest-path CS ids (set by router)

    DroneFlow(int id, int src, int dst, double rate);

    void setPath(const std::vector<int>& p);

    // Human-readable path string, e.g.  CS0 -> CS2 -> CS5
    std::string pathString(const std::vector<ChargingStation>& stations) const;
};

// ── DroneInstance ─────────────────────────────────────────────────────────────
// A single drone travelling through the network in the simulation.
struct DroneInstance {
    int    instanceId;      // unique drone id within the simulation
    int    flowId;          // which DroneFlow spawned this drone
    std::vector<int> remainingPath;  // CS ids still to visit (front = next CS)

    // Per-hop timing (filled in by Simulator as the drone visits each CS)
    double arrivalTime   {0.0};
    double serviceStart  {0.0};
    double departureTime {0.0};

    double waitingTime()  const { return serviceStart  - arrivalTime;   }
    double sojournTime()  const { return departureTime - arrivalTime;   }

    // True if there are more charging stations to visit after the current one
    bool hasNextHop() const { return remainingPath.size() > 1; }

    // Pop the front CS id (the one just visited) and return the next
    int advanceToNextHop() {
        remainingPath.erase(remainingPath.begin());
        return remainingPath.front();
    }

    int currentCS() const { return remainingPath.front(); }
};
