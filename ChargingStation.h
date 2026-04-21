#pragma once

/*
 * ChargingStation.h
 * -----------------
 * Models a single charging station as an M/M/1 queue.
 *
 * Dual-mode design:
 *   THEORETICAL  — closed-form M/M/1 formulas (rho, Lq, Wq, W)
 *   SIMULATION   — maintains a real FIFO queue of drone ids and a
 *                  "server busy until" timestamp; collects per-drone
 *                  waiting and sojourn samples that are averaged at
 *                  the end of the simulation run.
 *
 * M/M/1 formulas:
 *   rho = lambda / mu              traffic intensity
 *   Lq  = rho^2 / (1 - rho)       avg queue length
 *   Wq  = Lq / lambda              avg waiting time in queue  (Little's Law)
 *   W   = Wq + 1/mu               avg sojourn time
 */

#include "common.h"

// ── SimStats ─────────────────────────────────────────────────────────────────
// Collected during one simulation run; reset between runs.
struct SimStats {
    std::vector<double> waitingTimes;   // Wq sample per drone
    std::vector<double> sojournTimes;   // W  sample per drone
    int                 totalServed{0};

    void record(double wq, double w) {
        waitingTimes.push_back(wq);
        sojournTimes.push_back(w);
        ++totalServed;
    }

    void reset() {
        waitingTimes.clear();
        sojournTimes.clear();
        totalServed = 0;
    }

    double meanWq() const {
        if (waitingTimes.empty()) return 0.0;
        return std::accumulate(waitingTimes.begin(), waitingTimes.end(), 0.0)
               / waitingTimes.size();
    }

    double meanW() const {
        if (sojournTimes.empty()) return 0.0;
        return std::accumulate(sojournTimes.begin(), sojournTimes.end(), 0.0)
               / sojournTimes.size();
    }
};

// ── ChargingStation ───────────────────────────────────────────────────────────
class ChargingStation {
public:
    int         id;
    std::string name;
    double      serviceRate;   // mu  — drones serviced per unit time
    double      arrivalRate;   // lambda_eff — set by routing analysis

    // ── Simulation state (reset before each run) ──────────────────────────
    double serverFreeAt;       // simulation clock time when server next free
    int    queueLength;        // current number waiting (excluding in service)
    SimStats simStats;         // collected samples

    // ── Constructors ──────────────────────────────────────────────────────
    ChargingStation();
    ChargingStation(int id, const std::string& name, double mu);

    // Reset simulation state before a new run
    void resetSimState();

    // ── Theoretical M/M/1 metrics ─────────────────────────────────────────
    double trafficIntensity()  const;   // rho  = lambda / mu
    bool   isStable()          const;   // rho  < 1
    double avgQueueLength()    const;   // Lq   = rho^2 / (1 - rho)
    double avgWaitingTime()    const;   // Wq   = Lq / lambda
    double avgSojournTime()    const;   // W    = Wq + 1/mu

    // ── Simulation metrics (call after run) ───────────────────────────────
    double simTrafficIntensity() const; // rho_sim = lambda * meanW  (approx)
    double simAvgWaitingTime()   const; // mean of collected Wq samples
    double simAvgSojournTime()   const; // mean of collected W  samples

    // ── Display ───────────────────────────────────────────────────────────
    void printTheoryMetrics() const;
    void printSimMetrics()    const;
};
