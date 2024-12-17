#pragma GCC optimize("Ofast,no-stack-protector")
#include <iostream>
#include <string>
#include <vector>

constexpr long long powerOfTwo(long long n) { return 1LL << n; }

struct TerritoryData {
  long long gridSize;
  std::vector<long long> firstQuadrantTerritories;
  std::vector<long long> secondQuadrantTerritories;
  std::vector<long long> crossQuadrantTerritories;
};

void processTerritoryRow(long long row, const std::string& line,
                         long long halfGridSize, TerritoryData& data) {
  for (long long col = 0; col < line.size(); ++col) {
    if (line[col] != '1') continue;

    if (row < halfGridSize && col < halfGridSize) {
      data.firstQuadrantTerritories[row] |= powerOfTwo(col);
    } else if (row >= halfGridSize && col >= halfGridSize) {
      data.secondQuadrantTerritories[row - halfGridSize] |=
          powerOfTwo(col - halfGridSize);
    } else if (row < halfGridSize && col >= halfGridSize) {
      data.crossQuadrantTerritories[row] |= powerOfTwo(col - halfGridSize);
    }
  }
}

TerritoryData readTerritoryInput() {
  TerritoryData data;
  std::cin >> data.gridSize;

  long long halfGridSize = data.gridSize / 2;
  data.firstQuadrantTerritories.resize(halfGridSize);
  data.secondQuadrantTerritories.resize(data.gridSize - halfGridSize);
  data.crossQuadrantTerritories.resize(halfGridSize);

  std::string territoryLine;
  for (long long row = 0; row < data.gridSize; ++row) {
    std::cin >> territoryLine;
    processTerritoryRow(row, territoryLine, halfGridSize, data);
  }
  return data;
}

template <typename T>
std::vector<T> calculateQuadrantMasks(const std::vector<long long>& territories,
                                      long long size, bool isFirstQuadrant) {
  std::vector<T> masks(powerOfTwo(size), isFirstQuadrant ? true : 1);
  long long currentPowerOfTwoIndex = -1;

  for (long long mask = 1; mask < powerOfTwo(size); ++mask) {
    if (!(mask & (mask - 1))) {
      ++currentPowerOfTwoIndex;
      continue;
    }

    long long helpMask = mask - powerOfTwo(currentPowerOfTwoIndex);
    if (!masks[helpMask]) {
      masks[mask] = 0;
      continue;
    }

    long long tempMask = helpMask ^ territories[currentPowerOfTwoIndex];
    if (tempMask != (territories[currentPowerOfTwoIndex] - helpMask)) {
      masks[mask] = 0;
    }
  }

  if constexpr (std::is_same<T, int>::value) {
    masks[0] = 0;
    for (long long bitIndex = 0; bitIndex < size; ++bitIndex) {
      for (long long mask = 1; mask < powerOfTwo(size); ++mask) {
        if (!((mask >> bitIndex) & 1)) {
          masks[mask + powerOfTwo(bitIndex)] += masks[mask];
        }
      }
    }
  }
  return masks;
}

std::vector<long long> calculateCrossQuadrantMasks(
    long long gridSize,
    const std::vector<long long>& crossQuadrantTerritories) {
  std::vector<long long> masks(powerOfTwo(gridSize / 2), 0);
  masks[0] = powerOfTwo(gridSize - gridSize / 2) - 1;

  long long currentPowerOfTwoIndex = -1;
  for (long long mask = 1; mask < powerOfTwo(gridSize / 2); ++mask) {
    if (!(mask & (mask - 1))) {
      masks[mask] = crossQuadrantTerritories[++currentPowerOfTwoIndex];
    } else {
      masks[mask] = masks[mask - powerOfTwo(currentPowerOfTwoIndex)] &
                    crossQuadrantTerritories[currentPowerOfTwoIndex];
    }
  }
  return masks;
}

size_t calculateValidTerritoryDistributions(
    const std::vector<bool>& firstQuadrantMasks,
    const std::vector<int>& secondQuadrantDistribution,
    const std::vector<long long>& crossQuadrantMasks, long long gridSize) {
  size_t validCount = 0;
  for (long long mask = 0; mask < powerOfTwo(gridSize / 2); ++mask) {
    if (firstQuadrantMasks[mask]) {
      validCount += secondQuadrantDistribution[crossQuadrantMasks[mask]] + 1;
    }
  }
  return validCount;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);

  TerritoryData territoryData = readTerritoryInput();

  auto firstQuadrantMasks = calculateQuadrantMasks<bool>(
      territoryData.firstQuadrantTerritories, territoryData.gridSize / 2, true);

  auto secondQuadrantDistribution = calculateQuadrantMasks<int>(
      territoryData.secondQuadrantTerritories,
      territoryData.gridSize - territoryData.gridSize / 2, false);

  auto crossQuadrantMasks = calculateCrossQuadrantMasks(
      territoryData.gridSize, territoryData.crossQuadrantTerritories);

  std::cout << calculateValidTerritoryDistributions(
                   firstQuadrantMasks, secondQuadrantDistribution,
                   crossQuadrantMasks, territoryData.gridSize)
            << '\n';

  return 0;
}
