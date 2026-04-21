/*
 * Graph.cpp
 * ---------
 * Implementation of the Graph class (Dijkstra + path reconstruction).
 */

#include "Graph.h"

Graph::Graph(int n) : N(n), adj(n) {}

void Graph::addEdge(int u, int v, double distance) {
    adj[u].push_back({v, distance});
    adj[v].push_back({u, distance});
}

std::pair<std::vector<double>, std::vector<int>>
Graph::dijkstra(int s) const {
    std::vector<double> dist(N, INF);
    std::vector<int>    prev(N, -1);
    dist[s] = 0.0;

    using Entry = std::pair<double, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> pq;
    pq.push({0.0, s});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        for (const Edge& e : adj[u]) {
            double nd = dist[u] + e.weight;
            if (nd < dist[e.to]) {
                dist[e.to] = nd;
                prev[e.to] = u;
                pq.push({nd, e.to});
            }
        }
    }
    return {dist, prev};
}

std::vector<int> Graph::reconstructPath(const std::vector<int>& prev,
                                         int s, int t) const {
    std::vector<int> path;
    for (int cur = t; cur != -1; cur = prev[cur])
        path.push_back(cur);
    if (path.empty() || path.back() != s) return {};
    std::reverse(path.begin(), path.end());
    return path;
}
