/*
 * Drone.cpp
 * ---------
 * Implementation of DroneFlow and DroneInstance.
 * See Drone.h for full documentation.
 */

#include "Drone.h"

// ── DroneFlow ─────────────────────────────────────────────────────────────────

DroneFlow::DroneFlow(int id, int src, int dst, double rate)
    : id(id), source(src), destination(dst), arrivalRate(rate) {}

void DroneFlow::setPath(const std::vector<int>& p) {
    path = p;
}

std::string DroneFlow::pathString(const std::vector<ChargingStation>& stations) const {
    std::ostringstream oss;
    for (size_t i = 0; i < path.size(); ++i) {
        oss << stations[path[i]].name;
        if (i + 1 < path.size()) oss << " -> ";
    }
    return oss.str();
}
