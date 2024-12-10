#include <algorithm>
#include <iostream>
#include <limits>
#include <set>
#include <vector>

const size_t MAX_NODES = 2e5 + 100;
const long long INF = 1e18 + 7;

std::vector<std::pair<size_t, long long>> adjacencyList[MAX_NODES];
std::vector<long long> distanceFromVirus(MAX_NODES, INF);
std::vector<long long> distanceFromStart(MAX_NODES, INF);
std::vector<size_t> virusSources;

size_t numNodes, numEdges, numVirusSources;

void parseGraphInput() {
  std::cin >> numNodes >> numEdges >> numVirusSources;

  virusSources.resize(numVirusSources);
  distanceFromVirus.assign(numNodes + 1, INF);
  distanceFromStart.assign(numNodes + 1, INF);

  for (size_t i = 0; i < numVirusSources; ++i) {
    size_t sourceNode;
    std::cin >> sourceNode;
    virusSources[i] = sourceNode;
    distanceFromVirus[sourceNode] = 0;
  }

  for (size_t i = 0; i < numEdges; ++i) {
    size_t u, v;
    long long edgeWeight;
    std::cin >> u >> v >> edgeWeight;
    adjacencyList[u].push_back({v, edgeWeight});
    adjacencyList[v].push_back({u, edgeWeight});
  }
}

void calculateVirusPropagation() {
  std::set<std::pair<std::pair<long long, size_t>, size_t>> priorityQueue;

  for (size_t source : virusSources) {
    priorityQueue.insert({{0, 0}, source});
  }

  while (!priorityQueue.empty()) {
    auto [distanceInfo, currentNode] = *priorityQueue.begin();
    auto [distance, sourceType] = distanceInfo;

    priorityQueue.erase(priorityQueue.begin());

    for (const auto& [neighborNode, edgeWeight] : adjacencyList[currentNode]) {
      if (sourceType == 0) {
        if (distanceFromVirus[neighborNode] > distance + edgeWeight) {
          priorityQueue.erase(
              {{distanceFromVirus[neighborNode], 0}, neighborNode});
          distanceFromVirus[neighborNode] = distance + edgeWeight;
          priorityQueue.insert(
              {{distanceFromVirus[neighborNode], 0}, neighborNode});
        }
      }
    }
  }
}

void calculateShortestPathFromStart(size_t startNode, size_t targetNode) {
  std::set<std::pair<std::pair<long long, size_t>, size_t>> priorityQueue;

  distanceFromStart[startNode] = 0;
  priorityQueue.insert({{0, 1}, startNode});

  while (!priorityQueue.empty()) {
    auto [distanceInfo, currentNode] = *priorityQueue.begin();
    auto [distance, sourceType] = distanceInfo;

    priorityQueue.erase(priorityQueue.begin());

    for (const auto& [neighborNode, edgeWeight] : adjacencyList[currentNode]) {
      if (sourceType == 1) {
        if (distanceFromStart[neighborNode] > distance + edgeWeight &&
            distanceFromVirus[neighborNode] > distance + edgeWeight) {
          priorityQueue.erase(
              {{distanceFromStart[neighborNode], 1}, neighborNode});
          distanceFromStart[neighborNode] = distance + edgeWeight;
          priorityQueue.insert(
              {{distanceFromStart[neighborNode], 1}, neighborNode});
        }
      }
    }
  }
}

void findShortestSafeRoute() {
  size_t startNode, targetNode;

  parseGraphInput();

  std::cin >> startNode >> targetNode;

  calculateVirusPropagation();
  calculateShortestPathFromStart(startNode, targetNode);

  if (distanceFromStart[targetNode] == INF) {
    std::cout << -1 << std::endl;
  } else {
    std::cout << distanceFromStart[targetNode] << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  findShortestSafeRoute();

  return 0;
}