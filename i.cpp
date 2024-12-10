#pragma GCC optimize("Ofast,no-stack-protector")
#include <iostream>
#include <string>
#include <vector>

long long calculatePowerOfTwo(long long n) { return 1 << n; }

struct TerritoryData {
  long long gridSize;
  std::vector<long long> firstQuadrantTerritories;
  std::vector<long long> secondQuadrantTerritories;
  std::vector<long long> crossQuadrantTerritories;
};

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
    for (long long col = 0; col < data.gridSize; ++col) {
      if (territoryLine[col] != '1') continue;

      if (row < halfGridSize && col < halfGridSize)
        data.firstQuadrantTerritories[row] += calculatePowerOfTwo(col);
      else if (row >= halfGridSize && col >= halfGridSize)
        data.secondQuadrantTerritories[row - halfGridSize] +=
            calculatePowerOfTwo(col - halfGridSize);
      else if (row < halfGridSize && col >= halfGridSize)
        data.crossQuadrantTerritories[row] +=
            calculatePowerOfTwo(col - halfGridSize);
    }
  }
  return data;
}

std::vector<bool> calculateFirstQuadrantValidityMasks(
    const std::vector<long long>& firstQuadrantTerritories,
    long long halfGridSize) {
  std::vector<bool> validFirstQuadrantMasks(calculatePowerOfTwo(halfGridSize),
                                            true);
  long long currentPowerOfTwoIndex = -1, tempMask, helpMask;

  for (long long mask = 1; mask < calculatePowerOfTwo(halfGridSize); ++mask) {
    if (!(mask & (mask - 1))) {
      ++currentPowerOfTwoIndex;
      continue;
    }

    helpMask = mask - calculatePowerOfTwo(currentPowerOfTwoIndex);
    if (!validFirstQuadrantMasks[helpMask]) {
      validFirstQuadrantMasks[mask] = false;
      continue;
    }

    tempMask = helpMask ^ firstQuadrantTerritories[currentPowerOfTwoIndex];
    if (tempMask !=
        firstQuadrantTerritories[currentPowerOfTwoIndex] - helpMask) {
      validFirstQuadrantMasks[mask] = false;
    }
  }
  validFirstQuadrantMasks[0] = true;
  return validFirstQuadrantMasks;
}

std::vector<int> calculateSecondQuadrantValidityDistribution(
    const std::vector<long long>& secondQuadrantTerritories,
    long long gridSize) {
  long long halfGridSize = gridSize - gridSize / 2;
  std::vector<int> secondQuadrantValidityDistribution(
      calculatePowerOfTwo(halfGridSize), 1);
  long long currentPowerOfTwoIndex = -1, tempMask, helpMask;

  for (long long mask = 1; mask < calculatePowerOfTwo(halfGridSize); ++mask) {
    if (!(mask & (mask - 1))) {
      ++currentPowerOfTwoIndex;
      continue;
    }

    helpMask = mask - calculatePowerOfTwo(currentPowerOfTwoIndex);
    tempMask = helpMask ^ secondQuadrantTerritories[currentPowerOfTwoIndex];
    if ((tempMask !=
         secondQuadrantTerritories[currentPowerOfTwoIndex] - helpMask) ||
        (!secondQuadrantValidityDistribution[helpMask])) {
      secondQuadrantValidityDistribution[mask] = 0;
    }
  }

  secondQuadrantValidityDistribution[0] = 0;

  for (long long bitIndex = 0; bitIndex < halfGridSize; ++bitIndex) {
    for (long long mask = 1; mask < calculatePowerOfTwo(halfGridSize); ++mask) {
      if (!((mask >> bitIndex) % 2)) {
        secondQuadrantValidityDistribution[mask +
                                           calculatePowerOfTwo(bitIndex)] +=
            secondQuadrantValidityDistribution[mask];
      }
    }
  }

  return secondQuadrantValidityDistribution;
}

std::vector<long long> calculateCrossQuadrantTerritoryMasks(
    long long gridSize,
    const std::vector<long long>& crossQuadrantTerritories) {
  std::vector<long long> crossQuadrantMasks(calculatePowerOfTwo(gridSize / 2),
                                            0);
  crossQuadrantMasks[0] = calculatePowerOfTwo(gridSize - gridSize / 2) - 1;
  long long currentPowerOfTwoIndex = -1;

  for (long long mask = 1; mask < calculatePowerOfTwo(gridSize / 2); ++mask) {
    if (!(mask & (mask - 1)))
      crossQuadrantMasks[mask] =
          crossQuadrantTerritories[++currentPowerOfTwoIndex];
    else
      crossQuadrantMasks[mask] =
          crossQuadrantMasks[mask -
                             calculatePowerOfTwo(currentPowerOfTwoIndex)] &
          crossQuadrantTerritories[currentPowerOfTwoIndex];
  }
  return crossQuadrantMasks;
}

size_t calculateValidTerritoryDistributions(
    const std::vector<bool>& firstQuadrantValidityMasks,
    const std::vector<int>& secondQuadrantValidityDistribution,
    const std::vector<long long>& crossQuadrantMasks, long long gridSize) {
  size_t validDistributionCount = 0;
  for (long long mask = 0; mask < calculatePowerOfTwo(gridSize / 2); ++mask) {
    if (firstQuadrantValidityMasks[mask]) {
      validDistributionCount +=
          secondQuadrantValidityDistribution[crossQuadrantMasks[mask]] + 1;
    }
  }
  return validDistributionCount;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cout.tie(0);
  std::cin.tie(0);

  TerritoryData territoryData = readTerritoryInput();

  auto firstQuadrantValidityMasks = calculateFirstQuadrantValidityMasks(
      territoryData.firstQuadrantTerritories, territoryData.gridSize / 2);

  auto secondQuadrantValidityDistribution =
      calculateSecondQuadrantValidityDistribution(
          territoryData.secondQuadrantTerritories, territoryData.gridSize);

  auto crossQuadrantMasks = calculateCrossQuadrantTerritoryMasks(
      territoryData.gridSize, territoryData.crossQuadrantTerritories);

  std::cout << calculateValidTerritoryDistributions(
                   firstQuadrantValidityMasks,
                   secondQuadrantValidityDistribution, crossQuadrantMasks,
                   territoryData.gridSize)
            << '\n';

  return 0;
}