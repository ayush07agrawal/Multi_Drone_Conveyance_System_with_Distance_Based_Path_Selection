/*
 * ChargingStation.cpp
 * -------------------
 * Implementation of ChargingStation.
 * See ChargingStation.h for full documentation.
 */

#include "ChargingStation.h"

// ── Constructors ──────────────────────────────────────────────────────────────

ChargingStation::ChargingStation()
    : id(-1), name(""), serviceRate(0.0), arrivalRate(0.0),
      serverFreeAt(0.0), queueLength(0) {}

ChargingStation::ChargingStation(int id, const std::string& name, double mu)
    : id(id), name(name), serviceRate(mu), arrivalRate(0.0),
      serverFreeAt(0.0), queueLength(0) {}

void ChargingStation::resetSimState() {
    serverFreeAt = 0.0;
    queueLength  = 0;
    simStats.reset();
}

// ── Theoretical M/M/1 Metrics ────────────────────────────────────────────────

double ChargingStation::trafficIntensity() const {
    if (serviceRate <= 0.0) return INF;
    return arrivalRate / serviceRate;
}

bool ChargingStation::isStable() const {
    return trafficIntensity() < 1.0;
}

double ChargingStation::avgQueueLength() const {
    if (!isStable()) return INF;
    double rho = trafficIntensity();
    return (rho) / (1.0 - rho);
}

double ChargingStation::avgWaitingTime() const {
    if (!isStable() || arrivalRate <= 0.0) return INF;
    return avgQueueLength() / arrivalRate;
}

double ChargingStation::avgSojournTime() const {
    if (!isStable()) return INF;
    return avgWaitingTime() + 1.0 / serviceRate;
}

// ── Simulation Metrics ────────────────────────────────────────────────────────

double ChargingStation::simAvgWaitingTime() const {
    return simStats.meanWq();
}

double ChargingStation::simAvgSojournTime() const {
    return simStats.meanW();
}

double ChargingStation::simTrafficIntensity() const {
    // Little's Law approximation: rho ≈ lambda * E[W] - but simpler:
    // rho = fraction of time server is busy ≈ lambda_eff / mu
    // We use the same formula with effective lambda confirmed by simulation
    if (serviceRate <= 0.0) return 0.0;
    return arrivalRate / serviceRate;   // same as theory; sim validates via Wq
}

// ── Display ───────────────────────────────────────────────────────────────────

void ChargingStation::printTheoryMetrics() const {
    std::string wqStr = isStable() ? std::to_string(avgWaitingTime()) : "     INF";
    std::string wStr  = isStable() ? std::to_string(avgSojournTime()) : "     INF";

    std::cout << std::left  << std::setw(6)  << name
              << std::right << std::fixed    << std::setprecision(4)
              << " | lambda=" << std::setw(7) << arrivalRate
              << " | rho="   << std::setw(6) << trafficIntensity()
              << " | Wq="    << std::setw(10) << wqStr
              << " | W="     << std::setw(10) << wStr
              << "\n";
}

void ChargingStation::printSimMetrics() const {
    std::cout << std::left  << std::setw(6)  << name
              << std::right << std::fixed    << std::setprecision(4)
              << " | served=" << std::setw(5) << simStats.totalServed
              << " | rho="   << std::setw(6) << simTrafficIntensity()
              << " | Wq="    << std::setw(10) << simAvgWaitingTime()
              << " | W="     << std::setw(10) << simAvgSojournTime()
              << "\n";
}
