 /*
 * MultiDroneSystem.cpp
 * --------------------
 * Implementation of the MultiDroneSystem orchestrator.
 */

#include "MultiDroneSystem.h"

// ── Constructor ───────────────────────────────────────────────────────────────

MultiDroneSystem::MultiDroneSystem(int N) : graph(N) {
    stations.resize(N);
}

// ── Network builders ──────────────────────────────────────────────────────────

void MultiDroneSystem::addStation(int id, const std::string& name, double mu) {
    if (id < 0 || id >= static_cast<int>(stations.size()))
        throw std::out_of_range("Station id " + std::to_string(id) + " out of range.");
    stations[id] = ChargingStation(id, name, mu);
}

void MultiDroneSystem::addEdge(int u, int v, double distance) {
    graph.addEdge(u, v, distance);
}

void MultiDroneSystem::addDemand(int src, int dst, double lambda) {
    int id = static_cast<int>(flows.size());
    flows.emplace_back(id, src, dst, lambda);
}

// ── Routing & Theory ─────────────────────────────────────────────────────────

void MultiDroneSystem::computeShortestPaths() {
    for (DroneFlow& flow : flows) {
        auto [dist, prev] = graph.dijkstra(flow.source);
        flow.setPath(graph.reconstructPath(prev, flow.source, flow.destination));
    }
}

void MultiDroneSystem::computeArrivalRates() {
    for (ChargingStation& s : stations) s.arrivalRate = 0.0;

    for (const DroneFlow& flow : flows) {
        if (flow.path.empty()) {
            std::cerr << "[WARN] No path for flow " << flow.id << "\n";
            continue;
        }
        for (int csId : flow.path)
            stations[csId].arrivalRate += flow.arrivalRate;
    }
}

// ── Display helpers ───────────────────────────────────────────────────────────

void MultiDroneSystem::printHeader(const std::string& title) const {
    std::cout << "\n" << std::string(70, '=') << "\n"
              << "  " << title << "\n"
              << std::string(70, '=') << "\n";
}

void MultiDroneSystem::printGraph() const {
    printHeader("Network Topology");
    for (int u = 0; u < graph.N; ++u) {
        std::cout << "  " << stations[u].name
                  << " (mu=" << stations[u].serviceRate << ")  -->  ";
        for (const Edge& e : graph.adj[u])
            std::cout << stations[e.to].name << "(d=" << e.weight << ")  ";
        std::cout << "\n";
    }
}

void MultiDroneSystem::printPaths() const {
    printHeader("Drone Flows & Shortest Paths");
    std::cout << std::left
              << std::setw(8)  << "Flow"
              << std::setw(8)  << "Src"
              << std::setw(8)  << "Dst"
              << std::setw(10) << "Lambda"
              << "Path\n"
              << std::string(70, '-') << "\n";

    for (const DroneFlow& f : flows) {
        std::cout << std::left
                  << std::setw(8)  << ("F" + std::to_string(f.id))
                  << std::setw(8)  << stations[f.source].name
                  << std::setw(8)  << stations[f.destination].name
                  << std::setw(10) << f.arrivalRate
                  << (f.path.empty() ? "[UNREACHABLE]" : f.pathString(stations))
                  << "\n";
    }
}

void MultiDroneSystem::printTheoryTable() const {
    printHeader("Theoretical Results (M/M/1 Closed-Form)");
    std::cout << std::left  << std::setw(6)  << "CS"
              << std::right
              << std::setw(10) << "lambda_eff"
              << std::setw(8)  << "rho"
              << std::setw(8)  << "Stable"
              << std::setw(12) << "Wq"
              << std::setw(12) << "W"
              << "\n" << std::string(70, '-') << "\n";

    for (const ChargingStation& s : stations) {
        std::cout << std::left << std::setw(6) << s.name
                  << std::right << std::fixed << std::setprecision(4)
                  << std::setw(10) << s.arrivalRate
                  << std::setw(8)  << s.trafficIntensity()
                  << std::setw(8)  << (s.isStable() ? "YES" : "NO")
                  << std::setw(12) << (s.isStable() ? s.avgWaitingTime() : INF)
                  << std::setw(12) << (s.isStable() ? s.avgSojournTime() : INF)
                  << "\n";
    }
}

// ── Entry Point ───────────────────────────────────────────────────────────────

void MultiDroneSystem::run(int totalArrivals) {
    // ── Step 1: routing + theory ─────────────────────────────────────────
    computeShortestPaths();
    computeArrivalRates();

    printGraph();
    printPaths();
    printTheoryTable();

    // ── Step 2: simulation ───────────────────────────────────────────────
    printHeader("Running Discrete-Event Simulation ...");
    std::cout << "  Total arrivals to simulate : " << totalArrivals << "\n"
              << "  Warm-up fraction           : 10%\n"
              << "  RNG seed                   : 42 (fixed for reproducibility)\n";

    Simulator sim(0.10, totalArrivals);
    sim.run(stations, flows);

    std::cout << "  Simulation complete.\n";

    // ── Step 3: comparison ───────────────────────────────────────────────
    Comparator::printComparison(stations);

    // ── Step 4: bottleneck summary ───────────────────────────────────────
    printHeader("System Summary");
    int    unstable   = 0;
    double maxRho     = 0.0;
    std::string bottleneck;
    for (const ChargingStation& s : stations) {
        if (!s.isStable()) ++unstable;
        if (s.trafficIntensity() > maxRho) {
            maxRho     = s.trafficIntensity();
            bottleneck = s.name;
        }
    }
    std::cout << "  Charging stations : " << stations.size() << "\n"
              << "  Demand flows      : " << flows.size()    << "\n"
              << "  Unstable stations : " << unstable        << "\n"
              << "  Bottleneck        : " << bottleneck
              << "  (rho = " << std::fixed << std::setprecision(4) << maxRho << ")\n";
}
