#pragma once

/*
 * Comparator.h
 * ------------
 * Prints a formatted side-by-side comparison of theoretical (M/M/1)
 * and simulation results for every charging station.
 *
 * For each metric the relative error  |theory - sim| / theory * 100 %
 * is shown so that convergence quality is immediately visible.
 */

#include "common.h"
#include "ChargingStation.h"

class Comparator {
public:
    // Print the full comparison table for all stations
    static void printComparison(const std::vector<ChargingStation>& stations);

private:
    static void printHeader();
    static void printRow(const ChargingStation& s);
    static std::string pctError(double theory, double sim);
};
