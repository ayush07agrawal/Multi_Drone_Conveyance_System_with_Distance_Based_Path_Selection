/*
 * Comparator.cpp
 * --------------
 * Implementation of the Comparator class.
 */

#include "Comparator.h"

// ── Helpers ───────────────────────────────────────────────────────────────────

std::string Comparator::pctError(double theory, double sim) {
    if (theory <= 0.0 || theory == INF) return "    N/A";
    double err = std::abs(theory - sim) / theory * 100.0;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << std::setw(6) << err << "%";
    return oss.str();
}

// ── Header ────────────────────────────────────────────────────────────────────

void Comparator::printHeader() {
    const std::string bar(100, '=');
    std::cout << "\n" << bar << "\n"
              << "  Theory vs Simulation Comparison  "
              << "(M/M/1 closed-form  vs  Discrete-Event Simulation)\n"
              << bar << "\n";

    // Column headers
    std::cout << std::left  << std::setw(6)  << "CS"
              << std::right
              << std::setw(7)  << "rho"
              << std::setw(3)  << " | "
              << std::setw(10) << "Wq(theory)"
              << std::setw(10) << "Wq(sim)"
              << std::setw(8)  << "err%"
              << std::setw(3)  << " | "
              << std::setw(10) << "W(theory)"
              << std::setw(10) << "W(sim)"
              << std::setw(8)  << "err%"
              << std::setw(3)  << " | "
              << std::setw(8)  << "Stable"
              << "\n"
              << std::string(100, '-') << "\n";
}

// ── Per-row ───────────────────────────────────────────────────────────────────

void Comparator::printRow(const ChargingStation& s) {
    double thWq = s.isStable() ? s.avgWaitingTime() : INF;
    double thW  = s.isStable() ? s.avgSojournTime() : INF;
    double siWq = s.simAvgWaitingTime();
    double siW  = s.simAvgSojournTime();

    auto fmtVal = [](double v) -> std::string {
        if (v == INF) return "     INF  ";
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(5) << std::setw(10) << v;
        return oss.str();
    };

    std::cout << std::left  << std::setw(6)  << s.name
              << std::right << std::fixed    << std::setprecision(4)
              << std::setw(7)  << s.trafficIntensity()
              << std::setw(3)  << " | "
              << fmtVal(thWq)
              << fmtVal(siWq)
              << std::setw(8)  << pctError(thWq, siWq)
              << std::setw(3)  << " | "
              << fmtVal(thW)
              << fmtVal(siW)
              << std::setw(8)  << pctError(thW, siW)
              << std::setw(3)  << " | "
              << std::setw(8)  << (s.isStable() ? "YES" : "NO (unstable)")
              << "\n";
}

// ── Public entry point ────────────────────────────────────────────────────────

void Comparator::printComparison(const std::vector<ChargingStation>& stations) {
    printHeader();
    for (const auto& s : stations)
        printRow(s);

    // ── Summary ───────────────────────────────────────────────────────────
    std::cout << "\n  Legend:\n"
              << "    rho       - traffic intensity (lambda / mu)\n"
              << "    Wq        - average waiting time in queue\n"
              << "    W         - average sojourn time (Wq + 1/mu)\n"
              << "    err%      - |theory - sim| / theory * 100\n"
              << "  Lower err% indicates better convergence of simulation to theory.\n";
}
