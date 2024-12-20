#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <set>
#include <vector>

double calcDist(std::pair<int, int> a, std::pair<int, int> b) {
  return std::sqrt(std::pow(a.first - b.first, 2) +
                   std::pow(a.second - b.second, 2));
}

// part 1
double TSPNaive(const std::vector<std::pair<int, int>> &cities) {
  std::vector<int> perm(cities.size());
  std::iota(perm.begin(), perm.end(), 0);

  double min_cost = std::numeric_limits<double>::max();

  do {
    double current_cost = 0;
    for (size_t i = 0; i < cities.size(); ++i) {
      current_cost +=
          calcDist(cities[perm[i]], cities[perm[(i + 1) % cities.size()]]);
    }
    min_cost = std::min(min_cost, current_cost);
  } while (std::next_permutation(perm.begin(), perm.end()));

  return min_cost;
}

// part 2
double TSPGreedy(const std::vector<std::pair<int, int>> &cities) {
  std::set<int> visited;
  int current = 0;
  visited.insert(current);

  double total_cost = 0;

  while (visited.size() < cities.size()) {
    double min_distance = std::numeric_limits<double>::max();
    int next_city = -1;

    for (int i = 0; i < cities.size(); ++i) {
      if (visited.count(i) == 0) {
        double distance = calcDist(cities[current], cities[i]);
        if (distance < min_distance) {
          min_distance = distance;
          next_city = i;
        }
      }
    }

    total_cost += min_distance;
    visited.insert(next_city);
    current = next_city;
  }

  total_cost += calcDist(cities[current], cities[0]);
  return total_cost;
}

void runSingleTest(const std::vector<std::pair<int, int>> &cities,
                   const std::string &testName, double expectedNaive,
                   double expectedGreedy, std::ostream &out) {
  double naiveCost = TSPNaive(cities);
  double greedyCost = TSPGreedy(cities);

  assert(std::abs(naiveCost - expectedNaive) < 0.01 &&
         "Exhaustive cost does not match expected!");
  assert(std::abs(greedyCost - expectedGreedy) < 0.01 &&
         "Greedy cost does not match expected!");

  out << "Test Case: " << testName << std::endl;
  out << "Cities: ";
  for (auto &city : cities) {
    out << "(" << city.first << "," << city.second << ") ";
  }
  out << std::endl;
  out << "Exhaustive cost: " << naiveCost << std::endl;
  out << "Exhaustive expected: " << expectedNaive << std::endl;
  out << "Greedy cost: " << greedyCost << std::endl;
  out << "Greedy expected: " << expectedGreedy << std::endl;
  out << "------" << std::endl;
}

int main() {
  int testCount;
  std::cout << "Enter number of test cases: ";
  std::cin >> testCount;

  for (int t = 0; t < testCount; ++t) {
    std::string testName;
    int cityCount;
    double expectedNaive;
    double expectedGreedy;

    std::cin >> testName;
    std::cin >> cityCount;

    std::vector<std::pair<int, int>> cities(cityCount);

    for (int i = 0; i < cityCount; ++i) {
      std::cin >> cities[i].first >> cities[i].second;
    }

    std::cin >> expectedNaive;
    std::cin >> expectedGreedy;

    runSingleTest(cities, testName, expectedNaive, expectedGreedy, std::cout);
  }

  std::cout << "Tests completed" << std::endl;

  return 0;
}