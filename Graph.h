#pragma once

/*
 * Graph.h
 * -------
 * Weighted undirected graph representing the drone charging-station network.
 *
 * Nodes  = charging station ids (0 … N-1)
 * Edges  = aerial routes between stations (weight = physical distance)
 *
 * Provides Dijkstra's shortest-path algorithm used by the
 * distance-based routing policy.
 */

#include "common.h"

// ── Edge ─────────────────────────────────────────────────────────────────────
struct Edge {
    int    to;       // destination node id
    double weight;   // physical distance
};

// ── Graph ─────────────────────────────────────────────────────────────────────
class Graph {
public:
    int N;                              // Number of nodes
    std::vector<std::vector<Edge>> adj; // Adjacency list

    explicit Graph(int n);

    // Add undirected weighted edge
    void addEdge(int u, int v, double distance);

    // Dijkstra from source s.
    // Returns pair(dist[], prev[])
    std::pair<std::vector<double>, std::vector<int>> dijkstra(int s) const;

    // Reconstruct path from s to t using prev[].
    // Returns empty vector if unreachable.
    std::vector<int> reconstructPath(const std::vector<int>& prev,
                                     int s, int t) const;
};
