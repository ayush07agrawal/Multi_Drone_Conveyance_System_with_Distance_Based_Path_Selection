/*
 * Simulator.cpp
 * -------------
 * Discrete-Event Simulation engine.
 *
 * DESIGN
 * ------
 * - Each DroneInstance carries timing fields stamped FRESH at every CS hop.
 * - Stats (Wq, W) are recorded per-CS per-hop after the warmup window.
 * - A new external drone is injected only when a drone departs its SOURCE CS,
 *   maintaining the correct inter-arrival Poisson process per flow.
 * - Warmup is based on total SERVICE COMPLETIONS across the whole network,
 *   ensuring all CSs have had enough events before recording begins.
 */

#include "Simulator.h"

Simulator::Simulator(double warmupFraction, int totalArrivals)
    : warmupFraction_(warmupFraction), totalArrivals_(totalArrivals) {}

// ─────────────────────────────────────────────────────────────────────────────
void Simulator::run(std::vector<ChargingStation>& stations,
                    const std::vector<DroneFlow>&  flows,
                    double simTime)
{
    for (auto& s : stations) s.resetSimState();

    using PQ = std::priority_queue<Event, std::vector<Event>, std::greater<Event>>;
    PQ pq;

    int nextDroneId    = 0;
    int freshScheduled = 0;   // post-seed fresh drones injected
    // Warmup: discard stats for the first warmupCount TOTAL service completions
    int totalCompletions = 0;
    int warmupCount      = static_cast<int>(warmupFraction_ * totalArrivals_);
    // Target: collect stats until each CS has ~totalArrivals_ / N samples
    int targetCompletions = totalArrivals_ + warmupCount;

    seedArrivals(pq, flows, nextDroneId, freshScheduled);

    while (!pq.empty()) {
        Event ev = pq.top(); pq.pop();
        if (ev.time > simTime) break;
        if (totalCompletions >= targetCompletions && pq.empty()) break;

        if (ev.type == EventType::ARRIVAL) {
            pq.push(processArrival(ev, stations));
        } else {
            bool record = (totalCompletions >= warmupCount);
            ++totalCompletions;

            for (auto& ne : processDeparture(ev, stations, flows,
                                             nextDroneId, freshScheduled,
                                             targetCompletions, record))
                pq.push(ne);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void Simulator::seedArrivals(
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>>& pq,
    const std::vector<DroneFlow>& flows,
    int& nextDroneId,
    int& freshScheduled)
{
    for (const auto& flow : flows) {
        if (flow.path.empty()) continue;
        DroneInstance inst;
        inst.instanceId    = nextDroneId++;
        inst.flowId        = flow.id;
        inst.remainingPath = flow.path;
        pq.push(Event{ expRandom(flow.arrivalRate),
                       EventType::ARRIVAL, inst, flow.path.front() });
        ++freshScheduled;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Stamp per-hop timing fresh; update CS server clock; return departure event.
Event Simulator::processArrival(Event& ev,
                                 std::vector<ChargingStation>& stations)
{
    double           now = ev.time;
    ChargingStation& cs  = stations[ev.csId];

    ev.drone.arrivalTime   = now;
    double svcStart        = std::max(now, cs.serverFreeAt);
    ev.drone.serviceStart  = svcStart;
    double departure       = svcStart + expRandom(cs.serviceRate);
    ev.drone.departureTime = departure;
    cs.serverFreeAt        = departure;

    return Event{ departure, EventType::DEPARTURE, ev.drone, ev.csId };
}

// ─────────────────────────────────────────────────────────────────────────────
std::vector<Event> Simulator::processDeparture(
    Event& ev,
    std::vector<ChargingStation>& stations,
    const std::vector<DroneFlow>& flows,
    int&  nextDroneId,
    int&  freshScheduled,
    int   targetCompletions,
    bool  record)
{
    std::vector<Event> newEvents;
    double           now  = ev.time;
    int              k    = ev.csId;
    const DroneFlow& flow = flows[ev.drone.flowId];

    // Record per-hop Wq and W (both relative to arrivalTime stamped this hop)
    if (record) {
        stations[k].simStats.record(ev.drone.waitingTime(),
                                    ev.drone.sojournTime());
    }

    // Chain drone to next CS (timing fields zeroed; processArrival fills them)
    if (ev.drone.hasNextHop()) {
        DroneInstance next;
        next.instanceId    = ev.drone.instanceId;
        next.flowId        = ev.drone.flowId;
        next.remainingPath = ev.drone.remainingPath;
        next.remainingPath.erase(next.remainingPath.begin());
        newEvents.push_back(
            Event{ now, EventType::ARRIVAL, next, next.remainingPath.front() });
    }

    // Inject next external drone for this flow (only from source CS departures)
    bool atSource = (k == flow.path.front()
                     && ev.drone.remainingPath.size() == flow.path.size());
    if (atSource && freshScheduled < targetCompletions) {
        DroneInstance fresh;
        fresh.instanceId    = nextDroneId++;
        fresh.flowId        = flow.id;
        fresh.remainingPath = flow.path;
        newEvents.push_back(
            Event{ now + expRandom(flow.arrivalRate),
                   EventType::ARRIVAL, fresh, flow.path.front() });
        ++freshScheduled;
    }

    return newEvents;
}
