#pragma once

/*
 * Simulator.h
 * -----------
 * Discrete-Event Simulation (DES) engine for the multi-drone network.
 *
 * Event types:
 *   ARRIVAL    — DroneInstance arrives at a ChargingStation
 *   DEPARTURE  — DroneInstance finishes charging and leaves
 *
 * Timing model:
 *   arrivalTime / serviceStart / departureTime are stamped FRESH at
 *   every CS hop by processArrival(), giving independent per-CS samples.
 *
 * Warmup:
 *   Stats are discarded for the first (warmupFraction * totalArrivals)
 *   total service completions across the network.
 */

#include "common.h"
#include "ChargingStation.h"
#include "Drone.h"

enum class EventType { ARRIVAL, DEPARTURE };

struct Event {
    double        time;
    EventType     type;
    DroneInstance drone;
    int           csId;
    bool operator>(const Event& o) const { return time > o.time; }
};

class Simulator {
public:
    explicit Simulator(double warmupFraction = 0.10, int totalArrivals = 50000);

    void run(std::vector<ChargingStation>& stations, const std::vector<DroneFlow>& flows, double simTime = 1e9);

private:
    double warmupFraction_;
    int    totalArrivals_;

    void seedArrivals(
        std::priority_queue<Event, std::vector<Event>, std::greater<Event>>& pq,
        const std::vector<DroneFlow>& flows,
        int& nextDroneId,
        int& freshScheduled);

    Event processArrival(Event& ev,
                         std::vector<ChargingStation>& stations);

    std::vector<Event> processDeparture(
        Event& ev,
        std::vector<ChargingStation>& stations,
        const std::vector<DroneFlow>& flows,
        int&  nextDroneId,
        int&  freshScheduled,
        int   targetCompletions,
        bool  record);
};
