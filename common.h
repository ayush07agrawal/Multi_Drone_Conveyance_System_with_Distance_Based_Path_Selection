#pragma once

/*
 * common.h
 * --------
 * Shared constants, standard-library includes, and the project-wide
 * random-number generator used by both the theoretical and simulation modules.
 */

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <deque>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <random>
#include <functional>
#include <numeric>
#include <map>

// ── Infinity shorthand ────────────────────────────────────────────────────────
constexpr double INF = std::numeric_limits<double>::infinity();

// ── Global RNG (seeded once in main) ─────────────────────────────────────────
inline std::mt19937& globalRng() {
    static std::mt19937 rng(42);   // fixed seed for reproducibility
    return rng;
}

// Exponential variate with rate `rate`
inline double expRandom(double rate) {
    std::exponential_distribution<double> dist(rate);
    return dist(globalRng());
}
