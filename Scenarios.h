#pragma once

/*
 * Scenarios.h
 * -----------
 * Built-in example scenario and interactive input mode.
 *
 * totalArrivals controls simulation depth:
 *   10000  — fast, ~2-5% error
 *   50000  — balanced (default)
 *   200000 — high accuracy, ~0.5% error
 */

void runExampleScenario(int totalArrivals = 50000);
void runInteractiveMode();
